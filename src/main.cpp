#include "../include/logger.h"
#include <iostream>

int main() {
    Logger myLogger; 

    myLogger.addLog(LogLevel::INFO, "Program started");
    myLogger.addLog(LogLevel::WARNING, "Testing automatic systems");
    myLogger.addLog(LogLevel::ERROR, "Just a test error");
    
    return 0;
}