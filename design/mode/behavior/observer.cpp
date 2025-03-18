#include <iostream>
#include <vector>
#include <algorithm>

// 观察者接口
class Observer {
public:
    virtual void update(const std::string& message) = 0;
    virtual ~Observer() = default;
};

// 主题
class NewsPublisher {
private:
    std::vector<Observer*> _observers;
    std::string _latestNews;

public:
    void attach(Observer* observer) {
        _observers.push_back(observer);
    }

    void detach(Observer* observer) {
        _observers.erase(std::remove(_observers.begin(), _observers.end(), observer), _observers.end());
    }

    void notify() {
        for (auto observer : _observers) {
            observer->update(_latestNews);
        }
    }

    void publishNews(const std::string& news) {
        _latestNews = news;
        notify();
    }
};

// 具体观察者
class EmailSubscriber : public Observer {
public:
    void update(const std::string& message) override {
        std::cout << "Email Received: " << message << std::endl;
    }
};

int main() {
    NewsPublisher publisher;
    EmailSubscriber emailSubscriber;
    publisher.attach(&emailSubscriber);
    publisher.publishNews("C++17 is released!");
    return 0;
}