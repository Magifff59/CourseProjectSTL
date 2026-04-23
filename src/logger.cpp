#include "../include/logger.h"
#include <iostream>
#include <fstream>
#include <iomanip>

LogEntry::LogEntry(LogLevel l, std::string msg) : level(l), message(msg) {
    timestamp = std::time(nullptr);
}

void Logger::addLog(LogLevel level, const std::string& msg) {
    logs.emplace_back(level, msg);
}

void Logger::printLogs() const {
    for (const auto& entry : logs) {
        std::tm* lt = std::localtime(&entry.timestamp);
        std::cout << "[" << std::put_time(lt, "%H:%M:%S") << "] " << entry.message << std::endl;
    }
}

void Logger::saveToFile(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        for (const auto& entry : logs) {
            outFile << entry.message << "\n";
        }
        outFile.close();
    }
}