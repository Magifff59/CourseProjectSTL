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
    std::vector<std::string> messages = {
        "System boot", "User login", "Database sync", 
        "Network timeout", "Critical service failure", "Invalid API key"
    };
    
    LogLevel levels[] = {
        LogLevel::DEBUG, 
        LogLevel::INFO, 
        LogLevel::WARNING, 
        LogLevel::ERROR
    };

    for (int i = 0; i < count; ++i) {
        logger.addLog(levels[rand() % 4], messages[rand() % messages.size()]);
    }
}

// --- ОСНОВНА ПРОГРАМА ---

int main() {
    srand(static_cast<unsigned int>(time(0)));
    Logger myLogger(true); 
    
    int choice;
    do {
        clearScreen(); // Очищаємо екран при кожному поверненні в меню
        std::cout << "===== ENCHANTIX LOGGER MENU =====" << std::endl;
        std::cout << "[1] Add new log manually;" << std::endl;
        std::cout << "[2] Show all logs;" << std::endl;
        std::cout << "[3] Filter by level;" << std::endl;  
        std::cout << "[4] Search by keyword;" << std::endl;  
        std::cout << "[5] Show statistics;" << std::endl;    
        std::cout << "[6] Clear all logs;" << std::endl;  
        std::cout << "[7] Generate 5 random logs;" << std::endl;
        std::cout << "[8] Test TRACE and Profiler;" << std::endl;
        std::cout << "[0] Exit." << std::endl;
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
                
                // очищення буферу ПЕРЕД getline, щоб не проскочило введення тексту
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
                std::cout << "Enter message: ";
                std::getline(std::cin, msg);
                
                myLogger.addLog(static_cast<LogLevel>(lvlIdx), msg);
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
                myLogger.filterByLevel(static_cast<LogLevel>(lvlIdx));
                waitForEnter();
                break;
            }
            
            // пошук за ключовим словом
            case 4: {
                std::string key;
                std::cout << "Enter keyword to search: ";
                
                // очищуємо ПЕРЕД getline
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::getline(std::cin, key);
                myLogger.findByMessage(key);

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
            case 7:
                generateRandomLogs(myLogger, 5);
                std::cout << "5 logs generated." << std::endl;
                waitForEnter();
                break;
            
            // тест ручного трасування
            case 8: {
                {
                    Timer t("Manual Trace Test", myLogger);
                    myLogger.trace("main()", "User requested a manual trace test.");
                    std::cout << "[System] Trace log added and timed." << std::endl;
                } 
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