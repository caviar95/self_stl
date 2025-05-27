#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>
#include <memory>
#include <atomic>

// ========== Observer 接口 ==========
class Observer {
public:
    virtual void onNotified() = 0;
    virtual ~Observer() = default;
};

// ========== 主题（被观察者） ==========
class TaskQueue {
public:
    void addObserver(std::shared_ptr<Observer> obs) {
        std::lock_guard<std::mutex> lock(observerMutex_);
        observers_.push_back(obs);
    }

    void pushTask(int task) {
        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            queue_.push(task);
        }
        notifyObservers();
    }

    bool popTask(int& task) {
        std::lock_guard<std::mutex> lock(queueMutex_);
        if (queue_.empty()) return false;
        task = queue_.front();
        queue_.pop();
        return true;
    }

	bool hasTask() {
	    std::lock_guard<std::mutex> lock(queueMutex_);
		return !queue_.empty();
	}

private:
    void notifyObservers() {
        std::lock_guard<std::mutex> lock(observerMutex_);
        for (auto& obs : observers_) {
            if (obs) obs->onNotified();  // 回调通知
        }
    }

private:
    std::queue<int> queue_;
    std::mutex queueMutex_;

    std::vector<std::shared_ptr<Observer>> observers_;
    std::mutex observerMutex_;
};

// ========== 消费者（观察者） ==========
class Consumer : public Observer, public std::enable_shared_from_this<Consumer> {
public:
    Consumer(std::shared_ptr<TaskQueue> queue, int id)
        : queue_(queue), id_(id), stopFlag_(false) {}

    void start() {
        thread_ = std::thread([self = shared_from_this()] {
            self->run();
        });
    }

	void stop() {
	    stopFlag_ = true;
		cv_.notify_all();  // 所有线程都唤醒
	}


    void onNotified() override {
        cv_.notify_one();  // 唤醒 run 中等待的线程
    }

private:
	void run() {
		while (true) {
			std::unique_lock<std::mutex> lock(cvMutex_);
			cv_.wait(lock, [this]() {
		        return stopFlag_ || queue_->hasTask();  // ✅ 非破坏性判断
	        });

	        if (stopFlag_ && !queue_->hasTask()) break;  // 等剩余任务处理完再退出

	        int task;
	        while (queue_->popTask(task)) {
				std::cout << "[Consumer " << id_ << "] Consumed task: " << task << std::endl;
			}
		}
	}
	

private:
    std::shared_ptr<TaskQueue> queue_;
    int id_;
    std::thread thread_;
    std::atomic<bool> stopFlag_;

    std::condition_variable cv_;
    std::mutex cvMutex_;
};

// ========== 生产者 ==========
void producer(std::shared_ptr<TaskQueue> queue) {
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << "[Producer] Produced task: " << i << std::endl;
        queue->pushTask(i);
    }
}

int main() {
    auto queue = std::make_shared<TaskQueue>();

    // 启动两个消费者
    auto consumer1 = std::make_shared<Consumer>(queue, 1);
    auto consumer2 = std::make_shared<Consumer>(queue, 2);

    queue->addObserver(consumer1);
    queue->addObserver(consumer2);

    consumer1->start();
    consumer2->start();

    // 启动生产者线程
    std::thread prodThread(producer, queue);
    prodThread.join();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    consumer1->stop();
    consumer2->stop();

    return 0;
}

