#pragma once

#include "command_handler.h"
#include "command_log.hpp"

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