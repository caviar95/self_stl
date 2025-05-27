#include <coroutine>
#include <iostream>

using namespace std;

struct CoRet {
    struct promise_type {
        suspend_never initial_suspend() {
            return {};
        } 

        suspend_never final_suspend() noexcept {
            return {};
        }

        void unhandled_exception() {}

        CoRet get_reture_object() {
            return {coroutine_handle<promise_type>::from_promise(*this)};
        }
    };

    coroutine_handle<promise_type> _h;
};

struct Input {};

CoRet Guess() {
    Input input;
    int g = co_await input;
    cout << "coroutine: You guess " << g << endl;
}

int main()
{
    auto ret = Guess();

    cout << "main: make a guess ..." << endl;

    return 0;
}
