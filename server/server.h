// server.h
#ifndef SERVER_H
#define SERVER_H

#include "state_machine.h"
#include "database/database.h" // Including Isaac's database header
#include <string>
#include <thread>
#include <vector>
#include <atomic>

// Server class declares the interface for our server.
class Server {
public:
    // Constructor that takes the port number.
    Server(int port);
    ~Server();

    // Start and stop the server.
    bool start();
    void stop();

private:
    int port;
    int serverSocket;              // Socket descriptor.
    std::atomic<bool> running;     // Flag to control the server loop.
    StateMachine stateMachine;     // Manages server state.
    std::vector<std::thread> clientThreads;  // Threads for handling clients.
    Database db;                   // Database instance (from Isaac's code)

    // Function to accept new client connections.
    void acceptClients();
    // Function to handle communication with a connected client.
    void handleClient(int clientSocket);
    // Dummy authentication function for a connected client.
    bool authenticateClient(int clientSocket);
};

#endif // SERVER_H
