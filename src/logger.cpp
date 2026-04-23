#include "../include/logger.h"
#include <iostream>
#include <fstream>
#include <iomanip>

// конструктор - ініціалізація прапорця показу у консолі
Logger::Logger(bool consoleOutput) : showInConsole(consoleOutput) {}

// деструктор - автоматично зберігає лоґи при завершенні роботи
Logger::~Logger() {
    saveToFile("logs.txt");
    std::cout << "[System] Logger shutting down. All logs saved to logs.txt" << std::endl;
}

// конструктор запису 
LogEntry::LogEntry(LogLevel l, std::string msg) : level(l), message(msg) {
    timestamp = std::time(nullptr);
}

// додає лоґа та виводить у консоль (якщо дозволено)
void Logger::addLog(LogLevel level, const std::string& msg) {
    logs.emplace_back(level, msg);

    if (showInConsole) {
        std::string levelStr;
        switch (level) {
            case LogLevel::INFO:    levelStr = "INFO"; break;
            case LogLevel::WARNING: levelStr = "WARNING"; break;
            case LogLevel::ERROR:   levelStr = "ERROR"; break;
        }
        std::cout << "[Console Log] [" << levelStr << "] " << msg << std::endl;
    }
}

// виводе УСІ лоґи у консоль
void Logger::printLogs() const {
    for (const auto& entry : logs) {
        std::tm* lt = std::localtime(&entry.timestamp);
        std::cout << "[" << std::put_time(lt, "%H:%M:%S") << "] " << entry.message << std::endl;
    }
}

// зберігає лоґи у файл
void Logger::saveToFile(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        for (const auto& entry : logs) {
            std::tm* localTime = std::localtime(&entry.timestamp);
            outFile << "[" << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << "] ";
            
            std::string levelStr;
            switch (entry.level) {
                case LogLevel::INFO:    levelStr = "INFO"; break;
                case LogLevel::WARNING: levelStr = "WARNING"; break;
                case LogLevel::ERROR:   levelStr = "ERROR"; break;
            }
            outFile << "[" << levelStr << "] " << entry.message << "\n";
        }
        outFile.close();
    }
}