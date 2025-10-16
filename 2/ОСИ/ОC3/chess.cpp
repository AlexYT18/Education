#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int nextIdx;
    int placed;
    unsigned char* occ;
} Node;

typedef struct QN {
    Node v;
    struct QN* next;
} QN;

int T_threads, N, L, K;
int** attack_of;
int* attack_count;
int* preset_x;
int* preset_y;

long long total_solutions = 0;
pthread_mutex_t total_mx = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t q_mx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t start_mx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t done_mx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t q_cv = PTHREAD_COND_INITIALIZER;
pthread_cond_t start_cv = PTHREAD_COND_INITIALIZER;
pthread_cond_t done_cv = PTHREAD_COND_INITIALIZER;

int start_flag = 0;
int all_work_done = 0;
int active_workers = 0;

QN* q_head = NULL;
QN* q_tail = NULL;

static inline int idx_of(int x, int y) { return x * N + y; }

void queue_push(Node n) {
    QN* p = (QN*)malloc(sizeof(QN));
    p->v = n;
    p->next = NULL;
    if (!q_tail)
        q_head = q_tail = p;
    else {
        q_tail->next = p;
        q_tail = p;
    }
}

int queue_pop(Node* out) {
    if (!q_head) return 0;
    QN* p = q_head;
    *out = p->v;
    q_head = p->next;
    if (!q_head) q_tail = NULL;
    free(p);
    return 1;
}

void queue_clear_leftovers() {
    while (q_head) {
        QN* p = q_head;
        q_head = p->next;
        if (p->v.occ) free(p->v.occ);
        free(p);
    }
    q_tail = NULL;
}

void build_attacks() {
    int dK[8][2] = { {1,2},{2,1},{2,-1},{1,-2},{-1,-2},{-2,-1},{-2,1},{-1,2} };
    int d1[8][2] = { {-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1} };
    attack_of = (int**)malloc(N * N * sizeof(int*));
    attack_count = (int*)malloc(N * N * sizeof(int));
    for (int i = 0; i < N * N; i++) {
        attack_of[i] = (int*)malloc(16 * sizeof(int));
        attack_count[i] = 0;
    }
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            int id = idx_of(x, y);
            for (int k = 0; k < 8; k++) {
                int nx = x + d1[k][0];
                int ny = y + d1[k][1];
                if (nx >= 0 && ny >= 0 && nx < N && ny < N)
                    attack_of[id][attack_count[id]++] = idx_of(nx, ny);
            }
            for (int k = 0; k < 8; k++) {
                int nx = x + dK[k][0];
                int ny = y + dK[k][1];
                if (nx >= 0 && ny >= 0 && nx < N && ny < N)
                    attack_of[id][attack_count[id]++] = idx_of(nx, ny);
            }
        }
    }
}

int safe_place(const unsigned char* occ, int id) {
    if (occ[id]) return 0;
    for (int i = 0; i < attack_count[id]; i++)
        if (occ[attack_of[id][i]]) return 0;
    return 1;
}

long dfs_count(Node st) {
    long local = 0;
    if (st.placed == L) {
        free(st.occ);
        return 1;
    }
    for (int i = st.nextIdx; i < N * N; i++) {
        if (safe_place(st.occ, i)) {
            unsigned char* occ2 = (unsigned char*)malloc(N * N);
            memcpy(occ2, st.occ, N * N);
            occ2[i] = 1;
            Node nxt;
            nxt.nextIdx = i + 1;
            nxt.placed = st.placed + 1;
            nxt.occ = occ2;
            local += dfs_count(nxt);
        }
    }
    free(st.occ);
    return local;
}

void* worker(void*) {
    pthread_mutex_lock(&start_mx);
    while (!start_flag)
        pthread_cond_wait(&start_cv, &start_mx);
    pthread_mutex_unlock(&start_mx);

    for (;;) {
        Node job;
        pthread_mutex_lock(&q_mx);
        while (!queue_pop(&job)) {
            if (active_workers == 0) {
                all_work_done = 1;
                pthread_cond_signal(&done_cv);
            }
            if (all_work_done) {
                pthread_mutex_unlock(&q_mx);
                return NULL;
            }
            pthread_cond_wait(&q_cv, &q_mx);
            if (all_work_done) {
                pthread_mutex_unlock(&q_mx);
                return NULL;
            }
        }
        active_workers++;
        pthread_mutex_unlock(&q_mx);

        long got = dfs_count(job);
        pthread_mutex_lock(&total_mx);
        total_solutions += got;
        pthread_mutex_unlock(&total_mx);

        pthread_mutex_lock(&q_mx);
        active_workers--;
        if (!q_head && active_workers == 0) {
            all_work_done = 1;
            pthread_cond_signal(&done_cv);
        }
        pthread_mutex_unlock(&q_mx);
        pthread_cond_broadcast(&q_cv);
    }
    return NULL;
}

