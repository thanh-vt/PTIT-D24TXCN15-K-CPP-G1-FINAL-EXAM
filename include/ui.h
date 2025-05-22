#pragma once

#include <chrono>
#include <memory>

class UI {
public:
    static UI& getInstance();

    void start();
    void showMainMenu();

private:
    UI();
    ~UI();

    // Prevent copying
    UI(const UI&) = delete;
    UI& operator=(const UI&) = delete;

    // Helper functions
    void clearScreen();
    void waitForEnter();
    std::string getInput(const std::string& prompt);
    double getAmountInput(const std::string& prompt);
    std::chrono::system_clock::time_point getDateInput(const std::string& prompt);

};