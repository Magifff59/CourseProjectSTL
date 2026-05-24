#include "../include/logger.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <map>
#include <vector>
#include <thread>

#define ANSI_RESET "\033[0m"

// ===========================|БЛОК ІНІЦІАЛІЗАЦІЇ|===========================
// конструктор запису 
LogEntry::LogEntry(LogLevel l, std::string msg, std::string fName) 
    : level(l), message(msg), funcName(fName) {
    
    timestamp = std::time(nullptr);
    
    // отримання ID потоку
    std::stringstream ss;
    ss << std::this_thread::get_id();
    threadId = ss.str();
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
    std::cout << "\033[94m[System] Logger shutting down. All logs saved to logs.txt\033[0m" << std::endl;
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
// додає лоґ та виводить у консоль (якщо дозволено)
void Logger::addLog(LogLevel level, const std::string& msg, std::string funcName) {
    // emplace_back для ефективности
    logs.emplace_back(level, msg, funcName);

    if (showInConsole) {
        // виклик допоміжного методу для виводу останнього запису
        printLogEntry(logs.back());
    }
}

// допоміжний метод для форматованого виводу одного запису
void Logger::printLogEntry(const LogEntry& entry) const {
    std::tm* lt = std::localtime(&entry.timestamp);
    // додавання кольору до рядка
    std::cout << getColor(entry.level);

    // мітка для TRACE
    if (entry.level == LogLevel::TRACE) std::cout << "[~] ";

    // вивід часу, рівня та ID потоку
    std::cout << "[" << std::put_time(lt, "%H:%M:%S") << "] "
              << "[" << levelToString(entry.level) << "] "
              << "[TID: " << entry.threadId << "] "; // додано ID потоку

    // якщо назва функції не порожня - виводить її (елемент трасування)
    if (!entry.funcName.empty()) {
        std::cout << "{FUNC: " << entry.funcName << "()} -> ";
    }

    std::cout << entry.message << "\033[0m" << std::endl;
}

// виводе УСІ лоґи у консоль
void Logger::printLogs() const {
    
    std::cout << "\n\033[1;36m  +-------------------------------------+" << std::endl;
    std::cout << "  |           FULL LOG HISTORY          |" << std::endl;
    std::cout << "  +-------------------------------------+" << std::endl;
    
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

    std::cout << "\033[94m[System] Logs sorted " << (descending ? "(newest first)" : "(oldest first)") << "\033[0m" << std::endl;
}

// фільтрує та виводе лоґи за рівнем і повертає їх список для експортування
std::vector<LogEntry> Logger::filterByLevel(LogLevel level) const {
    std::vector<LogEntry> result; // зберігання копії знайдених лоґів
    
    std::string targetLevel = levelToString(level);
    
    std::cout << "\n\033[1;36m  +-----------------------------------------+" << std::endl;
    std::cout << "           FILTERED BY LEVEL: " << targetLevel << "        " << std::endl;
    std::cout << "  +-----------------------------------------+" << std::endl;

    
    for (const auto& entry : logs) {
        if (entry.level == level) {
            printLogEntry(entry); // вивід у консоль
            result.push_back(entry); // додавання у вектор для експорту
        }
    }
    
    if (result.empty()) {
        std::cout << "No logs found for this level." << std::endl;
    }
    
    return result;
}

// вивід лоґів за ключовим словом і повертає їх список для експортування
std::vector<LogEntry> Logger::findByMessage(const std::string& keyword) const {
    std::vector<LogEntry> result; // Вектор для збереження результатів пошуку

    // копія ключового слова в нижньому регістрі
    std::string lowerKeyword = keyword;
    std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), 
                   [](unsigned char c){ return std::tolower(c); });

    std::cout << "\n\033[1;36m  +-----------------------------------------+" << std::endl;
    std::cout << "              SEARCHED FOR: " << keyword << std::endl;
    std::cout << "  +-----------------------------------------+" << std::endl;

    for (const auto& entry : logs) {
        // копія тексту лоґу також у нижньому регістрі
        std::string lowerMessage = entry.message;
        std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(), 
                       [](unsigned char c){ return std::tolower(c); });

        // пошук входження
        if (lowerMessage.find(lowerKeyword) != std::string::npos) {
            printLogEntry(entry); // друкує у консоль
            result.push_back(entry); // додає до списку результатів
        }
    }

    if (result.empty()) {
        std::cout << "No logs found matching your request." << std::endl;
    }
    
    return result; // повертає вектор знайдених лоґів
}

