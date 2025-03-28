#include "server.h"
#include "utils/logger.h"
#include <iostream>
#include <csignal>
#include <chrono>
#include <thread>

// Global pointer to allow signal handling for graceful shutdown.
Server* serverInstance = nullptr;

// Signal handler to stop the server.
void signalHandler(int signum) {
    if (serverInstance) {
        serverInstance->stop();
    }
    exit(signum);
}

int main() {
    // Setup signal handlers for graceful shutdown.
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    int port = 12345; // Example port number.
    Server server(port);
    serverInstance = &server;

    if (!server.start()) {
        std::cerr << "Failed to start server." << std::endl;
        return -1;
    }

    // Main loop: the server is running and handling clients in separate threads.
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
