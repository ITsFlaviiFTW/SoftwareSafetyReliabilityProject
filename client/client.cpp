#include "client.h"
#include "utils/logger.h"
#include "packet.h"

#include <iostream>
#include <sstream>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#endif

Client::Client(const std::string& serverIp, int serverPort)
    : serverIp_(serverIp),
      serverPort_(serverPort),
      sockFd_(INVALID_SOCKET),
      isConnected_(false)
{
}

Client::~Client() {
    disconnect();
}

bool Client::connectToServer() {
    // Create a TCP socket
    sockFd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockFd_ == INVALID_SOCKET) {
        logError("Failed to create socket on Windows.");
        return false;
    }

    // Prepare the server address structure
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort_);

    // Convert IP string to usable format
    int result = inet_pton(AF_INET, serverIp_.c_str(), &serverAddr.sin_addr);
    if (result <= 0) {
        logError("Invalid server IP address format.");
        closesocket(sockFd_);
        sockFd_ = INVALID_SOCKET;
        return false;
    }

    // Connect to the server
    result = connect(sockFd_, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        logError("Connection to server failed.");
        closesocket(sockFd_);
        sockFd_ = INVALID_SOCKET;
        return false;
    }

    isConnected_ = true;
    logInfo("Connected to server at " + serverIp_ + ":" + std::to_string(serverPort_));
    return true;
}

void Client::disconnect() {
    if (isConnected_) {
        shutdown(sockFd_, SD_BOTH);
        closesocket(sockFd_);
        sockFd_ = INVALID_SOCKET;
        isConnected_ = false;
        logInfo("Disconnected from server.");
    }
}

bool Client::authenticate(const std::string& username, const std::string& password) {
    if (!isConnected_) {
        logError("Cannot authenticate; client is not connected.");
        return false;
    }

    std::map<std::string, std::string> authRequest;
    authRequest["type"]     = "auth";
    authRequest["username"] = username;
    authRequest["password"] = password;

    std::string requestData = encodeRequest(authRequest);

    // Create packet using createPacket from packet.h
    std::string packet = createPacket("client", "server", "TCP", 1, requestData);

    if (!sendData(packet)) {
        return false;
    }

    std::string serverResponse = receiveData();
    if (serverResponse.empty()) {
        return false;
    }

    std::string payload = extractPayload(serverResponse);
    if (payload.empty()) {
        logError("Authentication response payload is empty.");
        return false;
    }

    auto responseMap = decodeResponse(payload);
    if (responseMap["status"] == "success") {
        logInfo("Authentication successful for user: " + username);
        return true;
    } else {
        logError("Authentication failed: " + responseMap["message"]);
        return false;
    }
}

std::vector<Task> Client::getTasks() {
    std::vector<Task> tasks;
    if (!isConnected_) {
        logError("Cannot get tasks; client is not connected.");
        return tasks;
    }

    std::map<std::string, std::string> request;
    request["type"] = "get_tasks";
    std::string requestData = encodeRequest(request);

    std::string packet = createPacket("client", "server", "TCP", 2, requestData);
    if (!sendData(packet)) {
        return tasks;
    }

    std::string response = receiveData();
    if (response.empty()) {
        return tasks;
    }

    std::string payload = extractPayload(response);
    auto responseMap = decodeResponse(payload);

    if (responseMap["status"] == "success" && responseMap.find("tasks") != responseMap.end()) {
        // Example format: "AC123:Engine Check:Not Started;AC456:Fuel System:In Progress"
        std::string tasksStr = responseMap["tasks"];
        std::stringstream ss(tasksStr);
        std::string taskSegment;
        while (std::getline(ss, taskSegment, ';')) {
            Task t;
            std::stringstream taskStream(taskSegment);
            std::string item;
            int index = 0;
            while (std::getline(taskStream, item, ':')) {
                if (index == 0) t.aircraft = item;
                if (index == 1) t.taskName = item;
                if (index == 2) t.status   = item;
                index++;
            }
            tasks.push_back(t);
        }
        logInfo("Retrieved tasks from server.");
    } else {
        logError("Failed to retrieve tasks: " + responseMap["message"]);
    }

    return tasks;
}

