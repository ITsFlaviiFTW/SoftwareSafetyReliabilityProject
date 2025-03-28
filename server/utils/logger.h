#ifndef LOGGER_H
#define LOGGER_H

#include <string>

// Logger class provides thread-safe logging to a local file.
class Logger {
public:
    // Returns the singleton instance of Logger.
    static Logger& getInstance();
    
    // Logs an entry with a timestamp.
    void log(const std::string& entry);
    
private:
    Logger();                             // Private constructor for singleton pattern.
    ~Logger();
    Logger(const Logger&) = delete;       // Disable copy constructor.
    Logger& operator=(const Logger&) = delete;  // Disable assignment operator.
};

#endif // LOGGER_H
