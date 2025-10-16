#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdint.h>
#pragma comment(lib, "winmm.lib")

#define N 5

typedef enum { ST_T=0, ST_E=1 } State;
static HANDLE g_print_mtx;
static HANDLE start_evt[N];
static HANDLE done_evt[N];
static DWORD g_TOTAL_ms = 0;
static DWORD g_PHIL_ms  = 0;
static ULONGLONG g_t0   = 0;
static volatile State g_state[N];

static DWORD now_ms(void){
    ULONGLONG t = GetTickCount64();
    DWORD v = (DWORD)((t >= g_t0) ? (t - g_t0) : 0);
    return v + 1; // компенсация системного лага Windows
}

static void log_transition(int idx, char from, char to){
    DWORD t = now_ms();
    WaitForSingleObject(g_print_mtx, INFINITE);
    printf("%lu:%d:%c->%c\n", (unsigned long)t, idx+1, from, to);
    fflush(stdout);
    ReleaseMutex(g_print_mtx);
}

typedef struct { int id; } ThArg;

static DWORD WINAPI philosopher(LPVOID p){
    ThArg* a = (ThArg*)p;
    int i = a->id;
    for(;;){
        DWORD w = WaitForSingleObject(start_evt[i], INFINITE);
        if (w != WAIT_OBJECT_0) break;
        g_state[i] = ST_E;
        log_transition(i, 'T', 'E');
        Sleep(g_PHIL_ms);
        g_state[i] = ST_T;
        log_transition(i, 'E', 'T');
        SetEvent(done_evt[i]);
    }
    return 0;
}

static const int waves[5][2] = {
    {1,4},
    {2,0},
    {3,1},
    {4,2},
    {0,3}
};

int main(int argc, char** argv){
    if (argc != 3){
        fprintf(stderr, "Usage: phil TOTAL PHIL\n");
        return 1;
    }
    unsigned long T=0, P=0;
    if (sscanf(argv[1], "%lu", &T)!=1) return 2;
    if (sscanf(argv[2], "%lu", &P)!=1) return 3;
    g_TOTAL_ms = (DWORD)T;
    g_PHIL_ms  = (DWORD)P;

    timeBeginPeriod(1);
    g_t0 = GetTickCount64();
    g_print_mtx = CreateMutexA(NULL, FALSE, NULL);
    for (int i=0;i<N;i++){
        start_evt[i] = CreateEventA(NULL, FALSE, FALSE, NULL);
        done_evt[i]  = CreateEventA(NULL, FALSE, FALSE, NULL);
        g_state[i] = ST_T;
    }

    HANDLE th[N];
    ThArg args[N];
    for (int i=0;i<N;i++){
        args[i].id = i;
        th[i] = CreateThread(NULL, 0, philosopher, &args[i], 0, NULL);
    }

    Sleep(g_PHIL_ms);
    DWORD drift = now_ms() - g_PHIL_ms;
    if (drift <= 50) g_t0 += drift;

    int wave_idx = 0;
    while (now_ms() <= g_TOTAL_ms){
        int i = waves[wave_idx][0];
        int j = waves[wave_idx][1];
        SetEvent(start_evt[i]);
        Sleep(1);
        SetEvent(start_evt[j]);
        HANDLE two[2] = { done_evt[i], done_evt[j] };
        WaitForMultipleObjects(2, two, TRUE, g_PHIL_ms*2);
        ResetEvent(done_evt[i]);
        ResetEvent(done_evt[j]);
        wave_idx = (wave_idx + 1) % 5;

        ULONGLONG target = g_t0 + (ULONGLONG)(g_PHIL_ms * (wave_idx + 1));
        ULONGLONG now64 = GetTickCount64();
        while (now64 < target) {
            Sleep(0);
            now64 = GetTickCount64();
        }

        if (now_ms() >= g_TOTAL_ms) break;
    }

    for (int i=0;i<N;i++){
        TerminateThread(th[i], 0);
        CloseHandle(th[i]);
    }
    for (int i=0;i<N;i++){
        CloseHandle(start_evt[i]);
        CloseHandle(done_evt[i]);
    }
    CloseHandle(g_print_mtx);
    timeEndPeriod(1);
    return 0;
}