void seed_tasks(unsigned char* base_occ, int split_depth) {
    typedef struct { int nextIdx; int placed; unsigned char* occ; } S;
    S* buf = NULL; int cap = 1024, sz = 0, head = 0;
    buf = (S*)malloc(sizeof(S) * cap);
    unsigned char* o = (unsigned char*)malloc(N * N);
    memcpy(o, base_occ, N * N);
    buf[sz++] = (S){ 0, 0, o };
    for (int depth = 0; depth < split_depth; depth++) {
        int level = sz - head;
        for (int t = 0; t < level; t++) {
            S cur = buf[head++];
            for (int i = cur.nextIdx; i < N * N; i++) {
                if (safe_place(cur.occ, i)) {
                    if (sz >= cap) {
                        cap *= 2;
                        buf = (S*)realloc(buf, sizeof(S) * cap);
                    }
                    unsigned char* occ2 = (unsigned char*)malloc(N * N);
                    memcpy(occ2, cur.occ, N * N);
                    occ2[i] = 1;
                    buf[sz++] = (S){ i + 1, cur.placed + 1, occ2 };
                }
            }
            free(cur.occ);
        }
    }
    for (int i = head; i < sz; i++) {
        Node n;
        n.nextIdx = buf[i].nextIdx;
        n.placed = buf[i].placed;
        n.occ = buf[i].occ;
        queue_push(n);
    }
    free(buf);
}

void debag() {
    FILE* f = fopen("debug.txt", "a");
    if (!f) return;
    fprintf(f, "Threads=%d N=%d L=%d K=%d\n", T_threads, N, L, K);
    fprintf(f, "Queue: head=%p tail=%p active=%d total=%lld\n",
        (void*)q_head, (void*)q_tail, active_workers, (long long)total_solutions);
    fclose(f);
}

int main() {
    FILE* in = fopen("input.txt", "r");
    if (!in) return 0;
    if (fscanf(in, "%d", &T_threads) != 1) { fclose(in); return 0; }
    if (fscanf(in, "%d %d %d", &N, &L, &K) != 3) { fclose(in); return 0; }

    preset_x = (int*)malloc(sizeof(int) * K);
    preset_y = (int*)malloc(sizeof(int) * K);
    for (int i = 0; i < K; i++)
        fscanf(in, "%d %d", &preset_x[i], &preset_y[i]);
    fclose(in);

    build_attacks();

    unsigned char* occ = (unsigned char*)calloc(N * N, 1);
    for (int i = 0; i < K; i++) {
        int id = idx_of(preset_x[i], preset_y[i]);
        occ[id] = 1;
    }

    for (int i = 0; i < K; i++) {
        int id = idx_of(preset_x[i], preset_y[i]);
        for (int j = 0; j < attack_count[id]; j++) {
            if (occ[attack_of[id][j]]) {
                FILE* o = fopen("output.txt", "w");
                if (o) { fprintf(o, "0"); fclose(o); }
                FILE* t = fopen("time.txt", "w");
                if (t) { fprintf(t, "0"); fclose(t); }
                return 0;
            }
        }
    }

    int split_depth = (L >= 5) ? 3 : ((L >= 3) ? 2 : 1);
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * T_threads);
    for (int i = 0; i < T_threads; i++)
        pthread_create(&threads[i], NULL, worker, NULL);

    pthread_mutex_lock(&q_mx);
    seed_tasks(occ, split_depth);
    pthread_cond_broadcast(&q_cv);
    pthread_mutex_unlock(&q_mx);

    /* debag(); */

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    pthread_mutex_lock(&start_mx);
    start_flag = 1;
    pthread_cond_broadcast(&start_cv);
    pthread_mutex_unlock(&start_mx);

    pthread_mutex_lock(&done_mx);
    while (!all_work_done)
        pthread_cond_wait(&done_cv, &done_mx);
    pthread_mutex_unlock(&done_mx);

    clock_gettime(CLOCK_MONOTONIC, &t1);
    for (int i = 0; i < T_threads; i++)
        pthread_join(threads[i], NULL);

    long long ms = (t1.tv_sec - t0.tv_sec) * 1000LL + (t1.tv_nsec - t0.tv_nsec) / 1000000LL;

    FILE* out = fopen("output.txt", "w");
    if (out) { fprintf(out, "%lld", (long long)total_solutions); fclose(out); }
    FILE* tout = fopen("time.txt", "w");
    if (tout) { fprintf(tout, "%lld", ms); fclose(tout); }

    queue_clear_leftovers();
    for (int c = 0; c < N * N; c++) free(attack_of[c]);
    free(attack_of);
    free(attack_count);
    free(preset_x);
    free(preset_y);
    free(occ);
    free(threads);

    return 0;
}
