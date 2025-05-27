#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

// Observer Interface
class Observer {
public:
    virtual void update(const std::string& message) = 0;
};

// Concrete Observer
class ConcreteObserver : public Observer {
public:
    void update(const std::string& message) override {
        std::cout << "Observer received: " << message << std::endl;
    }
};

// Subject Class
class Subject {
private:
    std::vector<Observer*> observers;

public:
    void addObserver(Observer* observer) {
        observers.push_back(observer);
    }

    void removeObserver(Observer* observer) {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }

    void notifyObservers(const std::string& message) {
        for (auto& observer : observers) {
            observer->update(message);
        }
    }
};

int main() {
    Subject subject;
    ConcreteObserver observer1, observer2;
    subject.addObserver(&observer1);
    subject.addObserver(&observer2);
    
    subject.notifyObservers("Hello Observers!");
    return 0;
}
