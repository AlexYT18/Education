// tcpclient.cpp — Linux (g++ 5.x)
// Отправляет все валидные строки из файла по протоколу 'put'.
//
// Формат строки файла: "AA BBB hh:mm:ss Message"
//   AA  : 0..65535  (uint16_t)
//   BBB : -2147483648..2147483647 (int32_t)
//   hh  : 0..23
//   mm  : 0..59
//   ss  : 0..59
//   Message: любая строка до конца (может быть пустой)
//
// Формат пакета к серверу (после 'put'):
//   [4] index  (uint32 BE, htonl)
//   [2] AA     (uint16 BE, htons)
//   [4] BBB    (int32  BE, htonl на uint32_t-касте)
//   [1] hh
//   [1] mm
//   [1] ss
//   [4] len    (uint32 BE, длина Message)
//   [N] Message (ровно N байт, без '\0')


#define _CRT_SECURE_NO_WARNINGS_
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
#include <unistd.h>
#include <stdint.h>
#include <limits.h>
#include <unistd.h>

//#define SERVER_PORT 9000
//#define SERVER_IP "127.0.0.1"
#define DEAFAULT_BUFLEN 10240
#define MAX_LINE 1024

int handshake(int sockfd);
int send_put(int sockfd);
int wait_ok(int sockfd);
int read_lines_and_send(int sockfd, const char *filename);
int recv_response(int sockfd, const char* outfile);
static int send_all(int s, const void *buf, size_t len);
int send_one_message(int s, uint32_t index, uint16_t AA, int32_t BBB, 
                        uint8_t hh, uint8_t mm, uint8_t ss,
                        const char *msg, uint32_t msg_len);
static void trio_check(char *s);
static int parse_line(const char* line, uint16_t* AA, int32_t* BBB,
                        uint8_t* hh, uint8_t* mm, uint8_t* ss,
                        const char** msg, uint32_t* msg_len);

/*int handshake(int sockfd)
{
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);//kostil

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Connect in handshake error\n");
        return -1;
    }
    return 0;
}*/

int send_put(int sockfd)
{
    const char *msg = "put";
    if(send(sockfd, msg, strlen(msg), 0) != (ssize_t)strlen(msg))
    {
        perror("Put don't send\n");
        return -1;
    }
    printf("put send\n");
    return 0;
}

static int send_all(int s, const void *buf, size_t len)// all buf to one call
{
    const unsigned char *p = (const unsigned char*)buf;
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
                        const char *msg, uint32_t msg_len)
{
    if (hh > 23 || mm > 59 || ss > 59)
    {
        fprintf(stderr, "invalid time %02u:%02u:%02u\n",hh, mm, ss);
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
    printf("sent message idx=%u, AA = %u, BBB = %d %02u:%02u:%02u len=%u\n",index,AA,BBB,hh,mm,ss,msg_len);
    return 0;
}


static int wait_n_ok(int sockfd, int expected)
{
    int got = 0;
    char buf[2];

    while (got < expected) {
        int rec = 0;
        while (rec < 2) {
            int r = recv(sockfd, buf + rec, 2 - rec, 0);
            if (r <= 0) { perror("recv (ok)"); return -1; }
            rec += r;
        }
        if (buf[0] == 'o' && buf[1] == 'k') {
            got++;
        } else {
            fprintf(stderr, "protocol: not 'ok' (%02x %02x)\n",
                    (unsigned char)buf[0], (unsigned char)buf[1]);
            return -1;
        }
    }
    printf("Received %d ok\n", got);
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

static int connect_with_retries(int sockfd, const struct sockaddr_in* addr)
{
    for (int i = 0; i < 10; ++i) {
        if (connect(sockfd, (const struct sockaddr*)addr, sizeof(*addr)) == 0)
            return 0;
        usleep(10000); 
    }
    perror("connect");
    return -1;
}
//------ test

static void trio_check(char *s)
{
    size_t n = strlen(s);
    while (n && (s[n-1] == '\n' || s[n-1] == '\r')) s[--n] = '\0';

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
    char timebuf[16] = {0};
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
    if (bbb_ll < (long long)INT32_MIN || bbb_ll > (long long)INT32_MAX) {
        fprintf(stderr, "[parse_line] BBB out of range: %lld\n", bbb_ll);
        return -1;
    }

    int H=-1, M=-1, S=-1;
    if (sscanf(timebuf, "%d:%d:%d", &H, &M, &S) != 3 ||
        (H < 0 || H > 23 || M < 0 || M > 59 || S < 0 || S > 59)) 
    {
        fprintf(stderr, "[parse_line] bad time: %s\n", timebuf);
        return -1;
    }

    const char *p = line_in + off;
    while (*p == ' ') p++;

    *AA  = (uint16_t)aa_ull;
    *BBB = (int32_t)bbb_ll;
    *hh  = (uint8_t)H;
    *mm  = (uint8_t)M;
    *ss  = (uint8_t)S;

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

    if(buf[0] == 'o' && buf[1] == 'k')
    {
        printf("OK from servern\n");
        return 0;
    }
    return -1;
}

int main(int argc, char** argv)
{
    
    if (argc != 3)
    {
        fprintf(stderr, "Use: %s [IP:POrt] [file.txt]\n",argv[0]);
        return 1;
    }
    const char* dst = argv[1];
    const char* filename = argv[2];
    int sent = 0;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);


    if (sockfd < 0) 
    {
        perror("Socket error");
        return 1;
    }

    struct sockaddr_in servaddr;
    if (parse_ip_port(dst,&servaddr) < 0)
    {
        fprintf(stderr, "Wrong address: %s (need 0.0.0.0:0000)\n",dst);
        close(sockfd);
        return 1;
    }
    if (connect_with_retries(sockfd, &servaddr) < 0)
    {
        close (sockfd);
        return 1;
    }
    //if (handshake(sockfd) < 0)
    //{
    //    close(sockfd);
    //    return 1;
    //}

    if (send_put(sockfd) < 0)
    {
        close(sockfd);
        return 1;
    }
    
    if (send_messages_from_file(sockfd, filename, &sent) < 0)
    {
        fprintf(stderr, "send error\n");
        close(sockfd);
        return 1;

    }
    printf("Sent %d message from file\n", sent);



    if (wait_n_ok(sockfd, sent) < 0)
    {
        fprintf(stderr, "FAIL end_ok from server\n");
        close(sockfd);
        return 1;
    }

    if(send_one_message(sockfd, (uint32_t)sent, (uint16_t)0,(int32_t)0,(uint8_t)0,(uint8_t)0,(uint8_t)0, (const char*)"stop", (uint32_t)4) == 0)
    {
        if(wait_ok(sockfd) == 0) printf("Sent stop mess and final\n");
        else fprintf(stderr,"Stop message sent,but no final ok\n");

    }
    else fprintf(stderr, "Fail to send stop message");
    close(sockfd);
    printf("Connection closed\n");
    return 0;
}