// для перевірки символів (y/n)
char Logger::getValidChar(const std::string& prompt, const std::string& validOptions) {
    char input;
    while (true) {
        std::cout << prompt;
        std::cin >> input;
        input = std::tolower(input); // щоб працювала велика та мала літери

        // очищення буферу від усього що залишилось після першого символу
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (validOptions.find(input) != std::string::npos) return input; // якщо символ є допустимий - повертає його
        
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\033[1;31m  [!] Invalid choice! Use one of: " << validOptions << "\033[0m\n";
    }
}

// для перевірки чисел (діапазон від min до max)
int Logger::getValidInt(const std::string& prompt, int min, int max) {
    int input;
    while (true) {
        std::cout << prompt;
        if (std::cin >> input && input >= min && input <= max) return input;
        
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\033[1;31m  [!] Invalid input! Enter a number between " << min << " and " << max << ".\033[0m\n";
    }
}

// отримання лоґів за останні N годин
std::vector<LogEntry> Logger::getRecentLogs(const std::vector<LogEntry>& source, int hours) const {

    std::vector<LogEntry> result;
    std::time_t now = std::time(nullptr);
    std::time_t threshold = now - (static_cast<long long>(hours) * 3600); 

    for (const auto& entry : source) {
        if (entry.timestamp >= threshold) {
            result.push_back(entry);
        }
    }
    return result;
}

// вивід статистики по лоґах
void Logger::printStatistics() const {
    if (logs.empty()) {
        std::cout << "\n\033[33m[!] No logs available to analyze.\033[0m" << std::endl;
        return;
    }

    // карта для автоматичного підрахунку
    std::map<LogLevel, int> counts;
    
    // ініціалізація всіх можливих рівнів нулями
    counts[LogLevel::DEBUG] = 0;
    counts[LogLevel::TRACE] = 0;
    counts[LogLevel::INFO] = 0;
    counts[LogLevel::WARNING] = 0;
    counts[LogLevel::ERROR] = 0;
    counts[LogLevel::FATAL] = 0;

    // підрахування одиним циклом
    for (const auto& entry : logs) {
        counts[entry.level]++;
    }

    // --- ДИЗАЙН СТАТИСНИКИ ---
    std::cout << "\n\033[1;36m  +-----------------------------------------+" << std::endl;
    std::cout << "  |             LOG STATISTICS              |" << std::endl;
    std::cout << "  +-----------------------------------------+" << std::endl;

    std::cout << "  |  Total entries:           \033[1;37m" << std::setw(6) << logs.size() << "\033[1;36m        |" << std::endl;
    std::cout << "  +-----------------------------------------+" << std::endl;

    // проходка по карті і виведення даних, використовуючи уже наявні методи кольорів
    std::vector<LogLevel> order = { 
        LogLevel::DEBUG, LogLevel::TRACE, LogLevel::INFO, 
        LogLevel::WARNING, LogLevel::ERROR, LogLevel::FATAL 
    };

    for (LogLevel lvl : order) {
        std::cout << "  |  " << getColor(lvl) << std::left << std::setw(12) 
                  << "[" + levelToString(lvl) + "]" << "\033[0m" << " :           " 
                  << std::right << std::setw(6) << counts[lvl] << "  \033[1;36m      |" << std::endl;
    }

    // блок аналітики критичних помилок
    double totalErrors = counts[LogLevel::ERROR] + counts[LogLevel::FATAL];
    double errorRate = (totalErrors / logs.size()) * 100;

    std::cout << "  +-----------------------------------------+" << std::endl;
    std::cout << "  |    Critical Issues Rate:    ";
    
    // динамічний колір для відсотка
    if (errorRate > 20.0) std::cout << "\033[1;31m"; // жирний червоний - все погано
    else std::cout << "\033[1;32m";                  // зелений - все ок
    
    std::cout << std::fixed << std::setprecision(2) << std::setw(6) << errorRate << "%" << "\033[1;36m     |" << std::endl;
    std::cout << "  +-----------------------------------------+\033[0m" << std::endl;
}

// імпорт лоґів з файлу з оригінальним часом їх створення
void Logger::loadFromFile(const std::string& filename) {
    Profiler p(*this, "History Restoration");
    std::ifstream inFile(filename);
    if (!inFile) return;

    std::string line;
    int loadedCount = 0;

    while (std::getline(inFile, line)) {
        if (line.empty()) continue;

        // приклад: [0000-00-00 00:00:00] [INFO] Message
        size_t firstBracketOpen = line.find('[');
        size_t firstBracketClose = line.find(']');
        size_t secondBracketOpen = line.find('[', firstBracketClose);
        size_t secondBracketClose = line.find(']', secondBracketOpen);

        if (firstBracketOpen != std::string::npos && firstBracketClose != std::string::npos &&
            secondBracketOpen != std::string::npos && secondBracketClose != std::string::npos) {
            
            // витяг дати як рядка
            std::string timeStr = line.substr(firstBracketOpen + 1, firstBracketClose - firstBracketOpen - 1);
            
            // конвертація рядка у time_t
            std::tm tm = {};
            std::istringstream ss(timeStr);
            ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

            tm.tm_isdst = -1; // автоматичне визначення літнього часу

            std::time_t logTime = std::mktime(&tm);

            // витяг рівня як рядка
            std::string levelStr = line.substr(secondBracketOpen + 1, secondBracketClose - secondBracketOpen - 1);
            
            // витяг повідомлення
            std::string message = line.substr(secondBracketClose + 2);

            LogLevel lvl = LogLevel::INFO;
            if (levelStr == "DEBUG") lvl = LogLevel::DEBUG;
            else if (levelStr == "TRACE") lvl = LogLevel::TRACE;
            else if (levelStr == "WARNING") lvl = LogLevel::WARNING;
            else if (levelStr == "ERROR") lvl = LogLevel::ERROR;
            else if (levelStr == "FATAL") lvl = LogLevel::FATAL;

            // додавання з ОРИГІНАЛЬНИМ часом
            LogEntry entry(lvl, message); // виклик конструктору (він поставить поточний час)
            entry.timestamp = logTime;    // мануально змінюється час на той, що дістали з файлу
            logs.push_back(entry);        // додавання повністю готового об'єкту у вектор
            loadedCount++;
        }
    }
    inFile.close();
    if (loadedCount > 0) {
        std::cout << "\033[94m[System] History restored: " << loadedCount << " logs loaded." << "\033[0m" << std::endl;
    }
}

// зберігає лоґи у файл 
void Logger::saveToFile(const std::string& filename) const {

    Profiler p(const_cast<Logger&>(*this), "Full History Save");
    std::ofstream outFile(filename);

    if (outFile.is_open()) {
        for (const auto& entry : logs) {
            std::tm* localTime = std::localtime(&entry.timestamp);
            outFile << "[" << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << "] "
                    << "[" << levelToString(entry.level) << "] " << entry.message << "\n";

        }
        outFile.close();
    }
} 

// зберігає конкретний набір лоґів у файл (для експорту результатів фільтрації/пошуку)
void Logger::saveToFile(const std::string& filename, const std::vector<LogEntry>& data) const {
    Profiler p(const_cast<Logger&>(*this), "Export to " + filename);
    
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        for (const auto& entry : data) {
            std::tm* localTime = std::localtime(&entry.timestamp);
            outFile << "[" << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << "] "
                    << "[" << levelToString(entry.level) << "] " << entry.message << "\n";
        }
        outFile.close();
        std::cout << "\033[94m[System] Report saved to " << filename << "\033[0m" << std::endl;
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
