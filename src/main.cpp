#include "../include/logger.h"
#include <iostream>

int main() {
    std::cout << "Running Logger Application..." << std::endl;
    
    Logger myLogger; 
    
    myLogger.addLog(LogLevel::INFO, "Project started successfully");
    myLogger.addLog(LogLevel::WARNING, "Testing warning message");
    
    myLogger.saveToFile("logs.txt");
    
    std::cout << "Done! Check your logs.txt file." << std::endl;
    return 0;
}