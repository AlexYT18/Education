#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

int N, T;
unsigned long long** dp;
unsigned long long result_val = 0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_start = PTHREAD_COND_INITIALIZER;
pthread_cond_t cv_done = PTHREAD_COND_INITIALIZER;
int start_epoch = 0, finished = 0;

void* worker(void* arg) {
    int tid = *(int*)arg;
    int seen = -1;
    for (;;) {
        pthread_mutex_lock(&mtx);
        while (seen == start_epoch)
            pthread_cond_wait(&cv_start, &mtx);
        if (start_epoch < 0) {
            pthread_mutex_unlock(&mtx);
            break;
        }
        int j = start_epoch;
        seen = j;
        pthread_mutex_unlock(&mtx);

        for (int r = tid; r < j; r += T) {
            int first = (r == 0 ? j : r + j);
            for (int i = first; i <= N; i += j)
                dp[i][j] = dp[i][j - 1] + dp[i - j][j];
        }

        pthread_mutex_lock(&mtx);
        finished++;
        if (finished == T)
            pthread_cond_signal(&cv_done);
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

void debug_check() {
    unsigned long long** dp_ref = (unsigned long long**)malloc((N + 1) * sizeof(unsigned long long*));
    for (int i = 0; i <= N; i++)
        dp_ref[i] = (unsigned long long*)calloc(N + 1, sizeof(unsigned long long));

    for (int j = 0; j <= N; j++)
        dp_ref[0][j] = 1;

    FILE* dbg = fopen("debug.txt", "w");
    for (int j = 1; j <= N; j++) {
        for (int i = 1; i < j && i <= N; i++)
            dp_ref[i][j] = dp_ref[i][j - 1];
        for (int i = j; i <= N; i++)
            dp_ref[i][j] = dp_ref[i][j - 1] + dp_ref[i - j][j];

        int bad_i = -1;
        for (int i = 0; i <= N; i++)
            if (dp[i][j] != dp_ref[i][j]) { bad_i = i; break; }

        if (bad_i != -1)
            fprintf(dbg, "j=%d i=%d got=%llu ref=%llu\n", j, bad_i, dp[bad_i][j], dp_ref[bad_i][j]);
        else
            fprintf(dbg, "j=%d ok min=%llu mid=%llu last=%llu\n",
                    j, dp[0][j], dp[j <= N ? j : N][j], dp[N][j]);
    }
    fclose(dbg);

    for (int i = 0; i <= N; i++)
        free(dp_ref[i]);
    free(dp_ref);
}

int main() {
    FILE* fin = fopen("input.txt", "r");
    if (!fin) return 1;
    if (fscanf(fin, "%d", &T) != 1) { fclose(fin); return 1; }
    if (fscanf(fin, "%d", &N) != 1) { fclose(fin); return 1; }
    fclose(fin);

    dp = (unsigned long long**)malloc((N + 1) * sizeof(unsigned long long*));
    for (int i = 0; i <= N; i++)
        dp[i] = (unsigned long long*)calloc(N + 1, sizeof(unsigned long long));

    for (int j = 0; j <= N; j++)
        dp[0][j] = 1;

    pthread_t* th = (pthread_t*)malloc(T * sizeof(pthread_t));
    int* ids = (int*)malloc(T * sizeof(int));
    for (int i = 0; i < T; i++) {
        ids[i] = i;
        pthread_create(&th[i], NULL, worker, &ids[i]);
    }

    struct timeval t1, t2;
    gettimeofday(&t1, NULL);

    for (int j = 1; j <= N; j++) {
        for (int i = 1; i < j && i <= N; i++)
            dp[i][j] = dp[i][j - 1];

        pthread_mutex_lock(&mtx);
        start_epoch = j;
        finished = 0;
        pthread_cond_broadcast(&cv_start);
        while (finished < T)
            pthread_cond_wait(&cv_done, &mtx);
        pthread_mutex_unlock(&mtx);
    }

    gettimeofday(&t2, NULL);
    result_val = dp[N][N - 1];

    pthread_mutex_lock(&mtx);
    start_epoch = -1;
    pthread_cond_broadcast(&cv_start);
    pthread_mutex_unlock(&mtx);
    for (int i = 0; i < T; i++)
        pthread_join(th[i], NULL);

    double ms = (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_usec - t1.tv_usec) / 1000.0;

    FILE* fout = fopen("output.txt", "w");
    fprintf(fout, "%d\n%d\n%llu\n", T, N, result_val);
    fclose(fout);

    FILE* tf = fopen("time.txt", "w");
    fprintf(tf, "%.3f\n", ms);
    fclose(tf);

    // --- Debug mode ---
    //debug_check();  

    for (int i = 0; i <= N; i++)
        free(dp[i]);
    free(dp);
    free(th);
    free(ids);
    return 0;
}
