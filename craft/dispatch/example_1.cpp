#include "command_dispatch.hpp"
#include "command_handler_impl.hpp"

int main() {
    CommandDispatcher dispatcher;
    LoginHandler login_handler;
    LogoutHandler logout_handler;

    // 注册处理器
    dispatcher.registerHandler<LoginCommand>(&login_handler);
    dispatcher.registerHandler<LogoutCommand>(&logout_handler);

    // 创建命令
    LoginCommand login("admin", "p@ssw0rd");
    LogoutCommand logout(1001, "session_timeout");

    // 分发命令
    dispatcher.dispatch(login);
    dispatcher.dispatch(logout);

    // 测试未注册命令
    class UnknownCommand : public Command {
        void printInfo() const override {
            std::cout << "[UnknownCommand]\n";
        }
    };
    UnknownCommand unknown;
    dispatcher.dispatch(unknown);

    return 0;
}