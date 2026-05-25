#include "../include/logger.h"
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <limits>
 
// маркос скидання кольору у консолі
#define ANSI_RESET "\033[0m"

// --- СЕРВІСНІ ФУНКЦІЇ ІНТЕРФЕЙСУ ---

// ф-ція очищення екрану (кросплатформно)
void clearScreen() {
#ifdef _WIN32
    std::system("cls");   
#else
    std::system("clear"); 
#endif
}
// ф-ція очікування Enter
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

// -----------------------------------

// генератор лоґів для тестування
void generateRandomLogs(Logger& logger, int count) {
    Profiler p(logger, "Random Generation of " + std::to_string(count) + " logs");
    // списки створення різноманітних повідомлень
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

// дизайн банеру при запуску
void showWelcomeBanner() {
    std::cout << "\033[1;35m"; // пурпуровий колір 
    std::cout << R"(
 ______ _   _  _____ _    _          _   _ _______ _____ __  __
|  ____| \ | |/ ____| |  | |   /\   | \ | |__   __|_   _|\ \/ /
| |__  |  \| | |    | |__| |  /  \  |  \| |  | |    | |   \  /
|  __| | . ` | |    |  __  | / /\ \ | . ` |  | |    | |    \/  
| |____| |\  | |____| |  | |/ ____ \| |\  |  | |   _| |_  /  \
|______|_| \_|\_____|_|  |_/_/    \_\_| \_|  |_|  |_____|/_/\_\
    )" << std::endl;
    std::cout << "           >>> SYSTEM READY | ALL MODULES LOADED <<<" << std::endl;
    std::cout << "               [ v1.0 | High-Speed Diagnostic ]" << std::endl;
    std::cout << "\033[0m" << std::endl;
}
// дизайн головного меню
void showMenu() {
    std::cout << "\033[1;36m"; // блакитний колір
    std::cout << "  +-------------------------------------+" << std::endl;
    std::cout << "  |        ENCHANTIX LOGGER MENU        |" << std::endl;
    std::cout << "  +-------------------------------------+" << std::endl;
    std::cout << "  |  [1] Add Log    |  [5] Statistics   |" << std::endl;
    std::cout << "  |  [2] View All   |  [6] Clear Logs   |" << std::endl;
    std::cout << "  |  [3] Filter     |  [7] Generate     |" << std::endl;
    std::cout << "  |  [4] Search     |  [8] Trace Test   |" << std::endl;
    std::cout << "  +-------------------------------------+" << std::endl;
    std::cout << "  |           [0] Emergency Exit        |" << std::endl;
    std::cout << "  +-------------------------------------+" << std::endl;
}
// головна функція з кейсами вибору користувача
int main() {
    srand(static_cast<unsigned int>(time(0)));
    
    // показ банеру при запуску
    showWelcomeBanner();
    Logger myLogger(true);
    myLogger.loadFromFile("logs.txt"); 
    
    std::cout << "\033[94m[System] Database loaded. Press Enter to open Menu...\033[0m";
    std::cin.get(); // жде натискання Enter для переходу до меню

    // запам'ятовує час початку сесії 
    auto sessionStart = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(sessionStart);

    int choice;
    do {
        clearScreen();
        showMenu(); // показ меню
        
        choice = myLogger.getValidInt("  Choice >> ", 0, 8);

        switch (choice) {
            // ручне додавання лоґу
            case 1: {
                std::string msg;

                // дизайн вибору рівня лоґу
                std::cout << "\n\033[1;36m  +-----------------------------------------+" << std::endl;
                std::cout << "  |            SELECT LOG LEVEL             |" << std::endl;
                std::cout << "  +-----------------------------------------+" << std::endl;
                std::cout << "  | [0] \033[1;34mDEBUG\033[1;36m    |  [1] \033[1;36mTRACE\033[1;36m  |  [2] \033[1;32mINFO\033[1;36m  |" << std::endl;
                std::cout << "  | [3] \033[1;33mWARNING\033[1;36m  |  [4] \033[1;31mERROR\033[1;36m  |  [5] \033[1;35mFATAL\033[1;36m |" << std::endl;
                std::cout << "  +-----------------------------------------+\033[0m" << std::endl;
                
                // ф-ція перевірки для рівня
                int lvlIdx = myLogger.getValidInt("  Choice >> ", 0, 5);
                
                // перетворення індексу на тип LogLevel для зручности
                LogLevel selectedLevel = static_cast<LogLevel>(lvlIdx);

                // очищення буферу ПЕРЕД getline (на випадок залишку після getValidInt)
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
                
                std::cout << "  Enter message: ";
                std::getline(std::cin, msg);
                
                // АВТОМАТИЧНЕ ТРАСУВАННЯ ДЛЯ КРИТИЧНИХ ТИПІВ
                if (selectedLevel == LogLevel::TRACE) LOG_TRACE(myLogger, msg);
                else if (selectedLevel == LogLevel::ERROR) LOG_ERROR(myLogger, msg);
                else if (selectedLevel == LogLevel::FATAL) LOG_FATAL(myLogger, msg);
                else myLogger.addLog(selectedLevel, msg); // для всіх інших (INFO, DEBUG, WARNING) додає без назви функції

                std::cout << "\033[94m[System] Log added successfully.\033[0m" << std::endl;
                
                // підказка користувачу
                std::cout << "\n" << "\033[1;37;46m" << "(Note: You might need to press Enter twice)" << "\033[0m" << std::endl; 
                
                waitForEnter(); 
                break;
            }
            
            // показ усіх лоґів з можливістю сортування
           case 2: {
            if (myLogger.isEmpty()) {
                std::cout << "\n\033[1;33m  [!] Log list is empty.\033[0m" << std::endl;
            } else {
                int sortChoice = 0;
                
                // дизайн вибору сортування
                std::cout << "\n\033[1;36m  +-------------------------------------+" << std::endl;
                std::cout << "  |         DISPLAY CONFIGURATION       |" << std::endl;
                std::cout << "  +-------------------------------------+" << std::endl;
                std::cout << "  | [1] Newest first (Latest on top)    |" << std::endl;
                std::cout << "  | [2] Oldest first (Chronological)    |" << std::endl;
                std::cout << "  +-------------------------------------+\033[0m" << std::endl;

                // поки користувач не введе 1 або 2
                while (true) {
                    std::cout << "  Choice >> ";

                    if (!(std::cin >> sortChoice)) { 
                        // введено літери
                        std::cin.clear(); // скид прапорця помилки cin
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // чистка буферу
                        std::cout << "\n\033[1;31m  [!] Invalid input! Please enter a NUMBER (1 or 2).\033[0m" << std::endl;
                        continue; 
                    }

                    if (sortChoice != 1 && sortChoice != 2) {
                        // введено число, але не 1 чи 2
                        std::cout << "\n\033[1;31m  [!] Choice must be 1 or 2! Try again.\033[0m" << std::endl;
                        continue;
                    }

                    break; // введення правильне - вихід з циклу
                }

                // сортування та вивід (тільки після успішного вибору)
                Profiler p(myLogger, "Full Log Display");
                myLogger.sortByTimestamp(sortChoice == 1);
                myLogger.printLogs();
            }
            waitForEnter();
            break;
            }
            
            // фільтрація за рівнем
            case 3: {
                std::string msg;

                // дизайн вибору рівня фільтрації
                std::cout << "\n\033[1;36m  +-----------------------------------------+" << std::endl;
                std::cout << "  |        SELECT LOG LEVEL TO FILTER       |" << std::endl;
                std::cout << "  +-----------------------------------------+" << std::endl;
                std::cout << "  | [0] \033[1;34mDEBUG\033[1;36m    |  [1] \033[1;36mTRACE\033[1;36m  |  [2] \033[1;32mINFO\033[1;36m  |" << std::endl;
                std::cout << "  | [3] \033[1;33mWARNING\033[1;36m  |  [4] \033[1;31mERROR\033[1;36m  |  [5] \033[1;35mFATAL\033[1;36m |" << std::endl;
                std::cout << "  +-----------------------------------------+\033[0m" << std::endl;
                
                int lvlIdx = myLogger.getValidInt("  Choice >> ", 0, 5);

                // отримує первинну вибірку
                auto results = myLogger.filterByLevel(static_cast<LogLevel>(lvlIdx));

                if (!results.empty()) {
                    std::cout << "\n\033[1;36m  +-----------------------------------------+" << std::endl;
                    std::cout << "    Found: \033[1;37m" << results.size() << " entries\033[1;36m " << std::endl;
                    std::cout << "  +-----------------------------------------+\033[0m" << std::endl;
            
                    // перевірка y/n
                    char expChoice = myLogger.getValidChar("    Export to file? (YES - y / NO - n) >> ", "y or n");

                    if (expChoice == 'y') {                        
                        // перевірка вибору 1 або 2
                        int timeChoice = myLogger.getValidInt("    Filter by time? (NO - 1 / YES - 2) >> ", 1, 2);

                        std::vector<LogEntry> finalResults = results;
                        if (timeChoice == 2) {
                            // перевірка годин (наприклад від 1год до 8760год - рік)
                            int hrs = myLogger.getValidInt("    Enter hours (1-8760) >> ", 1, 8760);
                            finalResults = myLogger.getRecentLogs(results, hrs);
                            std::cout << "    Final count after filter: " << finalResults.size() << std::endl;
                        }
                        
                             if (!finalResults.empty()) {
                            std::cout << "\033[1;36m  +-----------------------------------------+\033[0m" << std::endl;
                            std::cout << "    (Enter for default 'filter_report.txt') " << std::endl;
                            std::cout << "    Filename >> ";
        
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            std::string fName;
                            std::getline(std::cin, fName);
                            
                            if (fName.empty()) fName = "filter_report.txt";
                            myLogger.saveToFile(fName, finalResults);
                        }
                    }
                } else {
                    std::cout << "  \033[1;33m[!] No matching logs found.\033[0m" << std::endl;
                }

                // підказка користувачу
                std::cout << "\n" << "\033[1;37;46m" << " (Note: You might need to press Enter twice) " << "\033[0m" << std::endl; 
                waitForEnter();
                break;
            }
            
            // пошук за ключовим словом
            case 4: {
                std::string key;
                std::cout << "\n  Enter keyword to search: ";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::getline(std::cin, key);

                std::vector<LogEntry> results;

                // ОБЛАСТЬ ДІЇ ПРОФАЙЛЕРА (тільки сам пошук у пам'яті)
                {
                    Profiler p(myLogger, "Keyword Search [" + key + "]");
                    results = myLogger.findByMessage(key); 
                } // самознищення профайлеру p і миттєво видає реальний час пошуку

                if (!results.empty()) {
                    // дизайн результатів пошуку
                    std::cout << "\n\033[1;36m  +-----------------------------------------+" << std::endl;
                    std::cout << "    Found: \033[1;37m" << results.size() << " entries\033[1;36m " << std::endl;
                    std::cout << "  +-----------------------------------------+\033[0m" << std::endl;
                    
                    // перевірка y/n
                    char expChoice = myLogger.getValidChar("    Export to file? (YES - y / NO - n) >> ", "y or n");

                    if (expChoice == 'y') {
                        
                        // перевірка вибору 1 або 2
                        int timeChoice = myLogger.getValidInt("    Filter by time? (NO - 1 / YES - 2) >> ", 1, 2);

                        std::vector<LogEntry> finalResults = results;
                        if (timeChoice == 2) {
                            // перевірка годин (наприклад від 1год до 8760год - рік)
                            int hrs = myLogger.getValidInt("    Enter hours (1-8760) >> ", 1, 8760);
                            
                            finalResults = myLogger.getRecentLogs(results, hrs);
                            std::cout << "    Final count after time filter: " << finalResults.size() << std::endl;
                        }

                        if (!finalResults.empty()) {
                            std::cout << "\033[1;36m  +-----------------------------------------+\033[0m" << std::endl;
                            std::cout << "    (Enter for default 'filter_report.txt') " << std::endl;
                            std::cout << "    Filename >> ";

                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            std::string fName;
                            std::getline(std::cin, fName);

                            if (fName.empty()) fName = "search_report.txt";
                            if (fName.find(".txt") == std::string::npos) fName += ".txt";

                            myLogger.saveToFile(fName, finalResults);
                        }
                    }
                } else {
                    std::cout << "\033[94m[System] No logs found with this keyword.\033[0m" << std::endl;
                }
                
                // підказка користувачу
                std::cout << "\n" << "\033[1;37;46m" << " (Note: You might need to press Enter twice) " << "\033[0m" << std::endl; 
                waitForEnter();
                break;
            }
            
            // показ статистики
            case 5: {
                myLogger.printStatistics();
                waitForEnter();
                break;
            }
            
            // очищення списку лоґів
            case 6: {
                if (myLogger.isEmpty()) {
                    std::cout << "\n\033[1;33m  [!] Log list is already empty.\033[0m" << std::endl;
                } else {
                    // дизайн вибору типу очищення
                    std::cout << "\n\033[1;36m  +-------------------------------------+" << std::endl;
                    std::cout << "  |           CLEANUP OPTIONS           |" << std::endl;
                    std::cout << "  +-------------------------------------+" << std::endl;
                    std::cout << "  | [1] Clear ALL logs                  |" << std::endl;
                    std::cout << "  | [2] Remove logs by specific LEVEL   |" << std::endl;
                    std::cout << "  | [0] Cancel                          |" << std::endl;
                    std::cout << "  +-------------------------------------+\033[0m" << std::endl;

                    int cleanChoice = myLogger.getValidInt("  Choice >> ", 0, 2);

                    if (cleanChoice == 1) {
                        myLogger.clearLogs();
                    } 
                    else if (cleanChoice == 2) {
                        // дизайн вибору рівня очищення
                        std::cout << "\n\033[1;36m  +-----------------------------------------+" << std::endl;
                        std::cout << "  |          SELECT LEVEL TO REMOVE         |" << std::endl;
                        std::cout << "  +-----------------------------------------+" << std::endl;
                        std::cout << "  | [0] DEBUG  | [1] TRACE | [2] INFO       |" << std::endl;
                        std::cout << "  | [3] WARN   | [4] ERROR | [5] FATAL      |" << std::endl;
                        std::cout << "  +-----------------------------------------+\033[0m" << std::endl;
                        
                        int lvlIdx = myLogger.getValidInt("  Level to wipe >> ", 0, 5);
                        myLogger.removeLogsByLevel(static_cast<LogLevel>(lvlIdx));
                    }
                    else {
                        std::cout << "\033[94m[System] Cleanup cancelled.\033[0m" << std::endl;
                    }
                }
                
                waitForEnter();
                break;
            }
            
            // генерація випадкових лоґів для тестування    
            case 7: {
                int count = myLogger.getValidInt("  How many logs to generate? (1-10000) >> ", 1, 10000);  
                generateRandomLogs(myLogger, count); 
                std::cout << "\033[94m[System] Generated " << count << " random logs.\033[0m" << std::endl;
               
                waitForEnter();
                break;
            }
            
            // тест ручного трасування
            case 8: {
                LOG_TRACE(myLogger, "Manual trace test initiated");
                std::cout << "\033[94m[System] Trace log added using macro.\033[0m" <<  std::endl;
                waitForEnter();
                break;
            }
            
            // вихід з програми
            case 0: {
                clearScreen();
                
                // розрахунок часу завершення та тривалості сесії
                auto sessionEnd = std::chrono::system_clock::now();
                std::time_t end_time = std::chrono::system_clock::to_time_t(sessionEnd);
                
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(sessionEnd - sessionStart);
                int minutes = duration.count() / 60;
                int seconds = duration.count() % 60;

                // дизайн фінального звіту при виході
                std::cout << "\n\033[1;36m+-------------------------------------------+" << std::endl;
                std::cout << "|         ENCHANTIX SESSION SUMMARY         |" << std::endl;
                std::cout << "+-------------------------------------------+\033[0m" << std::endl;
                
                char startTimeStr[9];
                char endTimeStr[9];
                std::strftime(startTimeStr, sizeof(startTimeStr), "%H:%M:%S", std::localtime(&start_time));
                std::strftime(endTimeStr, sizeof(endTimeStr), "%H:%M:%S", std::localtime(&end_time));

                std::cout << "   Start Time:     \033[1;32m" << startTimeStr << "         \033[0m" << std::endl;
                std::cout << "   End Time:       \033[1;31m" << endTimeStr << "         \033[0m" << std::endl;
                std::cout << "   Uptime:         \033[1;33m" << minutes << "m " << seconds << "s         \033[0m" << std::endl;
                std::cout << "   Logs Handled:   \033[1;35m" << myLogger.getCount() << "         \033[0m" << std::endl;
                std::cout << "   Build Version:  \033[1;34m" << __TIME__ << " (Stable)         \033[0m" << std::endl;
                
                std::cout << "\033[1;36m+-------------------------------------------+\033[0m" << std::endl;
                
                break;
            }
        }
    } while (choice != 0);

    return 0;
}