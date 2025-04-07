#include "state_machine.h"

// Initialize the state machine in Idle state.
StateMachine::StateMachine() : currentState(ServerState::Idle) {}

// Transition the state machine to a new state.
void StateMachine::transition(ServerState newState) {
    // Here you could add logging or additional validation.
    currentState = newState;
}

// Return the current state.
ServerState StateMachine::getState() const {
    return currentState;
}
