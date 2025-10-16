#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    int begin;
    int end;
    unsigned depth;
} Segment;

int threads_cnt = 0;
size_t N = 0;
int32_t* A = NULL;

int reading_input(const char* filename);
int writing_output_time(const char* output_file, const char* time_file, unsigned long long elapsed_ms);

enum { SMALL_THRESHOLD = 1000 };
enum { QUEUE_CAPACITY = 1 << 20 };

static Segment* Q = NULL;
static int q_front = 0;
static int q_back = 0;

static HANDLE sem_ready = NULL;
static HANDLE event_done = NULL;
static CRITICAL_SECTION q_cs;

static volatile LONG64 tasks_active = 0;


int push_task(Segment s)
{
    int ok = 0;
    EnterCriticalSection(&q_cs);
    if ((q_back - q_front) < QUEUE_CAPACITY)
    {
        Q[q_back & (QUEUE_CAPACITY - 1)] = s;
        q_back++;
        ok = 1;
    }
    LeaveCriticalSection(&q_cs);
    if (ok) ReleaseSemaphore(sem_ready, 1, NULL);
    return ok;
}

int pop_task(Segment* out)
{
    DWORD wr = WaitForSingleObject(sem_ready, INFINITE);
    if (wr != WAIT_OBJECT_0) return 0;
    EnterCriticalSection(&q_cs);
    *out = Q[q_front & (QUEUE_CAPACITY - 1)];
    q_front++;
    LeaveCriticalSection(&q_cs);
    return 1;
}

void dispatch_task(int l, int r, unsigned d)
{
    Segment s;
    s.begin = l;
    s.end = r;
    s.depth = d;
    if (!push_task(s))
    {
        int len = r - l;
        for (int a = l + 1; a < r; ++a)
        {
            int32_t key = A[a];
            int b = a - 1;
            while (b >= l && A[b] > key)
            {
                A[b + 1] = A[b];
                --b;
            }
            A[b + 1] = key;
        }
    }
}

DWORD WINAPI worker_fn(LPVOID arg)
{
    (void)arg;
    while (1)
    {
        Segment s;
        if (!pop_task(&s)) return 0;
        if (s.begin < 0) break;

        int l = s.begin;
        int r = s.end;

        while (1)
        {
            int len = r - l;
            if (len <= 1)
            {
                if (InterlockedDecrement64(&tasks_active) == 0)
                    SetEvent(event_done);
                break;
            }

            if (len <= SMALL_THRESHOLD)
            {
                for (int a = l + 1; a < r; ++a)
                {
                    int32_t key = A[a];
                    int b = a - 1;
                    while (b >= l && A[b] > key)
                    {
                        A[b + 1] = A[b];
                        --b;
                    }
                    A[b + 1] = key;
                }

                if (InterlockedDecrement64(&tasks_active) == 0)
                    SetEvent(event_done);
                break;
            }

            int i = l, j = r - 1;
            int mid = l + (r - l) / 2;

            int32_t a = A[l], b = A[mid], c = A[r - 1];
            if (a > b) { int32_t t = a; a = b; b = t; }
            if (b > c) { int32_t t = b; b = c; c = t; }
            if (a > b) { int32_t t = a; a = b; b = t; }
            int32_t pivot = b;

            while (i <= j)
            {
                while (A[i] < pivot) ++i;
                while (A[j] > pivot) --j;
                if (i <= j)
                {
                    int32_t tmp = A[i];
                    A[i] = A[j];
                    A[j] = tmp;
                    ++i; --j;
                }
            }

            int l1 = l, r1 = j + 1;
            int l2 = i, r2 = r;
            int len1 = r1 - l1;
            int len2 = r2 - l2;

            if (len1 < 0) len1 = 0;
            if (len2 < 0) len2 = 0;

            if (len1 >= len2)
            {
                if (len1 > 0)
                {
                    InterlockedIncrement64(&tasks_active);
                    dispatch_task(l1, r1, s.depth + 1);
                }
                l = l2;
                r = r2;
            }
            else
            {
                if (len2 > 0)
                {
                    InterlockedIncrement64(&tasks_active);
                    dispatch_task(l2, r2, s.depth + 1);
                }
                l = l1;
                r = r1;
            }
            // printf("[TID %lu] depth=%u range=%d-%d pivot=%d\n",
            //        GetCurrentThreadId(), s.depth, l, r, pivot);
        }
    }
    return 0;
}

