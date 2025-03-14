#pragma once

#include <unordered_map>
#include <mutex>
#include <functional>
#include <iostream>
#include <typeindex>

#include "command.h"
#include "command_handler.h"

class CommandDispatcher {
private:
    std::unordered_map<std::type_index, 
                      std::function<void(const Command&)>> handlers;
    std::mutex map_mutex;

    template <typename T>
    static void typeSafeHandler(const Command& cmd, CommandHandler<T>* handler) {
        try {
            const auto& concrete_cmd = dynamic_cast<const T&>(cmd);
            handler->handle(concrete_cmd);
        } catch (const std::bad_cast&) {
            std::cerr << "Command type mismatch!\n";
        }
    }

public:
    template <typename T>
    void registerHandler(CommandHandler<T>* handler) {
        std::lock_guard<std::mutex> lock(map_mutex);
        auto wrapper = [handler](const Command& cmd) {
            typeSafeHandler<T>(cmd, handler);
        };
        handlers[typeid(T)] = wrapper;
    }

    void dispatch(const Command& cmd) {
        std::lock_guard<std::mutex> lock(map_mutex);
        auto it = handlers.find(typeid(cmd));
        if (it != handlers.end()) {
            cmd.printInfo();
            it->second(cmd);
        } else {
            std::cerr << "No handler registered for: "
                      << typeid(cmd).name() << "\n";
        }
    }

    void unregisterAll() {
        std::lock_guard<std::mutex> lock(map_mutex);
        handlers.clear();
    }
};