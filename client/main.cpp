#include "client.h"
#include "utils/logger.h"

#include <iostream>
#include <sstream>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#endif

int main(int argc, char* argv[]) {
#ifdef _WIN32
    // Initialize Winsock on Windows
    WSADATA wsaData;
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaResult != 0) {
        std::cerr << "Failed to initialize Winsock. Error code: " << wsaResult << std::endl;
        return 1;
    }
#endif

    if (argc < 5) {
        std::cout << "Usage: " << argv[0] << " <server_ip> <server_port> <username> <password>\n";
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    std::string serverIp   = argv[1];
    int serverPort         = std::stoi(argv[2]);
    std::string username   = argv[3];
    std::string password   = argv[4];

    Client client(serverIp, serverPort);
    if (!client.connectToServer()) {
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    if (!client.authenticate(username, password)) {
        client.disconnect();
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    // Retrieve tasks
    auto tasks = client.getTasks();
    std::cout << "Tasks retrieved:\n";
    for (const auto& t : tasks) {
        std::cout << " - " << t.aircraft << ": " << t.taskName << " [" << t.status << "]\n";
    }

    // If there is at least one task, update its status
    if (!tasks.empty()) {
        tasks[0].status = "In Progress";
        client.updateTaskStatus(tasks[0]);
    }

    // Get an aircraft status
    std::string status = client.getAircraftStatus("AC123");
    std::cout << "Aircraft AC123 status: " << status << "\n";

    // Update an aircraft status
    client.updateAircraftStatus("AC123", "Maintenance Completed");

    // Disconnect
    client.disconnect();

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
