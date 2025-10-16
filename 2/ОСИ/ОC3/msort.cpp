#define _CRT_SECURE_NO_WARNINGS
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>

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

static sem_t sem_ready;
static pthread_mutex_t q_mutex = PTHREAD_MUTEX_INITIALIZER;

static volatile long long tasks_active = 0;
static pthread_mutex_t active_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t done_cond = PTHREAD_COND_INITIALIZER;

void debug(const char* msg)
{
    printf("%s\n", msg);
}

int push_task(Segment s)
{
    int ok = 0;
    pthread_mutex_lock(&q_mutex);
    if ((q_back - q_front) < QUEUE_CAPACITY)
    {
        Q[q_back & (QUEUE_CAPACITY - 1)] = s;
        q_back++;
        ok = 1;
    }
    pthread_mutex_unlock(&q_mutex);
    if (ok) sem_post(&sem_ready);
    return ok;
}

int pop_task(Segment* out)
{
    sem_wait(&sem_ready);
    pthread_mutex_lock(&q_mutex);
    *out = Q[q_front & (QUEUE_CAPACITY - 1)];
    q_front++;
    pthread_mutex_unlock(&q_mutex);
    return 1;
}

void merge(int l, int m, int r)
{
    int n1 = m - l;
    int n2 = r - m;
    int32_t* L = (int32_t*)malloc(sizeof(int32_t) * n1);
    int32_t* R = (int32_t*)malloc(sizeof(int32_t) * n2);
    for (int i = 0; i < n1; ++i) L[i] = A[l + i];
    for (int j = 0; j < n2; ++j) R[j] = A[m + j];
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j]) A[k++] = L[i++];
        else A[k++] = R[j++];
    }
    while (i < n1) A[k++] = L[i++];
    while (j < n2) A[k++] = R[j++];
    free(L);
    free(R);
}

void insertion_sort(int l, int r)
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
}

void dispatch_task(int l, int r, unsigned d)
{
    Segment s;
    s.begin = l;
    s.end = r;
    s.depth = d;
    if (!push_task(s))
    {
        insertion_sort(l, r);
    }
}

void* worker_fn(void* arg)
{
    (void)arg;
    while (1)
    {
        Segment s;
        if (!pop_task(&s)) return NULL;
        if (s.begin < 0) break;

        int l = s.begin;
        int r = s.end;
        int len = r - l;
        if (len <= 1)
        {
            pthread_mutex_lock(&active_mutex);
            if (--tasks_active == 0) pthread_cond_signal(&done_cond);
            pthread_mutex_unlock(&active_mutex);
            continue;
        }
        if (len <= SMALL_THRESHOLD)
        {
            insertion_sort(l, r);
            pthread_mutex_lock(&active_mutex);
            if (--tasks_active == 0) pthread_cond_signal(&done_cond);
            pthread_mutex_unlock(&active_mutex);
            continue;
        }

        int mid = l + (r - l) / 2;
        pthread_mutex_lock(&active_mutex);
        tasks_active += 2;
        pthread_mutex_unlock(&active_mutex);
        dispatch_task(l, mid, s.depth + 1);
        dispatch_task(mid, r, s.depth + 1);
        merge(l, mid, r);

        pthread_mutex_lock(&active_mutex);
        if (--tasks_active == 0) pthread_cond_signal(&done_cond);
        pthread_mutex_unlock(&active_mutex);
    }
    return NULL;
}

int main(void)
{
    if (reading_input("input.txt") != 0) return 1;
    Q = (Segment*)aligned_alloc(64, sizeof(Segment) * QUEUE_CAPACITY);
    if (!Q)
    {
        free(A);
        return 1;
    }
    sem_init(&sem_ready, 0, 0);

    pthread_t* th = (pthread_t*)malloc(sizeof(pthread_t) * threads_cnt);
    for (int i = 0; i < threads_cnt; ++i)
        pthread_create(&th[i], NULL, worker_fn, NULL);

    struct timeval t0, t1;
    gettimeofday(&t0, NULL);

    pthread_mutex_lock(&active_mutex);
    tasks_active = 1;
    pthread_mutex_unlock(&active_mutex);

    Segment first;
    first.begin = 0;
    first.end = (int)N;
    first.depth = 0;
    push_task(first);

    pthread_mutex_lock(&active_mutex);
    while (tasks_active > 0)
        pthread_cond_wait(&done_cond, &active_mutex);
    pthread_mutex_unlock(&active_mutex);

    gettimeofday(&t1, NULL);
    unsigned long long elapsed_ms = (t1.tv_sec - t0.tv_sec) * 1000ULL + (t1.tv_usec - t0.tv_usec) / 1000ULL;

    for (int i = 0; i < threads_cnt; ++i)
    {
        Segment stop = { -1, -1, 0 };
        push_task(stop);
    }

    for (int i = 0; i < threads_cnt; ++i)
        pthread_join(th[i], NULL);
    free(th);

    sem_destroy(&sem_ready);
    free(Q);
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
