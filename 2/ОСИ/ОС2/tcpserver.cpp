#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define snprintf _snprintf


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_CLIENTS 64
#define MAX_MSG_LEN 4096

typedef struct {
    SOCKET s;
    struct sockaddr_in addr;
    WSAEVENT ev;
    int active;
} client_t;

static client_t g_clients[MAX_CLIENTS];
static WSAEVENT g_events[MAX_CLIENTS];
static SOCKET g_listen = INVALID_SOCKET;
static int g_shutdown_requested = 0;

static void die_error(const char* point)
{
    int e = WSAGetLastError();
    printf("[FATAL] %s failed, WSA=%d\n", point, e);
    ExitProcess(1);
}

static const char* ip_port_str(const struct sockaddr_in* a, char* buf, size_t sz)
{
    unsigned int ip = ntohl(a->sin_addr.s_addr);
    unsigned short port = ntohs(a->sin_port);
    snprintf(buf, sz, "%u.%u.%u.%u:%u",
        (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF, port);
    return buf;
}

static void wsa_startup(void)
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        die_error("WSAStartup");
}

static void create_listen_socket(unsigned short port)
{
    g_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (g_listen == INVALID_SOCKET)
        die_error("socket");

    struct sockaddr_in a;
    ZeroMemory(&a, sizeof(a));
    a.sin_family = AF_INET;
    a.sin_port = htons(port);
    a.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(g_listen, (struct sockaddr*)&a, sizeof(a)) == SOCKET_ERROR)
        die_error("bind");

    if (listen(g_listen, SOMAXCONN) == SOCKET_ERROR)
        die_error("listen");

    printf("[INFO] Server listening on port %u\n", port);
}

static void client_close(int i)
{
    if (!g_clients[i].active)
        return;

    char who[64];
    printf("[INFO] Closing client %s\n", ip_port_str(&g_clients[i].addr, who, sizeof(who)));

    closesocket(g_clients[i].s);
    WSACloseEvent(g_clients[i].ev);
    g_clients[i].active = 0;
}

static void clients_close_all(void)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
        client_close(i);
    if (g_listen != INVALID_SOCKET)
        closesocket(g_listen);
}

static int recv_full(SOCKET s, void* buf, int len)
{
    int got = 0;
    char* p = (char*)buf;
    while (got < len)
    {
        int n = recv(s, p + got, len - got, 0);
        if (n > 0)
        {
            got += n;
            continue;
        }
        if (n == 0)
            return 0;
        int e = WSAGetLastError();
        if (e == WSAEWOULDBLOCK)
        {
            Sleep(1);
            continue;
        }
        printf("[WARN] recv_full() failed, WSA=%d\n", e);
        return -1;
    }
    return got;
}

int pump_client_read(int idx)
{
    SOCKET cs = g_clients[idx].s;
    char cmd[3];
    int r = recv(cs, cmd, 3, MSG_PEEK);
    if (r <= 0)
        return 1;
    if (r >= 3 && cmd[0] == 'p' && cmd[1] == 'u' && cmd[2] == 't')
    {
        recv(cs, cmd, 3, 0);
        printf("[INFO] Client %d entered PUT mode\n", idx);

        for (;;)
        {
            uint32_t num, msglen;
            uint16_t aa;
            int32_t bbb;
            uint8_t hh, mm, ss;

            r = recv_full(cs, &num, 4);     if (r == -2) return 1; if (r <= 0) return -1;
            r = recv_full(cs, &aa, 2);      if (r == -2) return 1; if (r <= 0) return -1;
            r = recv_full(cs, &bbb, 4);     if (r == -2) return 1; if (r <= 0) return -1;
            r = recv_full(cs, &hh, 1);      if (r == -2) return 1; if (r <= 0) return -1;
            r = recv_full(cs, &mm, 1);      if (r == -2) return 1; if (r <= 0) return -1;
            r = recv_full(cs, &ss, 1);      if (r == -2) return 1; if (r <= 0) return -1;
            r = recv_full(cs, &msglen, 4);  if (r == -2) return 1; if (r <= 0) return -1;

            num = ntohl(num);
            aa = ntohs(aa);
            bbb = ntohl(bbb);
            msglen = ntohl(msglen);
            if (msglen > MAX_MSG_LEN) msglen = MAX_MSG_LEN;

            char* msg = (char*)malloc(msglen + 1);
            if (!msg) return -1;

            r = recv_full(cs, msg, msglen);
            if (r == -2) { free(msg); return 1; }
            if (r <= 0) { free(msg); return -1; }
            msg[msglen] = 0;

            printf("[DEBUG] Received msg #%u: \"%s\"\n", num, msg);

            FILE* f = fopen("msg.txt", "a");
            if (f)
            {
                char who[64];
                fprintf(f, "%s %u %d %02u:%02u:%02u %s\n",
                    ip_port_str(&g_clients[idx].addr, who, sizeof(who)),
                    aa, bbb, hh, mm, ss, msg);
                fclose(f);
            }

            printf("[DEBUG] msg.txt updated\n");
            send(cs, "ok", 2, 0);
            if (strcmp(msg, "stop") == 0)
            {
                printf("[INFO] STOP received from client %d\n", idx);
                g_shutdown_requested = 1;
                free(msg);
                return 0;
            }

            free(msg);
        }
    }
    else
    {
        return 1;
    }
    return 1;
}

