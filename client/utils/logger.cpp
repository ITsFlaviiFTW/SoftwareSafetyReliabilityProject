#include "logger.h"
#include <fstream>
#include <iostream>
#include <ctime>

static const char* LOG_FILE = "client.log";

static std::string currentDateTime() {
    std::time_t now = std::time(nullptr);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %X", std::localtime(&now));
    return buf;
}

void logInfo(const std::string& message) {
    std::ofstream ofs(LOG_FILE, std::ios::app);
    if (!ofs.is_open()) {
        std::cerr << "Unable to open log file: " << LOG_FILE << std::endl;
        return;
    }
    ofs << currentDateTime() << " [INFO] " << message << "\n";
}

void logError(const std::string& message) {
    std::ofstream ofs(LOG_FILE, std::ios::app);
    if (!ofs.is_open()) {
        std::cerr << "Unable to open log file: " << LOG_FILE << std::endl;
        return;
    }
    ofs << currentDateTime() << " [ERROR] " << message << "\n";
}