int main(void)
{
    if (reading_input("input.txt") != 0) return 1;

    Q = (Segment*)_aligned_malloc(sizeof(Segment) * QUEUE_CAPACITY, 64);
    if (!Q)
    {
        fprintf(stderr, "Queue alloc failed\n");
        free(A);
        return 1;
    }

    InitializeCriticalSection(&q_cs);

    sem_ready = CreateSemaphoreA(NULL, 0, QUEUE_CAPACITY, NULL);
    event_done = CreateEventA(NULL, TRUE, FALSE, NULL);

    if (!sem_ready || !event_done)
    {
        fprintf(stderr, "Sync objects create failed\n");
        DeleteCriticalSection(&q_cs);
        _aligned_free(Q);
        free(A);
        return 1;
    }

    HANDLE* th = (HANDLE*)malloc(sizeof(HANDLE) * threads_cnt);
    for (int i = 0; i < threads_cnt; ++i)
    {
        th[i] = CreateThread(NULL, 0, worker_fn, NULL, 0, NULL);
    }

    LARGE_INTEGER f, t0, t1;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&t0);

    InterlockedExchange64(&tasks_active, 1);
    Segment first;
    first.begin = 0;
    first.end = (int)N;
    first.depth = 0;
    push_task(first);

    WaitForSingleObject(event_done, INFINITE);
    QueryPerformanceCounter(&t1);

    for (int i = 0; i < threads_cnt; ++i)
    {
        Segment stop;
        stop.begin = -1;
        stop.end = -1;
        stop.depth = 0;
        push_task(stop);
    }

    WaitForMultipleObjects((DWORD)threads_cnt, th, TRUE, INFINITE);
    for (int i = 0; i < threads_cnt; ++i) CloseHandle(th[i]);
    free(th);

    CloseHandle(event_done);
    CloseHandle(sem_ready);
    DeleteCriticalSection(&q_cs);
    _aligned_free(Q);

    double ms = (double)(t1.QuadPart - t0.QuadPart) * 1000.0 / (double)f.QuadPart;
    unsigned long long elapsed_ms = (unsigned long long)(ms + 0.5);

    writing_output_time("output.txt", "time.txt", elapsed_ms);
    free(A);
    return 0;
}

int reading_input(const char* filename)
{
    FILE* fin = fopen(filename, "r");
    if (!fin) return 1;

    unsigned long long Nll = 0ULL;
    if (fscanf(fin, "%d", &threads_cnt) != 1 || threads_cnt <= 0)
    {
        fclose(fin);
        return 1;
    }

    if (fscanf(fin, "%llu", &Nll) != 1 || Nll == 0ULL)
    {
        fclose(fin);
        return 1;
    }

    N = (size_t)Nll;
    A = (int32_t*)malloc(sizeof(int32_t) * N);
    if (!A)
    {
        fclose(fin);
        return 1;
    }

    for (size_t i = 0; i < N; ++i)
    {
        long long v;
        if (fscanf(fin, "%lld", &v) != 1)
        {
            fclose(fin);
            free(A);
            return 1;
        }
        A[i] = (int32_t)v;
    }

    fclose(fin);
    return 0;
}

int writing_output_time(const char* output_file, const char* time_file, unsigned long long elapsed_ms)
{
    FILE* fout = fopen(output_file, "w");
    if (!fout) return 1;

    fprintf(fout, "%d\n", threads_cnt);
    fprintf(fout, "%llu\n", (unsigned long long)N);
    for (size_t i = 0; i < N; ++i)
    {
        if (i) fputc(' ', fout);
        fprintf(fout, "%d", (int)A[i]);
    }
    fputc('\n', fout);
    fclose(fout);

    FILE* ftime = fopen(time_file, "w");
    if (!ftime) return 1;
    fprintf(ftime, "%llu\n", elapsed_ms);
    fclose(ftime);

    return 0;
}
