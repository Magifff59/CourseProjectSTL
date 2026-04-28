#include "../include/logger.h"
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <limits>  

#define ANSI_RESET "\033[0m"

// --- СЕРВІСНІ ФУНКЦІЇ ІНТЕРФЕЙСУ ---

void clearScreen() {
#ifdef _WIN32
    std::system("cls");   
#else
    std::system("clear"); 
#endif
}

void waitForEnter() {
    std::cout << "\n" << ANSI_RESET << "[Press Enter to return to the menu...]";
    // перевірка залишку в буфері символу переходу на новий рядок
    if (std::cin.peek() == '\n') {
        std::cin.ignore();
    }
    // очищення стану потоку введення (на випадок помилок)
    std::cin.sync(); 
    
    // жде безпосереднього натискання клавіші користувачем
    std::cin.get(); 
}

// --- ГЕНЕРАТОР ЛОГІВ ---

void generateRandomLogs(Logger& logger, int count) {
    Profiler p(logger, "Random Generation of " + std::to_string(count) + " logs");
    // списки для створення різноманітних повідомлень
    std::vector<std::string> actions = {"User login", "Data backup", "Request", "Connection", "System update"};
    std::vector<std::string> objects = {"Database", "AuthService", "MainServer", "Storage", "UI_Module"};
    std::vector<std::string> results = {"successful", "failed", "timed out", "denied", "intercepted"};

    LogLevel levels[] = {
        LogLevel::DEBUG, 
        LogLevel::INFO, 
        LogLevel::WARNING, 
        LogLevel::ERROR
    };

    for (int i = 0; i < count; ++i) {
        // конструкція випадкової фрази для лоґу
        std::string randomMessage = 
            objects[rand() % objects.size()] + ": " + 
            actions[rand() % actions.size()] + " " + 
            results[rand() % results.size()] + " (ID: " + 
            std::to_string(rand() % 1000) + ")";

        logger.addLog(levels[rand() % 4], randomMessage);
    }
}

// --- ОСНОВНА ПРОГРАМА ---

