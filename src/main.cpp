#include "../include/logger.h"
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>

#define ANSI_RESET "\033[0m"

// Наш генератор (поки залишаємо порожнім або з 5-10 логами для тесту)
void generateRandomLogs(Logger& logger, int count) {
    std::vector<std::string> messages = {
        "System boot", "User login", "Database sync", 
        "Network timeout", "Critical service failure", "Invalid API key"
    };
    
    // Додаємо ERROR у список
    LogLevel levels[] = {
        LogLevel::DEBUG, 
        LogLevel::INFO, 
        LogLevel::WARNING, 
        LogLevel::ERROR
    };

    for (int i = 0; i < count; ++i) {
        // Тепер беремо % 4, щоб захопити ERROR
        logger.addLog(levels[rand() % 4], messages[rand() % messages.size()]);
    }
}

int main() {
    srand(time(0));
    Logger myLogger(true); // Консольний вивід увімкнено
    
    int choice;
    do {
        std::cout << "\n===== ENCHANTIX LOGGER MENU =====" << std::endl;
        std::cout << "1. Add new log manually" << std::endl;
        std::cout << "2. Show all logs (printLogs)" << std::endl;
        std::cout << "3. Filter by level" << std::endl;  
        std::cout << "4. Search by keyword" << std::endl;  
        std::cout << "5. Show statistics" << std::endl;    
        std::cout << "6. Clear all logs" << std::endl;
        std::cout << "7. Trigger FATAL error" << std::endl;    
        std::cout << "8. Generate 5 random logs" << std::endl;
        std::cout << "9. Test TRACE" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Choice: ";
        
        // Перевірка на випадок, якщо користувач ввів не число
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number." << std::endl;
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        switch (choice) {
            case 1: {
                int lvlIdx;
                std::string msg;
                std::cout << "Select level (0:DEBUG, 1:TRACE, 2:INFO, 3:WARN, 4:ERR, 5:FATAL): ";
                std::cin >> lvlIdx;
                std::cin.ignore(); // Очищуємо буфер перед getline
                std::cout << "Enter message: ";
                std::getline(std::cin, msg);
                
                // Перетворюємо число на LogLevel
                LogLevel level = static_cast<LogLevel>(lvlIdx);
                myLogger.addLog(level, msg);
                break;
            }
            case 2:
                myLogger.printLogs();
                break;

            case 3: { // Фільтрація
                int lvlIdx;
                std::cout << "Select level to filter (0-5): ";
                std::cin >> lvlIdx;
                myLogger.filterByLevel(static_cast<LogLevel>(lvlIdx));
                break;
            }
            case 4: { // Пошук
                std::string key;
                std::cout << "Enter keyword to search: ";
                std::cin.ignore();
                std::getline(std::cin, key);
                myLogger.findByMessage(key);
                break;
            }
            case 5: // Статистика
                myLogger.printStatistics();
                break;

            case 6: // Очищення
                myLogger.clearLogs();
                break;

            case 7: {
             std::string msg;
             std::cout << "Enter fatal error message: ";
             std::cin.ignore();
             std::getline(std::cin, msg);
             myLogger.fatal(msg); // Викликаємо нашу обгортку
             break;
            }
                
            case 8:
                generateRandomLogs(myLogger, 5);
                break;

            case 9: {
             myLogger.trace("main()", "User requested a manual trace test.");
             std::cout << "[System] Trace log added for function: main()" << std::endl;
             break;
            }

            case 0:
                std::cout << "Exiting program..." << std::endl;
                break;

            default:
                std::cout << "Unknown option. Try again." << std::endl;
        }
    } while (choice != 0);

    return 0;
}