#pragma once 
#include <vector>
#include <string>
#include <ctime>

enum class LogLevel { INFO, WARNING, ERROR, TRACE };

struct LogEntry {
    std::time_t timestamp;
    LogLevel level;
    std::string message;

    LogEntry(LogLevel l, std::string msg);
};

class Logger {
private:
    std::vector<LogEntry> logs;
    bool showInConsole; 

public:
    Logger(bool consoleOutput = true); // конструктор (прапорець виводу у консоль)
    ~Logger();                         // деструктор 

    void addLog(LogLevel level, const std::string& msg);
    void printLogs() const;
    void saveToFile(const std::string& filename) const;
    void filterByLevel(LogLevel level) const;
    void findByMessage(const std::string& keyword) const;
    void printStatistics() const;
    void clearLogs();
    void removeLogsByLevel(LogLevel level);
    void trace(const std::string& funcName, const std::string& msg);
};