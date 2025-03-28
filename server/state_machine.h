#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

// Enumeration of possible server states.
enum class ServerState {
    Idle,           // No active connection.
    Authentication, // Authenticating client.
    Active,         // Client authenticated and active.
    Logging,        // Logging data transactions.
    Error           // An error has occurred.
};

// Simple state machine to track server status.
class StateMachine {
public:
    StateMachine();
    // Transition to a new state.
    void transition(ServerState newState);
    // Get the current state.
    ServerState getState() const;
private:
    ServerState currentState;
};

#endif // STATE_MACHINE_H
