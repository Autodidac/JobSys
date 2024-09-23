#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <stdexcept>
#include <string>

class Logger {
public:
    // Logs a message to file and console
    static void log(const std::string& message, bool toFile = true) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (toFile && logFile_.is_open()) {
            logFile_ << message << std::endl;
        }
        logBuffer_ << message << std::endl;
        std::cout << message << std::endl;
    }

    // Opens the log file
    static void openLogFile(const std::string& filename) {
        logFile_.open(filename);
        if (!logFile_) {
            throw std::runtime_error("Failed to open log file.");
        }
    }

    // Closes the log file
    static void closeLogFile() {
        if (logFile_.is_open()) {
            logFile_.close();
        }
    }

    // Gets the log buffer
    static std::ostringstream& getBuffer() {
        return logBuffer_;
    }

private:
    static std::mutex mutex_;              // Mutex for thread safety
    static std::ofstream logFile_;         // Log file stream
    static std::ostringstream logBuffer_; // Buffer for log messages
};

// Define static members
std::mutex Logger::mutex_;
std::ofstream Logger::logFile_;
std::ostringstream Logger::logBuffer_;
