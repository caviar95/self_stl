#pragma once

enum class OperType {
    ADD, DELELE, UPDATE
};

template <typename DataType>
class ObserverBase {
public:
    virtual ~ObserverBase() {}

    virtual void update(OperType op, const DataType &data) = 0;
};

