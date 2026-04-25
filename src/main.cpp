#include "../include/logger.h"
#include <iostream>

void someComplexFunction(Logger& logger) {
    // записуємо вхід у функцію
    logger.trace("someComplexFunction", "Starting heavy computation...");
}

int main() {
    Logger myLogger; 

    myLogger.addLog(LogLevel::INFO, "App started");
    myLogger.addLog(LogLevel::ERROR, "Old error to delete");
    
    someComplexFunction(myLogger);

    std::cout << "Before deletion: " << std::endl;
    myLogger.printStatistics();

    // Видаляємо помилки
    myLogger.removeLogsByLevel(LogLevel::ERROR);

    std::cout << "After deletion: " << std::endl;
    myLogger.printStatistics();

    return 0;
}