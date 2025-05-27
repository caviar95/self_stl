#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <string.h>

#define THREAD_COUNT 4

typedef struct Task {
    uint64_t creator_tid;
    int interval_sec;
    struct Task* next;
} Task;

typedef struct TaskQueue {
    Task* head;
    Task* tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} TaskQueue;

TaskQueue queues[THREAD_COUNT];
pthread_t workers[THREAD_COUNT];
pthread_t timer_thread;

static inline uint64_t get_tid() {
    return (uint64_t)syscall(SYS_gettid);
}

void enqueue_task(TaskQueue* q, Task* task) {
    pthread_mutex_lock(&q->mutex);
    if (q->tail == NULL) {
        q->head = q->tail = task;
    } else {
        q->tail->next = task;
        q->tail = task;
    }
    task->next = NULL;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

Task* dequeue_task(TaskQueue* q) {
    pthread_mutex_lock(&q->mutex);
    while (q->head == NULL) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    Task* task = q->head;
    q->head = task->next;
    if (q->head == NULL)
        q->tail = NULL;
    pthread_mutex_unlock(&q->mutex);
    return task;
}

void* worker_func(void* arg) {
    int idx = *(int*)arg;
    free(arg);
    TaskQueue* q = &queues[idx];

    while (1) {
        Task* task = dequeue_task(q);
        time_t now = time(NULL);
        struct tm tm_now;
        localtime_r(&now, &tm_now);
        char buf[64];
        strftime(buf, sizeof(buf), "%H:%M:%S", &tm_now);

        printf("[Worker %d] Executed by TID %lu | Created by TID %lu | Interval: %d sec | Time: %s\n",
               idx + 1, get_tid(), task->creator_tid, task->interval_sec, buf);

        free(task);
    }
    return NULL;
}

void* timer_func(void* arg) {
    (void)arg;
    // 定时周期数组，单位秒
    int intervals[THREAD_COUNT] = {1, 2, 3, 4};
    // 记录每个任务的上次触发时间（秒）
    time_t last_times[THREAD_COUNT];
    time_t now = time(NULL);
    for (int i = 0; i < THREAD_COUNT; i++) {
        last_times[i] = now;
    }

    while (1) {
        now = time(NULL);
        for (int i = 0; i < THREAD_COUNT; i++) {
            if (now - last_times[i] >= intervals[i]) {
                // 创建任务
                Task* task = malloc(sizeof(Task));
                task->creator_tid = get_tid();
                task->interval_sec = intervals[i];
                task->next = NULL;
                // 放入对应线程队列
                enqueue_task(&queues[i], task);
                last_times[i] = now;
            }
        }
        usleep(100 * 1000); // 100ms 轮询
    }
    return NULL;
}

int main() {
    for (int i = 0; i < THREAD_COUNT; i++) {
        queues[i].head = NULL;
        queues[i].tail = NULL;
        pthread_mutex_init(&queues[i].mutex, NULL);
        pthread_cond_init(&queues[i].cond, NULL);
    }

    // 启动工作线程
    for (int i = 0; i < THREAD_COUNT; i++) {
        int* idx = malloc(sizeof(int));
        *idx = i;
        pthread_create(&workers[i], NULL, worker_func, idx);
    }

    // 启动定时器线程
    pthread_create(&timer_thread, NULL, timer_func, NULL);

    // 主线程等待所有线程（这里不会退出）
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(workers[i], NULL);
    }
    pthread_join(timer_thread, NULL);

    return 0;
}
