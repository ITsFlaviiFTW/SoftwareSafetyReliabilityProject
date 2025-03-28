#include "logger.h"
#include <fstream>
#include <mutex>
#include <ctime>
#include <chrono>
#include <sstream>

// Mutex to ensure thread-safe logging.
static std::mutex logMutex;

Logger& Logger::getInstance() {
    static Logger instance;  // Guaranteed to be destroyed and instantiated on first use.
    return instance;
}

Logger::Logger() {
    // Initialization if needed.
}

Logger::~Logger() {
    // Cleanup if necessary.
}

void Logger::log(const std::string& entry) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::ofstream logFile("server_log.txt", std::ios::app);
    if (logFile.is_open()) {
        // Get current time and format as a timestamp.
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        char time_buffer[32];
        std::strftime(time_buffer, 32, "%Y-%m-%d %H:%M:%S", std::localtime(&now_time));
        logFile << "[" << time_buffer << "] " << entry << "\n";
        logFile.close();
    }
}
