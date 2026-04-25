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

// фільтрує та виводе лоґи за рівнем
void Logger::filterByLevel(LogLevel level) const {
    std::string targetLevel = (level == LogLevel::INFO) ? "INFO" : 
                              (level == LogLevel::WARNING) ? "WARNING" : "ERROR";
    
    std::cout << "\n========== FILTERING BY LVL: " << targetLevel << " ==========" << std::endl;
    
    bool found = false;
    for (const auto& entry : logs) {
        if (entry.level == level) {
            std::tm* lt = std::localtime(&entry.timestamp);
            std::cout << "[" << std::put_time(lt, "%H:%M:%S") << "] " << entry.message << std::endl;
            found = true;
        }
    }
    
    if (!found) {
        std::cout << "No logs found for this level." << std::endl;
    }
    std::cout << "========================================================================" << std::endl;
}

// вивід лоґів за ключовим словом
void Logger::findByMessage(const std::string& keyword) const {
    std::cout << "\n========== SEARCHING FOR KEYWORD: \"" << keyword << "\" ==========" << std::endl;
    
    bool found = false;
    for (const auto& entry : logs) {
        // Перевіряємо, чи є слово keyword всередині entry.message
        if (entry.message.find(keyword) != std::string::npos) {
            std::tm* lt = std::localtime(&entry.timestamp);
            std::cout << "[" << std::put_time(lt, "%H:%M:%S") << "] " << entry.message << std::endl;
            found = true;
        }
    }
    
    if (!found) {
        std::cout << "No logs found containing this keyword." << std::endl;
    }
    std::cout << "========================================================================" << std::endl;
}

// вивід статистики по лоґах
void Logger::printStatistics() const {
    int infoCount = 0;
    int warningCount = 0;
    int errorCount = 0;

    for (const auto& entry : logs) {
        switch (entry.level) {
            case LogLevel::INFO:    infoCount++; break;
            case LogLevel::WARNING: warningCount++; break;
            case LogLevel::ERROR:   errorCount++; break;
        }
    }

    std::cout << "\n========== LOG STATISTICS ==========" << std::endl;
    std::cout << "Total entries: " << logs.size() << std::endl;
    std::cout << "[INFO]:    " << infoCount << std::endl;
    std::cout << "[WARNING]: " << warningCount << std::endl;
    std::cout << "[ERROR]:   " << errorCount << std::endl;
    
    if (logs.size() > 0) {
        // Проста математика: відсоток помилок
        double errorRate = (static_cast<double>(errorCount) / logs.size()) * 100;
        std::cout << "Error Rate: " << errorRate << "%" << std::endl;
    }
    std::cout << "========================================================================" << std::endl;
}