#pragma once 
#include <vector>
#include <string>
#include <ctime>
#include <chrono>

enum class LogLevel { DEBUG, TRACE, INFO, WARNING, ERROR, FATAL };

struct LogEntry {
    std::time_t timestamp;
    LogLevel level;
    std::string message;

    LogEntry(LogLevel l, std::string msg);
};

// ==================================|КЛАС ТАЙМЕРУ|==================================
class Timer {
private:
    std::string functionName;
    std::chrono::time_point<std::chrono::high_resolution_clock> startPoint;
    class Logger& loggerRef; // посилання на об'єкт лоґеру для запису результатів

public:
    // конструктор - запам'ятовує час старту та назву функції
    Timer(const std::string& name, class Logger& logger);
    
    // деструктор - автоматично рахує час при виході з функції та пише в лоґ
    ~Timer();
};

// ==================================|КЛАС  ЛОҐЕРУ|==================================
class Logger {
private:
    std::vector<LogEntry> logs;
    bool showInConsole;
    std::string getColor(LogLevel level) const;
    std::string levelToString(LogLevel level) const;
    void printLogEntry(const LogEntry& entry) const;

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
    void fatal(const std::string& msg);
    void sortByTimestamp(bool descending = true);
    bool isEmpty() const; 
};