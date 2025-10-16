#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 256
#define MAX_HISTORY 128
#define MAX_MSG 65536

typedef struct {
    char id[64];
    uint32_t ids[MAX_HISTORY];
    int id_count;
    time_t last_active;
} Client;

Client clients[MAX_CLIENTS];
int client_count = 0;

int make_socket(uint16_t port) {
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) return -1;
    int yes = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    int flags = fcntl(s, F_GETFL, 0);
    fcntl(s, F_SETFL, flags | O_NONBLOCK);
    struct sockaddr_in a;
    memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = htonl(INADDR_ANY);
    a.sin_port = htons(port);
    if (bind(s, (struct sockaddr*)&a, sizeof(a)) < 0) {
        close(s);
        return -1;
    }
    return s;
}

Client* find_or_add_client(const char* id) {
    for (int i = 0; i < client_count; i++) {
        if (strcmp(clients[i].id, id) == 0)
            return &clients[i];
    }
    if (client_count >= MAX_CLIENTS) return NULL;
    Client* c = &clients[client_count++];
    memset(c, 0, sizeof(Client));
    strncpy(c->id, id, sizeof(c->id) - 1);
    c->last_active = time(NULL);
    return c;
}

int id_seen(Client* c, uint32_t id) {
    for (int i = 0; i < c->id_count; i++)
        if (c->ids[i] == id) return 1;
    return 0;
}

void add_id(Client* c, uint32_t id) {
    if (c->id_count < MAX_HISTORY)
        c->ids[c->id_count++] = id;
    else {
        memmove(c->ids, c->ids + 1, (MAX_HISTORY - 1) * sizeof(uint32_t));
        c->ids[MAX_HISTORY - 1] = id;
    }
}

void send_ack(int s, struct sockaddr_in* cli, socklen_t clen, Client* c) {
    uint32_t buf[20];
    int n = 0;
    for (int i = c->id_count - 1; i >= 0 && n < 20; i--)
        buf[n++] = htonl(c->ids[i]);
    sendto(s, buf, n * 4, 0, (struct sockaddr*)cli, clen);
}

void cleanup_clients() {
    time_t now = time(NULL);
    for (int i = 0; i < client_count; ) {
        if (now - clients[i].last_active > 30) {
            clients[i] = clients[--client_count];
        }
        else {
            i++;
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port_first> <port_last>\n", argv[0]);
        return 1;
    }

    int port_first = atoi(argv[1]);
    int port_last = atoi(argv[2]);
    if (port_first <= 0 || port_last < port_first || port_last > 65535) {
        fprintf(stderr, "Bad port range\n");
        return 1;
    }

    int sockc = port_last - port_first + 1;
    int socks[sockc];
    for (int i = 0; i < sockc; i++) {
        int s = make_socket(port_first + i);
        if (s < 0) {
            perror("bind");
            return 1;
        }
        socks[i] = s;
    }

    FILE* fmsg;
    int running = 1;

    while (running) {
        fd_set rfds;
        FD_ZERO(&rfds);
        int maxfd = -1;
        for (int i = 0; i < sockc; i++) {
            FD_SET(socks[i], &rfds);
            if (socks[i] > maxfd) maxfd = socks[i];
        }
        struct timeval tv = { 1, 0 };
        int r = select(maxfd + 1, &rfds, NULL, NULL, &tv);
        if (r < 0 && errno != EINTR) break;

        for (int i = 0; i < sockc; i++) {
            if (!FD_ISSET(socks[i], &rfds)) continue;
            struct sockaddr_in cli;
            socklen_t clen = sizeof(cli);
            char buf[MAX_MSG];
            int n = recvfrom(socks[i], buf, sizeof(buf), 0, (struct sockaddr*)&cli, &clen);
            if (n <= 0) continue;
            if (n < 17) continue;

            uint32_t idx_n, len_n;
            uint16_t AA_n;
            int32_t BBB_n;
            uint8_t hh, mm, ss;
            size_t off = 0;

            memcpy(&idx_n, buf + off, 4); off += 4;
            memcpy(&AA_n, buf + off, 2); off += 2;
            memcpy(&BBB_n, buf + off, 4); off += 4;
            hh = buf[off++]; mm = buf[off++]; ss = buf[off++];
            memcpy(&len_n, buf + off, 4); off += 4;

            uint32_t idx = ntohl(idx_n);
            uint16_t AA = ntohs(AA_n);
            int32_t BBB = ntohl(BBB_n);
            uint32_t mlen = ntohl(len_n);
            if (off + mlen > (size_t)n) continue;

            char msg[1024];
            if (mlen >= sizeof(msg)) mlen = sizeof(msg) - 1;
            memcpy(msg, buf + off, mlen);
            msg[mlen] = 0;

            char id[64];
            snprintf(id, sizeof(id), "%s:%u", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));

            Client* c = find_or_add_client(id);
            if (!c) continue;
            c->last_active = time(NULL);

            if (!id_seen(c, idx)) {
                add_id(c, idx);
                fmsg = fopen("msg.txt", "a");
                if (fmsg) {
                    fprintf(fmsg, "%s %u %d %02u:%02u:%02u %s\n",
                        id, AA, BBB, hh, mm, ss, msg);
                    fclose(fmsg);
                }
            }

            send_ack(socks[i], &cli, clen, c);

            if (strcmp(msg, "stop") == 0) {
                running = 0;
                break;
            }
        }
        cleanup_clients();
    }

    for (int i = 0; i < sockc; i++) close(socks[i]);
    return 0;
}
