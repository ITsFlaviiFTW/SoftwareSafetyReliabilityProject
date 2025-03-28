#include "server.h"
#include "utils/logger.h"
#include "packet.h"
#include <iostream>
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")  // Link against Winsock
#else
    #include <unistd.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
#endif

// Constructor initializes port and sets socket accordingly.
Server::Server(int port) : port(port), running(false) {
#ifdef _WIN32
    serverSocket = INVALID_SOCKET;
#else
    serverSocket = -1;
#endif
}

Server::~Server() {
    stop();
}

// Start the server, initializing Winsock on Windows.
bool Server::start() {
#ifdef _WIN32
    // Initialize Winsock.
    WSADATA wsaData;
    int wsaInit = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (wsaInit != 0) {
        Logger::getInstance().log("WSAStartup failed: " + std::to_string(wsaInit));
        return false;
    }
#endif

    // Create a TCP socket.
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WIN32
    if (serverSocket == INVALID_SOCKET) {
        Logger::getInstance().log("Error: Cannot create socket");
        return false;
    }
#else
    if (serverSocket < 0) {
        Logger::getInstance().log("Error: Cannot create socket");
        return false;
    }
#endif

    int opt = 1;
#ifdef _WIN32
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

#ifdef _WIN32
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        Logger::getInstance().log("Error: Bind failed");
        return false;
    }
#else
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        Logger::getInstance().log("Error: Bind failed");
        return false;
    }
#endif

#ifdef _WIN32
    if (listen(serverSocket, 10) == SOCKET_ERROR) {
        Logger::getInstance().log("Error: Listen failed");
        return false;
    }
#else
    if (listen(serverSocket, 10) < 0) {
        Logger::getInstance().log("Error: Listen failed");
        return false;
    }
#endif

    running = true;
    stateMachine.transition(ServerState::Idle);
    // Start a detached thread to accept clients.
    std::thread(&Server::acceptClients, this).detach();
    Logger::getInstance().log("Server started on port " + std::to_string(port));
    return true;
}

// Stop the server and clean up resources.
void Server::stop() {
    running = false;
#ifdef _WIN32
    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
    }
#else
    if (serverSocket >= 0) {
        close(serverSocket);
    }
#endif

    for (auto& th : clientThreads) {
        if (th.joinable())
            th.join();
    }
    Logger::getInstance().log("Server stopped");

#ifdef _WIN32
    WSACleanup();
#endif
}

// Accept new client connections.
void Server::acceptClients() {
    while (running) {
        struct sockaddr_in clientAddr;
#ifdef _WIN32
        int addrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET) {
            Logger::getInstance().log("Error: Accept failed");
            continue;
        }
#else
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            Logger::getInstance().log("Error: Accept failed");
            continue;
        }
#endif
        Logger::getInstance().log("Connection accepted from " + std::string(inet_ntoa(clientAddr.sin_addr)));
        clientThreads.push_back(std::thread(&Server::handleClient, this, clientSocket));
    }
}

// Handle communication with a connected client.
void Server::handleClient(int clientSocket) {
    // Transition to Authentication state.
    stateMachine.transition(ServerState::Authentication);
    if (!authenticateClient(clientSocket)) {
        Logger::getInstance().log("Authentication failed for client socket: " + std::to_string(clientSocket));
#ifdef _WIN32
        closesocket(clientSocket);
#else
        close(clientSocket);
#endif
        stateMachine.transition(ServerState::Idle);
        return;
    }
    stateMachine.transition(ServerState::Active);

    char buffer[1024];
    while (true) {
#ifdef _WIN32
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
#else
        int bytesRead = read(clientSocket, buffer, sizeof(buffer));
#endif
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
#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
    stateMachine.transition(ServerState::Idle);
}

// Authenticate the connected client (dummy implementation).
bool Server::authenticateClient(int clientSocket) {
    char buffer[256];
#ifdef _WIN32
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
#else
    int bytesRead = read(clientSocket, buffer, sizeof(buffer));
#endif
    if (bytesRead <= 0) {
        return false;
    }
    Logger::getInstance().log("Received login attempt: " + std::string(buffer, bytesRead));
    std::string authResponse = "AUTH_OK";
#ifdef _WIN32
    send(clientSocket, authResponse.c_str(), authResponse.size(), 0);
#else
    write(clientSocket, authResponse.c_str(), authResponse.size());
#endif
    return true;
}
