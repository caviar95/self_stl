#pragma once

#include <functional>

enum class OperType {
    Add, Remove, Update, Butt
};

template <typename DataType>
class Observer {
public:
    using Callback = std::function<void(OperType, const DataType &)>;

    explicit Observer(Callback callback) : callback_(callback) {}

    void update(OperType operation, const DataType &data) {
        callback_(operation, data);
    }

private:
    Callback callback_;
};