int main() {
    srand(static_cast<unsigned int>(time(0)));
    Logger myLogger(true);
    myLogger.loadFromFile("logs.txt"); // історія лоґів 
    
    int choice;
    do {
        clearScreen(); // Очищаємо екран при кожному поверненні в меню
        std::cout << "===== ENCHANTIX LOGGER MENU =====" << std::endl;
        std::cout << "[1] Add new log manually" << std::endl;
        std::cout << "[2] Show all logs" << std::endl;
        std::cout << "[3] Filter by level" << std::endl;  
        std::cout << "[4] Search by keyword" << std::endl;  
        std::cout << "[5] Show statistics" << std::endl;    
        std::cout << "[6] Clear all logs" << std::endl;  
        std::cout << "[7] Generate random logs" << std::endl;
        std::cout << "[8] Test TRACE and Profiler" << std::endl;
        std::cout << "[0] Exit" << std::endl;
        std::cout << "=================================" << std::endl;
        std::cout << "Choice: ";
        
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            waitForEnter();
            continue;
        }

        switch (choice) {
            // ручне додавання лоґу
            case 1: {
                int lvlIdx;
                std::string msg;
                std::cout << "Select level (0:DEBUG, 1:TRACE, 2:INFO, 3:WARNING, 4:ERROR, 5:FATAL): ";
                std::cin >> lvlIdx;
                
                // перетворення індексу на тип LogLevel для зручности
                LogLevel selectedLevel = static_cast<LogLevel>(lvlIdx);

                // очищення буферу ПЕРЕД getline
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
                std::cout << "Enter message: ";
                std::getline(std::cin, msg);
                
                // АВТОМАТИЧНЕ ТРАСУВАННЯ ДЛЯ КРИТИЧНИХ ТИПІВ
                if (selectedLevel == LogLevel::TRACE) {
                    LOG_TRACE(myLogger, msg);
                } 
                else if (selectedLevel == LogLevel::ERROR) {
                    LOG_ERROR(myLogger, msg);
                } 
                else if (selectedLevel == LogLevel::FATAL) {
                    LOG_FATAL(myLogger, msg);
                } 
                else {
                    // для всіх інших (INFO, DEBUG, WARNING) додає без назви функції
                    myLogger.addLog(selectedLevel, msg);
                }

                std::cout << "[System] Log added successfully.";
                std::cout << "\n(Note: You might need to press Enter twice)"; // підказка користувачу
                waitForEnter(); 
                break;
            }
            
            // показ усіх лоґів з можливістю сортування
            case 2: {
                if (myLogger.isEmpty()) {
                    std::cout << "Log list is empty." << std::endl;
                } else {
                    std::cout << "How to display logs?\n1. Newest first\n2. Oldest first\nChoice: ";
                    int sortChoice;
                    std::cin >> sortChoice;

                    // замірювання сортування та вивід разом
                    Profiler p(myLogger, "Full Log Display");
                    myLogger.sortByTimestamp(sortChoice == 1);
                    myLogger.printLogs();
                }
                waitForEnter();
                break;
            }
            
            // фільтрація за рівнем
            case 3: {
                int lvlIdx;
                std::cout << "Select level to filter (0:DEBUG, 1:TRACE, 2:INFO, 3:WARNING, 4:ERROR, 5:FATAL): ";
                std::cin >> lvlIdx;

                // отримує первинну вибірку
                auto results = myLogger.filterByLevel(static_cast<LogLevel>(lvlIdx));

                if (!results.empty()) {
                    std::cout << "\nFound " << results.size() << " entries. Export to file? (Yes [y]/No [n]): ";
                    char choice;
                    std::cin >> choice;

                    if (choice == 'y' || choice == 'Y') {
                        // фільтр за часом (опційно)
                        std::cout << "Filter by time? \n1. No (All found)\n2. Yes (Last N hours)\nChoice: ";
                        int timeChoice;
                        std::cin >> timeChoice;

                        std::vector<LogEntry> finalResults = results;
                        if (timeChoice == 2) {
                            int hrs;
                            std::cout << "Enter hours: ";
                            std::cin >> hrs;
                            finalResults = myLogger.getRecentLogs(results, hrs);
                            std::cout << "Final count after time filter: " << finalResults.size() << std::endl;
                        }

                        if (!finalResults.empty()) {
                            // кастомне ім'я файлу
                            std::cout << "Enter filename (press Enter for 'filter_report.txt'): ";
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            std::string fName;
                            std::getline(std::cin, fName);

                            if (fName.empty()) fName = "filter_report.txt";
                            if (fName.find(".txt") == std::string::npos) fName += ".txt";

                            myLogger.saveToFile(fName, finalResults);
                        }
                    }
                }
                std::cout << "\n(Note: You might need to press Enter twice)"; // підказка користувачу
                waitForEnter();
                break;
            }
            
            // пошук за ключовим словом
            case 4: {
                std::string key;
                std::cout << "Enter keyword to search: ";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::getline(std::cin, key);

                std::vector<LogEntry> results;

                // ОБЛАСТЬ ДІЇ ПРОФАЙЛЕРА: тільки сам пошук у пам'яті
                {
                    Profiler p(myLogger, "Keyword Search [" + key + "]");
                    results = myLogger.findByMessage(key); 
                } // Тут профайлер p знищується і миттєво видає реальний час пошуку

                if (!results.empty()) {
                    std::cout << "\nFound " << results.size() << " entries. Export to file? (Yes [y]/No [n]): ";
                    char choice;
                    std::cin >> choice;

                    if (choice == 'y' || choice == 'Y') {
                        std::cout << "Filter by time? \n1. No (All found)\n2. Yes (Last N hours)\nChoice: ";
                        int timeChoice;
                        std::cin >> timeChoice;

                        std::vector<LogEntry> finalResults = results;
                        if (timeChoice == 2) {
                            int hrs;
                            std::cout << "Enter hours: ";
                            std::cin >> hrs;
                            
                            // Можна додати профайлер і сюди, якщо цікаво, як швидко фільтрує час
                            finalResults = myLogger.getRecentLogs(results, hrs);
                            std::cout << "Final count after time filter: " << finalResults.size() << std::endl;
                        }

                        if (!finalResults.empty()) {
                            std::cout << "Enter filename (press Enter for 'search_report.txt'): ";
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            std::string fName;
                            std::getline(std::cin, fName);

                            if (fName.empty()) fName = "search_report.txt";
                            if (fName.find(".txt") == std::string::npos) fName += ".txt";

                            // Тут профайлер спрацює всередині самого методу saveToFile
                            myLogger.saveToFile(fName, finalResults);
                        }
                    }
                } else {
                    std::cout << "[System] No logs found with this keyword." << std::endl;
                }

                std::cout << "\n(Note: You might need to press Enter twice)"; // підказка користувачу
                waitForEnter();
                break;
            }
            
            // показ статистики
            case 5:
                myLogger.printStatistics();
                waitForEnter();
                break;
            
            // очищення списку лоґів
            case 6:
                myLogger.clearLogs();
                std::cout << "Logs cleared!" << std::endl;
                waitForEnter();
                break;
            
            // генерація випадкових лоґів
            case 7: {
                int count;
                std::cout << "How many random logs to generate? (e.g., 100): ";
                if (!(std::cin >> count)) {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    std::cout << "Invalid number!" << std::endl;
                } else {
                    generateRandomLogs(myLogger, count);
                    std::cout << "[System] " << count << " logs generated successfully." << std::endl;
                }
                waitForEnter();
                break;
            }
            
            // тест ручного трасування
            case 8: {
                // Використовуємо наш новий макрос
                LOG_TRACE(myLogger, "User requested a manual trace test");
    
                std::cout << "[System] Trace log added using macro." << std::endl;
                waitForEnter();
                break;
            }
            
            // вихід з програми
            case 0:
                std::cout << "Exiting program..." << std::endl;
                break;
            default:
                std::cout << "Unknown option. Try again." << std::endl;
                waitForEnter();
        }
    } while (choice != 0);

    return 0;
}