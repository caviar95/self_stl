// template.hpp
#ifndef TEMPLATE_HPP
#define TEMPLATE_HPP

template <typename T>
class MyTemplateClass {
public:
    MyTemplateClass(T value);
    void printData();
private:
    T data;
};

// 显式特化声明
template <>
class MyTemplateClass<int>;

template <>
class MyTemplateClass<double>;

template <>
class MyTemplateClass<long long>;

#endif
