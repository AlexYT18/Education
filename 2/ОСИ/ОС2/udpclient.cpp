#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#define WEBHOST "google.com"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#pragma comment(lib, "ws2_32.lib")
#else // LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <stdio.h>

//#define SERVER_PORT 9000
//#define SERVER_IP "127.0.0.1"
#define DEAFAULT_BUFLEN 10240
#define MAX_LINE 1024

typedef struct {
    uint32_t index;
    uint16_t AA;
    int32_t  BBB;
    uint8_t  hh, mm, ss;
    const char* msgp;
    uint32_t msglen;
    char* msgown;
} item_t;


int handshake(int sockfd);
int wait_ok(int sockfd);
int read_lines_and_send(int sockfd, const char* filename);
int recv_response(int sockfd, const char* outfile);
static int send_all(int s, const void* buf, size_t len);
int send_one_message(int s, uint32_t index, uint16_t AA, int32_t BBB,
    uint8_t hh, uint8_t mm, uint8_t ss,
    const char* msg, uint32_t msg_len);
static void trio_check(char* s);
static int parse_line(const char* line, uint16_t* AA, int32_t* BBB,
    uint8_t* hh, uint8_t* mm, uint8_t* ss,
    const char** msg, uint32_t* msg_len);
int run_get(const char* dst, const char* outfilename);

static int connect_with_retries(int sockfd, const struct sockaddr_in* addr)
{
    for (int i = 0; i < 10; ++i) {
        if (connect(sockfd, (const struct sockaddr*)addr, sizeof(*addr)) == 0)
            return 0;
#ifdef _WIN32
        Sleep(10);
#else
        usleep(10000);
#endif
    }
    perror("connect");
    return -1;
}

int load_messages(const char* filename, item_t** out, size_t* outn)
{
    *out = NULL;
    *outn = 0;
    FILE* f = fopen(filename, "rb");
    if (!f) { perror("fopen"); return -1; }

    char line[4096];
    uint32_t idx = 0;
    size_t cap = 0, n = 0;
    item_t* arr = NULL;

    while (fgets(line, sizeof(line), f)) {
        size_t L = strlen(line);
        while (L && (line[L - 1] == '\n' || line[L - 1] == '\r')) line[--L] = 0;
        if (!L) continue;

        uint16_t AA; int32_t BBB; uint8_t hh, mm, ss;
        const char* msg; uint32_t msglen;
        if (parse_line(line, &AA, &BBB, &hh, &mm, &ss, &msg, &msglen) != 0)
            continue;

        if (n == cap) {
            cap = cap ? cap * 2 : 64;
            item_t* tmp = (item_t*)realloc(arr, cap * sizeof(item_t));
            if (!tmp) { fclose(f); free(arr); return -1; }
            arr = tmp;
        }

        arr[n].index = idx++;
        arr[n].AA = AA;
        arr[n].BBB = BBB;
        arr[n].hh = hh;
        arr[n].mm = mm;
        arr[n].ss = ss;
        arr[n].msgown = (char*)malloc(msglen + 1);
        if (!arr[n].msgown) {
            fclose(f);
            for (size_t i = 0; i < n; i++) free(arr[i].msgown);
            free(arr);
            return -1;
        }
        memcpy(arr[n].msgown, msg, msglen);
        arr[n].msgown[msglen] = 0;
        arr[n].msgp = arr[n].msgown;
        arr[n].msglen = msglen;
        n++;
    }

    fclose(f);
    *out = arr;
    *outn = n;
    return 0;
}