static void server_loop(void)
{
    g_events[0] = WSACreateEvent();
    if (WSAEventSelect(g_listen, g_events[0], FD_ACCEPT) == SOCKET_ERROR)
        die_error("WSAEventSelect(listen)");

    while (1)
    {
        int event_count = 1;
        for (int i = 1; i < MAX_CLIENTS; i++)
            if (g_clients[i].active && g_clients[i].ev)
                event_count = i + 1;

        DWORD res = WSAWaitForMultipleEvents(event_count, g_events, FALSE, 100, FALSE);
        if (res == WSA_WAIT_FAILED)
            die_error("WSAWaitForMultipleEvents");

        int idx = res - WSA_WAIT_EVENT_0;

        if (idx == 0)
        {
            struct sockaddr_in a;
            int alen = sizeof(a);
            SOCKET cs = accept(g_listen, (struct sockaddr*)&a, &alen);
            if (cs == INVALID_SOCKET) continue;

            u_long nb = 1;
            ioctlsocket(cs, FIONBIO, &nb);

            int slot = -1;
            for (int i = 1; i < MAX_CLIENTS; i++)
                if (!g_clients[i].active) { slot = i; break; }
            if (slot == -1)
            {
                printf("[WARN] Too many clients, rejecting.\n");
                closesocket(cs);
                continue;
            }

            g_clients[slot].s = cs;
            g_clients[slot].addr = a;
            g_clients[slot].ev = WSACreateEvent();
            g_clients[slot].active = 1;
            if (WSAEventSelect(cs, g_clients[slot].ev, FD_READ | FD_CLOSE) == SOCKET_ERROR)
                die_error("WSAEventSelect(client)");
            g_events[slot] = g_clients[slot].ev;

            char who[64];
            printf("[INFO] Client connected: %s\n", ip_port_str(&a, who, sizeof(who)));
            int res_init = pump_client_read(slot);
            if (res_init < 0)
                client_close(slot);
        }
        else if (idx > 0 && g_clients[idx].active)
        {
            WSANETWORKEVENTS ne;
            WSAEnumNetworkEvents(g_clients[idx].s, g_clients[idx].ev, &ne);

            if (ne.lNetworkEvents & FD_READ)
            {
                int res = pump_client_read(idx);
                if (res < 0)
                    client_close(idx);
            }
            if (ne.lNetworkEvents & FD_CLOSE)
            {
                client_close(idx);
            }
        }

        if (g_shutdown_requested)
        {
            printf("[INFO] Waiting before shutdown...\n");
            Sleep(500);
            clients_close_all();
            printf("[INFO] Graceful shutdown complete.\n");
            return;
        }
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    unsigned short port = (unsigned short)atoi(argv[1]);
    if (port == 0)
    {
        printf("Bad port\n");
        return 1;
    }

    wsa_startup();
    create_listen_socket(port);
    memset(g_clients, 0, sizeof(g_clients));
    memset(g_events, 0, sizeof(g_events));

    printf("[INFO] Server started successfully\n");
    server_loop();

    WSACleanup();
    return 0;
}