bool Client::updateTaskStatus(const Task& task) {
    if (!isConnected_) {
        logError("Cannot update task; client is not connected.");
        return false;
    }

    std::map<std::string, std::string> request;
    request["type"]     = "update_task_status";
    request["aircraft"] = task.aircraft;
    request["taskName"] = task.taskName;
    request["status"]   = task.status;

    std::string requestData = encodeRequest(request);
    std::string packet = createPacket("client", "server", "TCP", 3, requestData);

    if (!sendData(packet)) {
        return false;
    }

    std::string response = receiveData();
    if (response.empty()) {
        return false;
    }

    std::string payload = extractPayload(response);
    auto responseMap = decodeResponse(payload);
    if (responseMap["status"] == "success") {
        logInfo("Task status updated successfully.");
        return true;
    } else {
        logError("Failed to update task status: " + responseMap["message"]);
        return false;
    }
}

std::string Client::getAircraftStatus(const std::string& aircraftId) {
    if (!isConnected_) {
        logError("Cannot get aircraft status; client is not connected.");
        return "";
    }

    std::map<std::string, std::string> request;
    request["type"]       = "get_aircraft_status";
    request["aircraftId"] = aircraftId;

    std::string requestData = encodeRequest(request);
    std::string packet = createPacket("client", "server", "TCP", 4, requestData);

    if (!sendData(packet)) {
        return "";
    }

    std::string response = receiveData();
    if (response.empty()) {
        return "";
    }

    std::string payload = extractPayload(response);
    auto responseMap = decodeResponse(payload);
    if (responseMap["status"] == "success") {
        logInfo("Retrieved aircraft status for " + aircraftId);
        return responseMap["aircraft_status"];
    } else {
        logError("Failed to retrieve aircraft status: " + responseMap["message"]);
        return "";
    }
}

bool Client::updateAircraftStatus(const std::string& aircraftId, const std::string& newStatus) {
    if (!isConnected_) {
        logError("Cannot update aircraft status; client is not connected.");
        return false;
    }

    std::map<std::string, std::string> request;
    request["type"]       = "update_aircraft_status";
    request["aircraftId"] = aircraftId;
    request["newStatus"]  = newStatus;

    std::string requestData = encodeRequest(request);
    std::string packet = createPacket("client", "server", "TCP", 5, requestData);

    if (!sendData(packet)) {
        return false;
    }

    std::string response = receiveData();
    if (response.empty()) {
        return false;
    }

    std::string payload = extractPayload(response);
    auto responseMap = decodeResponse(payload);
    if (responseMap["status"] == "success") {
        logInfo("Aircraft status updated successfully.");
        return true;
    } else {
        logError("Failed to update aircraft status: " + responseMap["message"]);
        return false;
    }
}

bool Client::sendData(const std::string& data) {
    if (!isConnected_) {
        logError("sendData() called but client is not connected.");
        return false;
    }

    // send() in Windows returns the number of bytes sent or SOCKET_ERROR
    int bytesSent = ::send(sockFd_, data.c_str(), static_cast<int>(data.size()), 0);
    if (bytesSent == SOCKET_ERROR) {
        logError("Failed to send data to server.");
        return false;
    }

    logInfo("Sent packet: " + data);
    return true;
}

std::string Client::receiveData() {
    if (!isConnected_) {
        logError("receiveData() called but client is not connected.");
        return "";
    }

    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));

    int bytesRead = recv(sockFd_, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead == SOCKET_ERROR) {
        logError("Failed to receive data from server.");
        return "";
    } else if (bytesRead == 0) {
        logInfo("Server closed the connection.");
        disconnect();
        return "";
    }

    std::string data(buffer, bytesRead);
    logInfo("Received packet: " + data);
    return data;
}

std::string Client::encodeRequest(const std::map<std::string, std::string>& requestData) {
    std::ostringstream oss;
    bool first = true;
    for (const auto& kv : requestData) {
        if (!first) oss << ";";
        oss << kv.first << "=" << kv.second;
        first = false;
    }
    return oss.str();
}

std::map<std::string, std::string> Client::decodeResponse(const std::string& responseData) {
    std::map<std::string, std::string> result;
    std::stringstream ss(responseData);
    std::string segment;

    while (std::getline(ss, segment, ';')) {
        auto pos = segment.find('=');
        if (pos != std::string::npos) {
            std::string key = segment.substr(0, pos);
            std::string val = segment.substr(pos + 1);
            result[key] = val;
        }
    }
    return result;
}
