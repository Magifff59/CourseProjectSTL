#include "../include/logger.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

#define ANSI_RESET "\033[0m"

// ===========================|БЛОК ІНІЦІАЛІЗАЦІЇ|===========================
// конструктор запису 
LogEntry::LogEntry(LogLevel l, std::string msg) : level(l), message(msg) {

    timestamp = std::time(nullptr);
}

// реалізація таймеру
Timer::Timer(const std::string& name, Logger& logger) 
    : functionName(name), loggerRef(logger) {
    startPoint = std::chrono::high_resolution_clock::now();
}
Timer::~Timer() {
    auto endPoint = std::chrono::high_resolution_clock::now();
    
    // обчислення тривалости в мікросекундах
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endPoint - startPoint).count();
    
    // переведення у мілісекунди для зручности
    double ms = duration / 1000.0;

    // автоматичний запис результату в лоґ через посилання на лоґер
    loggerRef.addLog(LogLevel::TRACE, "PROFILER: " + functionName + " execution took " + std::to_string(ms) + " ms");
}

// конструктор - ініціалізація прапорця показу у консолі
Logger::Logger(bool consoleOutput) : showInConsole(consoleOutput) {}

// деструктор - автоматично зберігає лоґи при завершенні роботи
Logger::~Logger() {
    saveToFile("logs.txt");
    std::cout << "[System] Logger shutting down. All logs saved to logs.txt" << std::endl;
}

// ========================|СЛУЖБОВІ МЕТОДИ (ГЕЛПЕР)|========================
// рівень повідомлень
std::string Logger::levelToString(LogLevel level) const {

    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::TRACE:   return "TRACE";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::FATAL:   return "FATAL";
        default:                return "UNKNOWN";
    }
}

// колір для консолі
std::string Logger::getColor(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG:   return "\033[36m";    // блакитний
        case LogLevel::TRACE:   return "\033[90m";    // сірий
        case LogLevel::INFO:    return "\033[32m";    // зелений
        case LogLevel::WARNING: return "\033[33m";    // жовтий
        case LogLevel::ERROR:   return "\033[31m";    // червоний
        case LogLevel::FATAL:   return "\033[1;31;43m"; // чирний червоний на жовтому фоні
        default:                return "\033[0m";     // скидання
    }
}

// =============================|ОСНОВНА ЛОГІКА|=============================
// додає лоґа та виводить у консоль (якщо дозволено)
void Logger::addLog(LogLevel level, const std::string& msg) {
    // emplace_back для ефективности
    logs.emplace_back(level, msg);

    if (showInConsole) {
        std::string levelStr = levelToString(level); 
        std::string color = getColor(level);
        std::string reset = "\033[0m"; // скидання кольору до стандартного

        // вивід повідомлення: колір, префікс, назва рівня, повідомлення (скидання кольору)
        std::cout << color << "[Console Log] [" << levelStr << "] " 
                  << msg << reset << std::endl;
    }
}

// трейсинґ повідомлень з ім'ям ф-ції
void Logger::trace(const std::string& funcName, const std::string& msg) {
    std::string fullMsg = "{FUNC: " + funcName + "} -> " + msg;
    addLog(LogLevel::TRACE, fullMsg);
}

// метод для фаталу
void Logger::fatal(const std::string& msg) {
    addLog(LogLevel::FATAL, msg);
}

// ====================|ВИВІД, ФІЛЬТР, ПОШУК, СТАТИСТИКА|====================
// допоміжний метод для форматованого виводу одного запису
void Logger::printLogEntry(const LogEntry& entry) const {
    std::tm* lt = std::localtime(&entry.timestamp);
    // додавання кольору до рядка
    std::cout << getColor(entry.level);

    // мітка для TRACE
    if (entry.level == LogLevel::TRACE) std::cout << "[~] ";

    std::cout << "[" << std::put_time(lt, "%H:%M:%S") << "] "
              << "[" << levelToString(entry.level) << "] " 
              << entry.message << "\033[0m" << std::endl;
}

// виводе УСІ лоґи у консоль
void Logger::printLogs() const {
    std::cout << "\n===== Full Log History =====" << std::endl;
    for (const auto& entry : logs) {
        printLogEntry(entry); // виклик printLogEntry 
    }
}

// впорядкування лоґів за часом
void Logger::sortByTimestamp(bool descending) {
    if (logs.empty()) return;

    std::sort(logs.begin(), logs.end(), [descending](const LogEntry& a, const LogEntry& b) {
        if (descending) {
            return a.timestamp > b.timestamp; // нові зверху
        } else {
            return a.timestamp < b.timestamp; // старі зверху
        }
    });

    std::cout << "[System] Logs sorted " << (descending ? "(newest first)" : "(oldest first)") << std::endl;
}