int send_one_datagram(int s, const struct sockaddr_in* to,
    uint32_t index, uint16_t AA, int32_t BBB,
    uint8_t hh, uint8_t mm, uint8_t ss,
    const char* msg, uint32_t msg_len)
{
    if (hh > 23 || mm > 59 || ss > 59) return -1;

    uint8_t header[17];
    size_t off = 0;
    uint32_t idx_n = htonl(index);
    uint16_t aa_n = htons(AA);
    int32_t  bbb_n = htonl(BBB);
    uint32_t len_n = htonl(msg_len);

    memcpy(header + off, &idx_n, 4); off += 4;
    memcpy(header + off, &aa_n, 2);  off += 2;
    memcpy(header + off, &bbb_n, 4); off += 4;
    header[off++] = hh;
    header[off++] = mm;
    header[off++] = ss;
    memcpy(header + off, &len_n, 4); off += 4;

    size_t total = 17 + msg_len;
    uint8_t* pkt = (uint8_t*)malloc(total);
    if (!pkt) return -1;
    memcpy(pkt, header, 17);
    if (msg_len) memcpy(pkt + 17, msg, msg_len);

    int r = sendto(s, (const char*)pkt, (int)total, 0,
        (const struct sockaddr*)to, sizeof(*to));
    free(pkt);
    if (r != (int)total) return -1;

    return 0;
}


static int send_all(int s, const void* buf, size_t len)
{
    const unsigned char* p = (const unsigned char*)buf;
    size_t sent = 0;
    while (sent < len)
    {
        int r = send(s, (const char*)(p + sent), (int)(len - sent), 0);
        if (r <= 0)
        {
            perror("send");
            return -1;
        }
        sent += (size_t)r;
    }
    return 0;
}

int send_one_message(int s, uint32_t index, uint16_t AA, int32_t BBB,
    uint8_t hh, uint8_t mm, uint8_t ss,
    const char* msg, uint32_t msg_len)
{
    if (hh > 23 || mm > 59 || ss > 59)
    {
        fprintf(stderr, "invalid time %02u:%02u:%02u\n", hh, mm, ss);
        return -1;
    }

    unsigned char header[17];
    size_t off = 0;
    uint32_t idx_n = htonl(index);
    uint16_t aa_n = htons(AA);
    int32_t bbb_n = htonl((int32_t)BBB);
    uint32_t len_n = htonl(msg_len);

    memcpy(header + off, &idx_n, 4); off += 4;
    memcpy(header + off, &aa_n, 2); off += 2;
    memcpy(header + off, &bbb_n, 4); off += 4;
    header[off++] = hh;
    header[off++] = mm;
    header[off++] = ss;
    memcpy(header + off, &len_n, 4); off += 4; // off == 17
    if (send_all(s, header, off) < 0) return -1;
    if (msg_len > 0 && send_all(s, msg, msg_len) < 0) return -1;
    printf("sent message idx=%u, AA = %u, BBB = %d %02u:%02u:%02u len=%u\n", index, AA, BBB, hh, mm, ss, msg_len);
    return 0;
}

static int parse_ip_port(const char* s, struct sockaddr_in* out)
{
    char ip[64]; // socket(ip:port)
    ip[0] = 0;
    int port = 0;
    const char* colon = strrchr(s, ':');
    if (!colon) return -1;

    size_t iplen = (size_t)(colon - s);
    if (iplen >= sizeof(ip)) return -1;

    memcpy(ip, s, iplen);
    ip[iplen] = '\0';
    port = atoi(colon + 1);

    if (port <= 0 || port > 65535) return -1;

    memset(out, 0, sizeof(*out));
    out->sin_family = AF_INET;
    out->sin_port = htons((uint16_t)port);
    if (inet_pton(AF_INET, ip, &out->sin_addr) != 1) return -1;
    return 0;
}


static void trio_check(char* s)
{
    size_t n = strlen(s);
    while (n && (s[n - 1] == '\n' || s[n - 1] == '\r')) s[--n] = '\0';

}


