#include <iostream>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <typeindex>
#include <functional>
#include <vector>

// ==================== 基础定义 ====================
class Command {
public:
    virtual ~Command() = default;
    virtual void printInfo() const = 0;
};

// ==================== 具体命令类型 ====================
class LoginCommand : public Command {
public:
    std::string username;
    std::string password;

    LoginCommand(std::string u, std::string p)
        : username(std::move(u)), password(std::move(p)) {}

    void printInfo() const override {
        std::cout << "[LoginCommand] User: " << username << "\n";
    }
};

class LogoutCommand : public Command {
public:
    int user_id;
    std::string reason;

    LogoutCommand(int id, std::string r)
        : user_id(id), reason(std::move(r)) {}

    void printInfo() const override {
        std::cout << "[LogoutCommand] UserID: " << user_id 
                  << ", Reason: " << reason << "\n";
    }
};

// ==================== 命令处理器接口 ====================
template <typename T>
class CommandHandler {
public:
    virtual void handle(const T& command) = 0;
    virtual ~CommandHandler() = default;
};

// ==================== 分发器核心类 ====================
class CommandDispatcher {
private:
    // 注册表：存储命令类型及其对应的处理器列表
    std::unordered_map<std::type_index, 
                      std::vector<std::function<void(const Command&)>>> handlers;
    std::mutex map_mutex;

    template <typename T>
    static void typeSafeHandler(const Command& cmd, 
                              CommandHandler<T>* handler) {
        try {
            const auto& concrete_cmd = dynamic_cast<const T&>(cmd);
            handler->handle(concrete_cmd);
        } catch (const std::bad_cast&) {
            std::cerr << "Command type mismatch!\n";
        }
    }

public:
    // 注册命令处理器
    template <typename T>
    void registerHandler(CommandHandler<T>* handler) {
        std::lock_guard<std::mutex> lock(map_mutex);
        auto wrapper = [handler](const Command& cmd) {
            typeSafeHandler<T>(cmd, handler);
        };
        handlers[typeid(T)].push_back(wrapper);
    }

    // 移除命令处理器
    template <typename T>
    void unregisterHandler(CommandHandler<T>* handler) {
        std::lock_guard<std::mutex> lock(map_mutex);
        auto& handler_list = handlers[typeid(T)];
        handler_list.erase(
            std::remove_if(handler_list.begin(), handler_list.end(),
                [handler](const std::function<void(const Command&)>& func) {
                    return func.target<CommandHandler<T>*>() == handler;
                }),
            handler_list.end()
        );
    }

    // 分发命令
    void dispatch(const Command& cmd) {
        std::lock_guard<std::mutex> lock(map_mutex);
        auto it = handlers.find(typeid(cmd));
        if (it != handlers.end()) {
            cmd.printInfo();
            for (const auto& handler : it->second) {
                handler(cmd);
            }
        } else {
            std::cerr << "No handler registered for: "
                      << typeid(cmd).name() << "\n";
        }
    }

    // 清空所有处理器
    void unregisterAll() {
        std::lock_guard<std::mutex> lock(map_mutex);
        handlers.clear();
    }
};

// ==================== 具体处理器实现 ====================
class LoginHandler : public CommandHandler<LoginCommand> {
public:
    void handle(const LoginCommand& cmd) override {
        std::cout << "Processing login: " << cmd.username
                  << " with password: " << cmd.password << "\n";
        // 实际业务逻辑...
    }
};

class LogoutHandler : public CommandHandler<LogoutCommand> {
public:
    void handle(const LogoutCommand& cmd) override {
        std::cout << "Processing logout: User#" << cmd.user_id
                  << ", Reason: " << cmd.reason << "\n";
        // 实际业务逻辑...
    }
};

// ==================== 使用示例 ====================
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

    // 移除处理器
    dispatcher.unregisterHandler<LoginCommand>(&login_handler);

    // 再次分发命令（LoginCommand 无处理器）
    dispatcher.dispatch(login);

    // 清空所有处理器
    dispatcher.unregisterAll();

    // 分发命令（无处理器）
    dispatcher.dispatch(logout);

    return 0;
}
