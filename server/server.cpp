#include "server.h"
#include "utils/logger.h"
#include "packet.h"
#include <iostream>
#include <cstring>
#include <unistd.h>       // POSIX functions for socket handling.
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Constructor initializes port and serverSocket.
Server::Server(int port) : port(port), serverSocket(-1), running(false) {}

// Destructor stops the server.
Server::~Server() {
    stop();
}

// Start the server: create socket, bind, and listen.
bool Server::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        Logger::getInstance().log("Error: Cannot create socket");
        return false;
    }
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        Logger::getInstance().log("Error: Bind failed");
        return false;
    }

    if (listen(serverSocket, 10) < 0) {
        Logger::getInstance().log("Error: Listen failed");
        return false;
    }

    running = true;
    stateMachine.transition(ServerState::Idle);
    std::thread(&Server::acceptClients, this).detach();
    Logger::getInstance().log("Server started on port " + std::to_string(port));
    return true;
}

// Stop the server and clean up resources.
void Server::stop() {
    running = false;
    if (serverSocket >= 0) {
        close(serverSocket);
    }
    for (auto& th : clientThreads) {
        if (th.joinable())
            th.join();
    }
    Logger::getInstance().log("Server stopped");
}

// Accept incoming client connections.
void Server::acceptClients() {
    while (running) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            Logger::getInstance().log("Error: Accept failed");
            continue;
        }
        Logger::getInstance().log("Connection accepted from " + std::string(inet_ntoa(clientAddr.sin_addr)));
        clientThreads.push_back(std::thread(&Server::handleClient, this, clientSocket));
    }
}

// Handle communication with a single client.
void Server::handleClient(int clientSocket) {
    // Transition to Authentication state.
    stateMachine.transition(ServerState::Authentication);
    if (!authenticateClient(clientSocket)) {
        Logger::getInstance().log("Authentication failed for client socket: " + std::to_string(clientSocket));
        close(clientSocket);
        stateMachine.transition(ServerState::Idle);
        return;
    }
    stateMachine.transition(ServerState::Active);

    // Process client requests.
    char buffer[1024];
    while (true) {
        int bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            Logger::getInstance().log("Client disconnected, socket: " + std::to_string(clientSocket));
            break;
        }

        // Build a packet from received data.
        Packet pkt;
        pkt.constructPacket("Client", "Server", "TCP", 0, buffer, bytesRead, "OK");

        // Log the received packet.
        std::string logEntry = "RX | src:" + pkt.header.src +
                               " | dst:" + pkt.header.dst +
                               " | protocol:" + pkt.header.protocol +
                               " | pkt#:" + std::to_string(pkt.header.pktNum) +
                               " | status:" + pkt.tail.errorCode;
        Logger::getInstance().log(logEntry);

        // TODO: Process the packet and send responses as needed.
    }
    close(clientSocket);
    stateMachine.transition(ServerState::Idle);
}

// Dummy client authentication.
bool Server::authenticateClient(int clientSocket) {
    char buffer[256];
    int bytesRead = read(clientSocket, buffer, sizeof(buffer));
    if (bytesRead <= 0) {
        return false;
    }
    Logger::getInstance().log("Received login attempt: " + std::string(buffer, bytesRead));
    // Accept any non-empty login for demonstration.
    std::string authResponse = "AUTH_OK";
    write(clientSocket, authResponse.c_str(), authResponse.size());
    return true;
}
