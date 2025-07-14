#pragma once

#include <string>

enum class State {
    Idle,
    Processing,
    Done
};

enum class Event {
    Start,
    Finish,
    Reset
};

inline std::string to_string(State state) {
    switch (state) {
        case State::Idle: return "Idle";
        case State::Processing: return "Processing";
        case State::Done: return "Done";
        default: return "Unknown State";
    }
}

inline std::string to_string(Event event) {
    switch (event) {
        case Event::Start: return "Start";
        case Event::Finish: return "Finish";
        case Event::Reset: return "Reset";
        default: return "Unknown Event";
    }
}