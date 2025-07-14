#include "fsm.hpp"
#include <iostream>

void PrintTransition(State from, Event evt, State to) {
    std::cout << "[FSM] " << to_string(from) 
              << " -- " << to_string(evt) 
                << " --> " << to_string(to) << std::endl;
}
