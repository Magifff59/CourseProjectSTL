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
    std::string threadId;
    std::string funcName;

    LogEntry(LogLevel l, std::string msg, std::string fName = "");
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

    void addLog(LogLevel level, const std::string& msg, std::string funcName = "");
    void printLogs() const;
    void saveToFile(const std::string& filename) const;
    void saveToFile(const std::string& filename, const std::vector<LogEntry>& data) const;
    std::vector<LogEntry> filterByLevel(LogLevel level) const;
    std::vector<LogEntry> findByMessage(const std::string& keyword) const;
    void printStatistics() const;
    void clearLogs();
    void removeLogsByLevel(LogLevel level);
    void sortByTimestamp(bool descending = true);
    bool isEmpty() const; 
    void loadFromFile(const std::string& filename);
    std::vector<LogEntry> getRecentLogs(const std::vector<LogEntry>& source, int hours) const;
    size_t getCount() const { return logs.size(); }
    // Допоміжні функції для валідації вводу
    char getValidChar(const std::string& prompt, const std::string& validOptions);
    int getValidInt(const std::string& prompt, int min, int max);
};

#define LOG_TRACE(logger, msg) logger.addLog(LogLevel::TRACE, msg, __FUNCTION__)
#define LOG_ERROR(logger, msg) logger.addLog(LogLevel::ERROR, msg, __FUNCTION__)
#define LOG_FATAL(logger, msg) logger.addLog(LogLevel::FATAL, msg, __FUNCTION__)

// ===============================|КЛАС ПРОФАЙЛЕРИНҐУ|===============================
class Profiler {
    Logger& logger;
    std::string actionName;
    std::chrono::steady_clock::time_point startTime;

public:
    // при створенні фіксація часу
    Profiler(Logger& l, const std::string& name) 
        : logger(l), actionName(name), startTime(std::chrono::steady_clock::now()) {}

    // при знищенні (кінець блоку {}) рахує різницю і пише в лоґ
    ~Profiler() {
        auto endTime = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> duration = endTime - startTime;
        
        std::string report = "PROFILER: " + actionName + " took " + std::to_string(duration.count()) + " ms";
        
        // метод TRACE для звіту
        logger.addLog(LogLevel::TRACE, report, "Profiler");
    }
};