#include <stdexcept>  
#include <typeinfo>  

class Any {  
    struct Base {  
        virtual ~Base() = default;  
        virtual Base* clone() const = 0;  
    };  
    template<typename T>  
    struct Derived : Base {  
        T value;  
        Derived(T v) : value(v) {}  
        Base* clone() const override { return new Derived(value); }  
    };  
    Base* ptr = nullptr;  
public:  
    template<typename T>  
    Any(T&& val) : ptr(new Derived<std::decay_t<T>>(std::forward<T>(val))) {}  
    ~Any() { delete ptr; }  

    template<typename T>  
    T cast() const {  
        if (auto d = dynamic_cast<Derived<T>*>(ptr)) {  
            return d->value;  
        }  
        throw std::bad_cast();  
    }  
};  

// Test Case  
int main() {  
    Any a = 42;  
    assert(a.cast<int>() == 42);  
}  