static int parse_line(const char* line_in,
    uint16_t* AA, int32_t* BBB,
    uint8_t* hh, uint8_t* mm, uint8_t* ss,
    const char** msg, uint32_t* msg_len)
{
    if (!line_in) return -1;

    while (*line_in == ' ') line_in++;

    unsigned long long aa_ull = 0;
    long long bbb_ll = 0;
    char timebuf[16] = { 0 };
    int off = 0;

    int nread = sscanf(line_in, " %llu %lld %15s %n",
        &aa_ull, &bbb_ll, timebuf, &off);
    if (nread < 3) {
        fprintf(stderr, "[parse_line] format fail (need AA BBB hh:mm:ss)\n");
        return -1;
    }

    // проверка диапазонов
    if (aa_ull > 65535ULL) {
        fprintf(stderr, "[parse_line] AA out of range: %llu\n", aa_ull);
        return -1;
    }
    if (bbb_ll < (long long)INT32_MIN || bbb_ll >(long long)INT32_MAX) {
        fprintf(stderr, "[parse_line] BBB out of range: %lld\n", bbb_ll);
        return -1;
    }

    int H = -1, M = -1, S = -1;
    if (sscanf(timebuf, "%d:%d:%d", &H, &M, &S) != 3 ||
        (H < 0 || H > 23 || M < 0 || M > 59 || S < 0 || S > 59))
    {
        fprintf(stderr, "[parse_line] bad time: %s\n", timebuf);
        return -1;
    }

    const char* p = line_in + off;
    while (*p == ' ') p++;

    *AA = (uint16_t)aa_ull;
    *BBB = (int32_t)bbb_ll;
    *hh = (uint8_t)H;
    *mm = (uint8_t)M;
    *ss = (uint8_t)S;

    *msg = p;
    *msg_len = (uint32_t)strlen(p);
    return 0;
}


static int send_messages_from_file(int s, const char* filename, int* out_sent) {
    if (out_sent) *out_sent = 0;

    FILE* f = fopen(filename, "rb");
    if (!f) { perror("fopen"); return -1; }

    char line[4096];
    uint32_t index = 0;
    int sent_cnt = 0;

    while (fgets(line, sizeof(line), f)) {
        trio_check(line);
        if (line[0] == '\0') continue;

        uint16_t AA; int32_t BBB;
        uint8_t hh, mm, ss;
        const char* msg; uint32_t msg_len;

        if (parse_line(line, &AA, &BBB, &hh, &mm, &ss, &msg, &msg_len) != 0) {
            continue;
        }

        if (send_one_message(s, index, AA, BBB, hh, mm, ss, msg, msg_len) != 0) {
            fclose(f);
            return -1;
        }
        ++index;
        ++sent_cnt;
    }

    fclose(f);
    if (out_sent) *out_sent = sent_cnt;
    return sent_cnt;
}


