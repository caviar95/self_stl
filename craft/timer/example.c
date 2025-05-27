#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef struct TimerTask {
    timer_t timerid;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    int result;
    int done;
} TimerTask;

static void timer_task_callback(union sigval sv) {
    TimerTask *task = (TimerTask*)sv.sival_ptr;

    sleep(1);

    int computed = 12345;

    pthread_mutex_lock(&task->mtx);
    task->result = computed;
    task->done = 1;
    pthread_cond_signal(&task->cond);
    pthread_mutex_unlock(&task->mtx);
}

int TimerTask_Start(TimerTask *task, long delay_ms) {
    struct sigevent sev;
    struct itimerspec its;

    pthread_mutex_init(&task->mtx, NULL);
    pthread_cond_init(&task->cond, NULL);

    task->done = 0;
    task->result = 0;

    memset(&sev, 0, sizeof(sev));
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = timer_task_callback;
    sev.sigev_value.sival_ptr = task;
    sev.sigev_notify_attributes = NULL;

    if (timer_create(CLOCK_REALTIME, &sev, &task->timerid) == -1) {
        perror("timer_create");
        return -1;
    }

    its.it_value.tv_sec = delay_ms / 1000;
    its.it_value.tv_nsec = (delay_ms % 1000) * 1000000;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    if (timer_settime(task->timerid, 0, &its, NULL) == -1) {
        perror("timer_settime");
        return -1;
    }

    return 0;
}

// 等待任务完成并返回结果
int TimerTask_Wait(TimerTask *task) {
    pthread_mutex_lock(&task->mtx);
    while (!task->done) {
        pthread_cond_wait(&task->cond, &task->mtx);
    }
    int res = task->result;
    pthread_mutex_unlock(&task->mtx);
    return res;
}

// 清理
void TimerTask_Destroy(TimerTask *task) {
    timer_delete(task->timerid);
    pthread_mutex_destroy(&task->mtx);
    pthread_cond_destroy(&task->cond);
}

int main() {
    TimerTask task;

    printf("Scheduling timer task (fires in 500ms)... \n");

    if (TimerTask_Start(&task, 500) != 0) {
        return 1;
    }

    printf("Main thread doing work...\n");

    sleep(2);

    int result = TimerTask_Wait(&task);
    printf("Got async result: %d\n", result);

    TimerTask_Destroy(&task);

    return 0;
}
