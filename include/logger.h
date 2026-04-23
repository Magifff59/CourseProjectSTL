#pragma once 
#include <vector>
#include <string>
#include <ctime>

enum class LogLevel { INFO, WARNING, ERROR };

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
    Logger(bool consoleOutput = true); // конструктор
    ~Logger();                         // деструктор 

    void addLog(LogLevel level, const std::string& msg);
    void printLogs() const;
    void saveToFile(const std::string& filename) const;
};