#pragma once

#include "state.hpp"
#include "version.hpp"

struct Message {
    Version ver;
    Event event;
    int session_id;
};
