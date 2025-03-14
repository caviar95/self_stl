#pragma once

#include "command.h"

#include <string>
#include <iostream>

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
