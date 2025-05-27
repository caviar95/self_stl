
## Model Structure Craft

```text
ThreadPool
  |- WorkerThread (n)
  |- TaskQueue (multi-threads-safe queue, support priority/timed-task)
  |- DelayTaskManager (timed-task scheduler, implement by min-heap)
  |- Submit(task, options) (Future/CancelHandle)

Producer
  |- create task and submit to ThreadPool

Consumer (WorkerThread)
  |- get task to execute from TaskQueue, support callback-notify/observer

```

## Design Steps

1. reuse current producer-consumer
2. encapsulate task to Task object + Task abstraction
3. add ThreadPool encapsulation(control all consumer threads in one)
4. add delaying-task scheduler(using std::priority_queue with min-heap)
5. add cancel logics(check cancelled flag before execute task)
6. thread logic with binding task(each thread maintains an independent task queue + a shared queue)

## Function List

1. multi-threads worked-thread-pool
2. support delay_task
3. support cancel task
4. support multi-tasks submit concurrently
5. support log after task executed
