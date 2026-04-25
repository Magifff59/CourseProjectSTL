#include "../include/logger.h"
#include <iostream>

int main() {
    Logger myLogger; 

    myLogger.addLog(LogLevel::INFO, "System online");
    myLogger.addLog(LogLevel::WARNING, "Low memory");
    myLogger.addLog(LogLevel::ERROR, "Critical failure");
    myLogger.addLog(LogLevel::INFO, "User login");
    myLogger.addLog(LogLevel::ERROR, "Disk error");
   
    myLogger.filterByLevel(LogLevel::ERROR);
    myLogger.findByMessage("User");
    myLogger.printStatistics();

    return 0;
}