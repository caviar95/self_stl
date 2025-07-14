#include "version.hpp"
#include "message.hpp"
#include "fsm_dispatch.hpp"
#include <vector>

int main() {
    FSMDispatch dispatcher;

    std::vector<Message> msgs = {
        {Version::V1, Event::Start, 100},
        {Version::V1, Event::Reset, 100},

        {Version::V2, Event::Start, 200},
        {Version::V2, Event::Finish, 200},
        {Version::V2, Event::Reset, 200},
    };

    for (const auto& msg : msgs) {
        dispatcher.Dispatch(msg);
    }

    return 0;
}