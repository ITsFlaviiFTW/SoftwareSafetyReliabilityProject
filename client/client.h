#ifndef CLIENT_H
#define CLIENT_H

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
#endif

#include <string>
#include <vector>
#include <map>

// Define your Task structure
struct Task {
    std::string aircraft;
    std::string taskName;
    std::string status;
};

class Client {
public:
    Client(const std::string& serverIp, int serverPort);
    ~Client();

    bool connectToServer();
    void disconnect();
    bool authenticate(const std::string& username, const std::string& password);
    std::vector<Task> getTasks();
    bool updateTaskStatus(const Task& task);
    std::string getAircraftStatus(const std::string& aircraftId);
    bool updateAircraftStatus(const std::string& aircraftId, const std::string& newStatus);

private:
    std::string serverIp_;
    int serverPort_;
    SOCKET sockFd_;  // Defined via winsock2.h
    bool isConnected_;

    bool sendData(const std::string& data);
    std::string receiveData();
    std::string encodeRequest(const std::map<std::string, std::string>& requestData);
    std::map<std::string, std::string> decodeResponse(const std::string& responseData);
};

#endif // CLIENT_H
