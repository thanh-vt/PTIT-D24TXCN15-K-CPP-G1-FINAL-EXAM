#include "ui.hpp"
#include "database.hpp"
#include <iostream>
#include <limits>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <cstdlib>
#include <termios.h>
#include <unistd.h>
#endif

UI &UI::getInstance() {
    static UI instance;
    return instance;
}

UI::UI() {
}

UI::~UI() {
}

void UI::start() {
    // Database is already loaded by the Database singleton constructor
    // No need to load it again here

    while (true) {
        clearScreen();
        std::cout << "=== E-Wallet Management System ===\n\n";
        std::cout << "1. Login\n";
        std::cout << "2. Register\n";
        std::cout << "3. Exit\n\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                auto user = login();
                if (user) {
                    if (user->isAdmin()) {
                        showAdminMenu(user);
                    } else {
                        showUserMenu(user);
                    }
                }
                break;
            }
            case 2:
                registerUser();
                break;
            case 3:
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                waitForEnter();
        }
    }
}

void UI::showMainMenu() {
    clearScreen();
    std::cout << "=== Main Menu ===\n\n";
    std::cout << "1. User Management\n";
    std::cout << "2. Wallet Management\n";
    std::cout << "3. Logout\n\n";
    std::cout << "Enter your choice: ";
}

void UI::showUserMenu(std::shared_ptr<User> user) {
    while (true) {
        clearScreen();
        std::cout << "=== User Menu ===\n\n";
        std::cout << "1. View Profile\n";
        std::cout << "2. Change Password\n";
        std::cout << "3. Enable/Disable 2FA\n";
        std::cout << "4. Wallet Management\n";
        std::cout << "5. Logout\n\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                viewProfile(user);
                break;
            case 2:
                changePassword(user);
                break;
            case 3:
                toggle2FA(user);
                break;
            case 4:
                showWalletMenu(user);
                break;
            case 5:
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                waitForEnter();
        }
    }
}

void UI::showAdminMenu(std::shared_ptr<User> admin) {
    while (true) {
        clearScreen();
        std::cout << "=== Admin Menu ===\n\n";
        std::cout << "1. View All Users\n";
        std::cout << "2. Create User\n";
        std::cout << "3. Update User\n";
        std::cout << "4. Delete User\n";
        std::cout << "5. View All Wallets\n";
        std::cout << "6. Logout\n\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                listUsers();
                break;
            case 2:
                createUser();
                break;
            case 3:
                // Update user
                break;
            case 4:
                deleteUser();
                break;
            case 5:
                // View all wallets
                break;
            case 6:
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                waitForEnter();
        }
    }
}

void UI::showWalletMenu(std::shared_ptr<User> user) {
    while (true) {
        clearScreen();
        std::cout << "=== Wallet Menu ===\n\n";
        std::cout << "1. View Balance\n";
        std::cout << "2. Transfer Points\n";
        std::cout << "3. Transaction History\n";
        std::cout << "4. Back\n\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                viewBalance(user);
                break;
            case 2:
                transferPoints(user);
                break;
            case 3:
                viewTransactionHistory(user);
                break;
            case 4:
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                waitForEnter();
        }
    }
}

std::string UI::getPasswordInput(const std::string &prompt) {
    std::string password;
    std::cout << prompt;

#ifdef _WIN32
    char ch;
    while ((ch = _getch()) != '\r') {  // Read until Enter key
        if (ch == '\b') {  // Backspace
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b";  // Move back, print space, move back again
            }
        } else {
            password += ch;
            std::cout << '*';  // Print asterisk instead of the character
        }
    }
    std::cout << std::endl;
#else
    // Save terminal settings
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO; // Disable echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Read password
    std::getline(std::cin, password);

    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    return password;
}

std::shared_ptr<User> UI::login() {
    clearScreen();
    std::cout << "=== Login ===\n\n";

    // TODO login

    return nullptr;
}

bool UI::authenticateUser(const std::string &username, const std::string &password) {
    // TODO authenticate user
    return true;
}

bool UI::verify2FA(std::shared_ptr<User> user) {
    if (!user->has2FA()) return true;

    std::string otp = getInput("Enter 2FA code: ");
    return user->verify2FA(otp);
}

void UI::createUser() {
    clearScreen();
    std::cout << "=== Create User ===\n\n";

    // TODO create user
    waitForEnter();
}

void UI::deleteUser() {
    clearScreen();
    // TODO delete user
    waitForEnter();
}

void UI::listUsers() {
    clearScreen();
    std::cout << "=== User List ===\n\n";
    // TODO view users
    waitForEnter();
}

void UI::viewBalance(std::shared_ptr<User> user) {
    clearScreen();
    std::cout << "=== Wallet Balance ===\n\n";
    // TODO view balance
    waitForEnter();
}

void UI::transferPoints(std::shared_ptr<User> user) {
    clearScreen();
    std::cout << "=== Transfer Points ===\n\n";

    // TODO transfer points
    waitForEnter();
}

void UI::viewTransactionHistory(std::shared_ptr<User> user) {
    if (!user) {
        std::cout << "No user selected.\n";
        return;
    }

    std::cout << "\nTransaction History for " << user->getUsername() << ":\n";
    // TODO view transaction history
    waitForEnter();
}

void UI::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void UI::waitForEnter() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string UI::getInput(const std::string &prompt) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
}

double UI::getAmountInput(const std::string &prompt) {
    double amount;
    while (true) {
        std::cout << prompt;
        if (std::cin >> amount && amount > 0) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return amount;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid amount. Please try again.\n";
    }
}

std::chrono::system_clock::time_point UI::getDateInput(const std::string &prompt) {
    while (true) {
        std::string dateStr = getInput(prompt);
        std::tm tm = {};
        std::stringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%d");

        if (!ss.fail()) {
            return std::chrono::system_clock::from_time_t(std::mktime(&tm));
        }
        std::cout << "Invalid date format. Please use YYYY-MM-DD.\n";
    }
}

void UI::registerUser() {
    clearScreen();
    std::cout << "=== User Registration ===\n\n";

    // Get username
    // TODO register user
    waitForEnter();
}

void UI::toggle2FA(std::shared_ptr<User> user) {
    clearScreen();
    std::cout << "=== 2FA Settings ===\n\n";

    // TODO toggle 2FA
    waitForEnter();
}

void UI::changePassword(std::shared_ptr<User> user) {
    clearScreen();
    // TODO change password
    waitForEnter();
}

void UI::viewProfile(std::shared_ptr<User> user) {
    clearScreen();
    std::cout << "=== User Profile ===\n\n";
    // TODO view profile

    waitForEnter();
}