int wait_ok(int sockfd)
{
    char buf[2];
    int rec = 0;
    while (rec < 2)
    {
        int r = recv(sockfd, buf + rec, 2 - rec, 0);
        if (r <= 0) // r == 0 close connect
        {
            perror("Error recv from wait_ok\n");
            return -1;
        }
        rec += r;
    }

    if (buf[0] == 'o' && buf[1] == 'k')
    {
        printf("OK from servern\n");
        return 0;
    }
    return -1;
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s IP:Port file.txt\n", argv[0]);
        return 1;
    }

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return 1;
    }

    struct sockaddr_in servaddr;
    if (parse_ip_port(argv[1], &servaddr) < 0) {
        fprintf(stderr, "Bad address: %s\n", argv[1]);
        WSACleanup();
        return 1;
    }

    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) {
        fprintf(stderr, "socket failed\n");
        WSACleanup();
        return 1;
    }

    item_t* msgs = NULL; size_t N = 0;
    if (load_messages(argv[2], &msgs, &N) != 0) {
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    size_t need = (N < 20) ? N : 20;
    int* confirmed = (int*)calloc(N ? N : 1, sizeof(int));
    if (!confirmed) {
        for (size_t i = 0; i < N; i++) free(msgs[i].msgown);
        free(msgs);
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    for (;;) {
        size_t have = 0;
        for (size_t i = 0; i < N; i++) if (confirmed[i]) have++;
        if (have >= need) break;

        for (size_t i = 0; i < N; i++) {
            if (!confirmed[i]) {
                send_one_datagram(sockfd, &servaddr,
                    msgs[i].index, msgs[i].AA, msgs[i].BBB,
                    msgs[i].hh, msgs[i].mm, msgs[i].ss,
                    msgs[i].msgp, msgs[i].msglen);
                //printf("Sent #%u: \"%s\"\n", msgs[i].index, msgs[i].msgp);

            }
        }

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(sockfd, &rfds);
        struct timeval tv; tv.tv_sec = 0; tv.tv_usec = 100000;
        int r = select(0, &rfds, NULL, NULL, &tv);
        if (r > 0 && FD_ISSET(sockfd, &rfds)) {
            char buf[4096];
            struct sockaddr_in from; int fromlen = sizeof(from);
            int n = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&from, &fromlen);
            if (n > 0 && (n % 4) == 0) {
                for (int off = 0; off < n; off += 4) {
                    uint32_t id;
                    memcpy(&id, buf + off, 4);
                    id = ntohl(id);
                    if (id < N) confirmed[id] = 1;
                }
            }
        }
    }

    for (size_t i = 0; i < N; i++) free(msgs[i].msgown);
    free(msgs);
    free(confirmed);

    closesocket(sockfd);
    WSACleanup();
    return 0;
}


int run_get(const char* dst, const char* outfilename)
{
    struct sockaddr_in servaddr;
    if (parse_ip_port(dst, &servaddr) < 0)
    {
        fprintf(stderr, "Invalid address format: %s\n", dst);
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return 1;
    }

    if (connect_with_retries(sockfd, &servaddr) < 0)
    {
        closesocket(sockfd);
        return 1;
    }

    const char* cmd = "get";
    if (send_all(sockfd, cmd, 3) < 0)
    {
        perror("send(get)");
        closesocket(sockfd);
        return 1;
    }

    FILE* f = fopen(outfilename, "wb");
    if (!f)
    {
        perror("fopen");
        closesocket(sockfd);
        return 1;
    }

    while (1)
    {
        uint32_t idx_n, msglen_n;
        uint16_t AA_n;
        int32_t BBB_n;
        uint8_t hh, mm, ss;

        int r = recv(sockfd, (char*)&idx_n, 4, MSG_WAITALL);
        if (r <= 0) break;

        recv(sockfd, (char*)&AA_n, 2, MSG_WAITALL);
        recv(sockfd, (char*)&BBB_n, 4, MSG_WAITALL);
        recv(sockfd, (char*)&hh, 1, MSG_WAITALL);
        recv(sockfd, (char*)&mm, 1, MSG_WAITALL);
        recv(sockfd, (char*)&ss, 1, MSG_WAITALL);
        recv(sockfd, (char*)&msglen_n, 4, MSG_WAITALL);


        uint32_t idx = ntohl(idx_n);
        uint16_t AA = ntohs(AA_n);
        int32_t BBB = ntohl(BBB_n);
        uint32_t msglen = ntohl(msglen_n);

        char* msg = (char*)malloc(msglen + 1);
        if (!msg) break;

        recv(sockfd, msg, msglen, MSG_WAITALL);
        msg[msglen] = '\0';

        fprintf(f, "%s:%u %u %d %02u:%02u:%02u %s\n",
            inet_ntoa(servaddr.sin_addr),
            ntohs(servaddr.sin_port),
            AA, BBB, hh, mm, ss, msg);

        printf("Received #%u: %u %d %02u:%02u:%02u %s\n",
            idx, AA, BBB, hh, mm, ss, msg);

        free(msg);
    }

    fclose(f);
    closesocket(sockfd);
    printf("All messages saved to %s\n", outfilename);
    return 0;
}
