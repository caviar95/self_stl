#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

#define NUM_WORKERS 4
#define MAX_TASKS   10

typedef struct {
    pthread_t thread_id;
    int worker_id;
    int interval_sec; // 定时周期
    time_t next_trigger;
} TimerTask;

typedef struct {
    int worker_id;
    int interval_sec;
    pthread_t creator_tid;
    void (*task_func)(int, int, pthread_t);
} TaskContext;

pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER;
TimerTask timer_tasks[MAX_TASKS];
int task_count = 0;

void log_time_info(const char* prefix, int worker_id, pthread_t exec_tid, pthread_t origin_tid, int interval) {
    time_t now = time(NULL);
    char buf[64];
    strftime(buf, sizeof(buf), "%F %T", localtime(&now));

    printf("[%s] [%s] Worker %d | Created by TID %lu | Executed by TID %lu | Interval: %ds\n",
           buf, prefix, worker_id,
           (unsigned long)origin_tid,
           (unsigned long)exec_tid,
           interval);
}

// 各线程的任务函数
void task_func(int worker_id, int interval, pthread_t creator_tid) {
    log_time_info("Task Exec", worker_id, pthread_self(), creator_tid, interval);
}

// 工作线程执行
void* worker_thread(void* arg) {
    TaskContext* ctx = (TaskContext*)arg;
    while (1) {
        sleep(ctx->interval_sec);
        task_func(ctx->worker_id, ctx->interval_sec, ctx->creator_tid);
    }
    return NULL;
}

// 定时器线程
void* timer_thread(void* arg) {
    while (1) {
        sleep(1); // 每秒检查一次
        time_t now = time(NULL);

        pthread_mutex_lock(&task_mutex);
        for (int i = 0; i < task_count; ++i) {
            if (now >= timer_tasks[i].next_trigger) {
                // 触发任务
                log_time_info("Timer Trigger", timer_tasks[i].worker_id, pthread_self(),
                              timer_tasks[i].thread_id, timer_tasks[i].interval_sec);
                timer_tasks[i].next_trigger = now + timer_tasks[i].interval_sec;
            }
        }
        pthread_mutex_unlock(&task_mutex);
    }
    return NULL;
}

// 注册定时器任务
void register_timer_task(pthread_t tid, int worker_id, int interval) {
    pthread_mutex_lock(&task_mutex);
    if (task_count >= MAX_TASKS) {
        printf("Max tasks reached!\n");
        pthread_mutex_unlock(&task_mutex);
        return;
    }

    timer_tasks[task_count].thread_id = tid;
    timer_tasks[task_count].worker_id = worker_id;
    timer_tasks[task_count].interval_sec = interval;
    timer_tasks[task_count].next_trigger = time(NULL) + interval;
    task_count++;
    pthread_mutex_unlock(&task_mutex);
}

int main() {
    pthread_t workers[NUM_WORKERS];
    TaskContext contexts[NUM_WORKERS];
    int intervals[NUM_WORKERS] = {1, 2, 3, 4};

    // 启动定时器线程
    pthread_t timer_tid;
    pthread_create(&timer_tid, NULL, timer_thread, NULL);

    // 启动工作线程并注册任务
    for (int i = 0; i < NUM_WORKERS; ++i) {
        contexts[i].worker_id = i + 1;
        contexts[i].interval_sec = intervals[i];
        contexts[i].creator_tid = pthread_self();
        contexts[i].task_func = task_func;

        pthread_create(&workers[i], NULL, worker_thread, &contexts[i]);
        register_timer_task(workers[i], i + 1, intervals[i]);
    }

    // 主线程等待
    pthread_join(timer_tid, NULL);
    for (int i = 0; i < NUM_WORKERS; ++i)
        pthread_join(workers[i], NULL);

    return 0;
}