// фільтрує та виводе лоґи за рівнем
void Logger::filterByLevel(LogLevel level) const {
    std::string targetLevel = levelToString(level);
    std::cout << "\n========== FILTERING BY LVL: " << targetLevel << " ==========" << std::endl;
    
    bool found = false;
    for (const auto& entry : logs) {
        if (entry.level == level) {
            printLogEntry(entry); // виклик printLogEntry
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
    // копіювання ключового слова в нижньому регістрі (для ігнорування регістру)
    std::string lowerKeyword = keyword;
    std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), 
                   [](unsigned char c){ return std::tolower(c); });

    std::cout << "\n========== SEARCHING FOR: \"" << keyword << "\" (Case-Insensitive) ==========" << std::endl;

    bool found = false;
    for (const auto& entry : logs) {
        // копіювання тексту лоґу також у нижній регістр
        std::string lowerMessage = entry.message;
        std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(), 
                       [](unsigned char c){ return std::tolower(c); });

        // пошук входження "малого у малому"
        if (lowerMessage.find(lowerKeyword) != std::string::npos) {
            printLogEntry(entry); // виклик printLogEntry
            found = true;
        }
    }

    if (!found) {
        std::cout << "No logs found matching your request." << std::endl;
    }
    std::cout << "============================================================================" << std::endl;
}

// вивід статистики по лоґах
void Logger::printStatistics() const {
    int infoCount = 0, warningCount = 0, errorCount = 0;
    int traceCount = 0, debugCount = 0, fatalCount = 0;

    for (const auto& entry : logs) {
        switch (entry.level) {
            case LogLevel::DEBUG:   debugCount++; break;
            case LogLevel::TRACE:   traceCount++; break;
            case LogLevel::INFO:    infoCount++; break;
            case LogLevel::WARNING: warningCount++; break;
            case LogLevel::ERROR:   errorCount++; break;
            case LogLevel::FATAL:   fatalCount++; break;
        }
    }

    std::cout << "\n\033[1m========== LOG STATISTICS ==========\033[0m" << std::endl;
    std::cout << "Total entries: " << logs.size() << std::endl;
    
    // виведення кожного рівня з фірмовим кольором
    std::cout << getColor(LogLevel::DEBUG)   << "[DEBUG]:   " << debugCount   << "\033[0m" << std::endl;
    std::cout << getColor(LogLevel::TRACE)   << "[TRACE]:   " << traceCount   << "\033[0m" << std::endl;
    std::cout << getColor(LogLevel::INFO)    << "[INFO]:    " << infoCount    << "\033[0m" << std::endl;
    std::cout << getColor(LogLevel::WARNING) << "[WARNING]: " << warningCount << "\033[0m" << std::endl;
    std::cout << getColor(LogLevel::ERROR)   << "[ERROR]:   " << errorCount   << "\033[0m" << std::endl;
    std::cout << getColor(LogLevel::FATAL)   << "[FATAL]:   " << fatalCount   << "\033[0m" << std::endl;

    if (!logs.empty()) {
        // сумарний відсоток критичних проблем (ERROR + FATAL)
        double totalErrors = errorCount + fatalCount;
        double errorRate = (totalErrors / logs.size()) * 100;

        std::cout << "------------------------------------" << std::endl;
        std::cout << "Critical Issues Rate: ";
        
        // якщо помилок багато, підсвічується сам відсоток червоним
        if (errorRate > 20.0) std::cout << "\033[31m"; 
        
        std::cout << std::fixed << std::setprecision(2) << errorRate << "%" << "\033[0m" << std::endl;
    }
    std::cout << "====================================" << std::endl;
}

// =======================|РОБОТА З ФАЙЛАМИ ТА ЧИСТКА|=======================
// зберігає лоґи у файл
void Logger::saveToFile(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        for (const auto& entry : logs) {
            std::tm* localTime = std::localtime(&entry.timestamp);
            
            outFile << "[" << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << "] ";
            
            outFile << "[" << levelToString(entry.level) << "] " << entry.message << "\n";
        }
        outFile.close();
    }
}

// очищення ВСІХ лоґів
void Logger::clearLogs() {
    logs.clear();
    // юзається сірий/синій колір для системних повідомлень
    std::cout << "\033[94m[System] All logs cleared from memory.\033[0m" << std::endl;
}

// перевірка на порожнечу
bool Logger::isEmpty() const {
    return logs.empty();
}

// видалення лоґів за рівнем
void Logger::removeLogsByLevel(LogLevel level) {
    std::string levelName = levelToString(level);
    //  алгоритм, який "відсуває" непотрібні елементи в кінець, а потім видаляє їх
    auto it = std::remove_if(logs.begin(), logs.end(), [level](const LogEntry& entry) {
        return entry.level == level;
    });
    
    int count = std::distance(it, logs.end());
    logs.erase(it, logs.end());
    
    std::cout << "\033[94m[System] Removed " << count << " entries of level: " 
              << levelName << "\033[0m" << std::endl;
}
