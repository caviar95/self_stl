#pragma once

#include "fsm.hpp"
#include "message.hpp"
#include <map>
#include <memory>
#include <unordered_map>
#include <iostream>

struct SessionContext {
    State state = State::Idle;
};

class FSMDispatch {
public:
    void Dispatch(const Message& msg) {
        auto& fsm = GetOrCreateFSM(msg.ver);
        auto& ctx = GetSessionContext(msg.session_id);

        std::cout << "[Dispatch] Msg ver=" << (int)msg.ver << ", sid=" << msg.session_id << "\n";
        fsm.HandleEvent(ctx.state, msg.event);
    }

private:
    IStateMachine& GetOrCreateFSM(Version ver) {
        auto it = fsm_map_.find(ver);
        if (it != fsm_map_.end()) {
            return *it->second;
        }

        auto fsm = CreateStateMachine(ver);
        if (!fsm) {
            throw std::runtime_error("Unsupported version");
        }

        fsm_map_[ver] = std::move(fsm);
        return *fsm_map_[ver];
    }

    SessionContext& GetSessionContext(int id) {
        return session_map_[id];
    }

    std::unordered_map<Version, std::unique_ptr<IStateMachine>> fsm_map_;
    std::unordered_map<int, SessionContext> session_map_;
};