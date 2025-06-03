#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <conio.h>
#else
#include <cstdlib>
#include <termios.h>
#include <unistd.h>
#endif

#include "admin.hpp"
#include "database.hpp"
#include "transaction.hpp"
#include <iostream>
#include <limits>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <chrono>
#include <algorithm>

Admin& Admin::getInstance() {
    static Admin instance;
    return instance;
}

Admin::Admin() {}

Admin::~Admin() {}

bool Admin::isAuthorized(std::shared_ptr<User> user) {
    return user && user->isAdmin();
}

void Admin::showAdminMenu(std::shared_ptr<User> admin) {
    if (!isAuthorized(admin)) {
        std::cout << "Access denied. Admin privileges required.\n";
        waitForEnter();
        return;
    }

    while (true) {
        clearScreen();
        std::cout << "=== Admin Menu ===\n\n";
        std::cout << "1. View All Users\n";
        std::cout << "2. Create User\n";
        std::cout << "3. Update User\n";
        std::cout << "4. View All Wallets\n";
        std::cout << "5. View All Transactions\n";
        std::cout << "6. Logout\n\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                adminViewAllUsers();
                break;
            case 2:
                adminCreateUser();
                break;
            case 3:
                adminUpdateUser();
                break;
            case 4:
                adminViewAllWallets();
                break;
            case 5:
                adminViewAllTransactions();
                break;
            case 6:
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                waitForEnter();
        }
    }
}

void Admin::adminViewAllUsers() {
    clearScreen();
    std::cout << "=== All Users (Admin) ===\n\n";

    auto users = Database::getInstance().getAllUsers();
    if (users.empty()) {
        std::cout << "No users found in the system.\n";
        waitForEnter();
        return;
    }

    std::cout << std::left << std::setw(15) << "Username"
              << std::setw(25) << "Full Name"
              << std::setw(10) << "2FA"
              << std::setw(10) << "Admin"
              << std::setw(15) << "Wallet ID" << "\n";
    std::cout << std::string(75, '-') << "\n";

    for (const auto& user : users) {
        std::cout << std::left << std::setw(15) << user->getUsername()
                  << std::setw(25) << user->getFullname()
                  << std::setw(10) << (user->has2FA() ? "Yes" : "No")
                  << std::setw(10) << (user->isAdmin() ? "Yes" : "No")
                  << std::setw(15) << user->getWalletId() << "\n";
    }

    std::cout << "\nTotal Users: " << users.size() << "\n";
    waitForEnter();
}

void Admin::adminCreateUser() {
    clearScreen();
    std::cout << "=== Create User (Admin) ===\n\n";

    std::string username = getInput("Username: ");
    if (Database::getInstance().getUser(username)) {
        std::cout << "Username already exists.\n";
        waitForEnter();
        return;
    }

    std::string fullname = getInput("Full Name: ");
    auto dob = getDateInput("Date of Birth (YYYY-MM-DD): ");

    // Create new user with a generated password
    User newUser(username, fullname, dob);
    std::string generatedPassword = newUser.generatePassword();  // Generate a new password
    newUser.setPassword(generatedPassword);  // Set the generated password

    // Create wallet for the user
    std::string walletId = "W" + username;  // Simple wallet ID generation
    Wallet wallet(walletId, 0.0);  // Create wallet with 0 balance

    // First add the wallet
    if (!Database::getInstance().addWallet(wallet)) {
        std::cout << "Failed to create wallet for user.\n";
        waitForEnter();
        return;
    }

    // Set the wallet ID for the user
    newUser.setWalletId(walletId);

    // Then add the user
    if (Database::getInstance().addUser(newUser)) {
        std::cout << "\nUser created successfully!\n";
        std::cout << "Generated password: " << generatedPassword << "\n";
        std::cout << "Please save this password securely.\n";

        // Verify the save was successful
        if (!Database::getInstance().saveToFiles()) {
            std::cout << "Warning: Failed to save changes to database.\n";
        }
    } else {
        std::cout << "Failed to create user.\n";
        // Clean up the wallet if user creation failed
        Database::getInstance().deleteWallet(walletId);
    }
    waitForEnter();
}



void Admin::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void Admin::waitForEnter() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string Admin::getInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
}

std::string Admin::getPasswordInput(const std::string& prompt) {
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
    newt.c_lflag &= ~ECHO;  // Disable echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Read password
    std::getline(std::cin, password);

    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    return password;
}

double Admin::getAmountInput(const std::string& prompt) {
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

std::chrono::system_clock::time_point Admin::getDateInput(const std::string& prompt) {
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

void Admin::adminUpdateUser() {
    clearScreen();
    std::cout << "=== Update User (Admin) ===\n\n";

    std::string username = getInput("Enter username to update: ");
    if (username == "admin") {
        std::cout << "Cannot modify admin user through this function.\n";
        waitForEnter();
        return;
    }

    auto user = Database::getInstance().getUser(username);
    if (!user) {
        std::cout << "User not found.\n";
        waitForEnter();
        return;
    }

    // Show current user info
    std::cout << "\nCurrent user information:\n";
    std::cout << "Username: " << user->getUsername() << "\n";
    std::cout << "Full Name: " << user->getFullname() << "\n";
    auto dob = user->getDateOfBirth();
    auto time = std::chrono::system_clock::to_time_t(dob);
    std::cout << "Date of Birth: " << std::put_time(std::localtime(&time), "%Y-%m-%d") << "\n";
    std::cout << "2FA Status: " << (user->has2FA() ? "Enabled" : "Disabled") << "\n\n";

    // Update options
    std::cout << "What would you like to update?\n";
    std::cout << "1. Full Name\n";
    std::cout << "2. Date of Birth\n";
    std::cout << "3. Toggle 2FA\n";
    std::cout << "4. Cancel\n\n";
    std::cout << "Enter your choice: ";

    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    switch (choice) {
        case 1: {
            std::string newFullname = getInput("Enter new full name: ");
            // Create updated user
            User updatedUser(user->getUsername(), newFullname, user->getDateOfBirth(), user->isAdmin());
            updatedUser.setPasswordHash(user->getPasswordHash());
            updatedUser.setWalletId(user->getWalletId());
            if (user->has2FA()) {
                updatedUser.enable2FA(user->getSecretKey());
            }

            if (Database::getInstance().updateUser(updatedUser)) {
                std::cout << "Full name updated successfully.\n";
            } else {
                std::cout << "Failed to update user.\n";
            }
            break;
        }
        case 2: {
            auto newDob = getDateInput("Enter new date of birth (YYYY-MM-DD): ");
            // Create updated user
            User updatedUser(user->getUsername(), user->getFullname(), newDob, user->isAdmin());
            updatedUser.setPasswordHash(user->getPasswordHash());
            updatedUser.setWalletId(user->getWalletId());
            if (user->has2FA()) {
                updatedUser.enable2FA(user->getSecretKey());
            }

            if (Database::getInstance().updateUser(updatedUser)) {
                std::cout << "Date of birth updated successfully.\n";
            } else {
                std::cout << "Failed to update user.\n";
            }
            break;
        }
        case 3: {
            if (user->has2FA()) {
                user->disable2FA();
                std::cout << "2FA disabled for user.\n";
            } else {
                user->enable2FA();
                std::cout << "2FA enabled for user.\n";
            }

            if (Database::getInstance().updateUser(*user)) {
                std::cout << "2FA status updated successfully.\n";
            } else {
                std::cout << "Failed to update 2FA status.\n";
            }
            break;
        }
        case 4:
            std::cout << "Update cancelled.\n";
            break;
        default:
            std::cout << "Invalid choice.\n";
    }
    waitForEnter();
}



void Admin::adminViewAllWallets() {
    clearScreen();
    std::cout << "=== All Wallets (Admin) ===\n\n";

    auto wallets = Database::getInstance().getAllWallets();
    if (wallets.empty()) {
        std::cout << "No wallets found in the system.\n";
        waitForEnter();
        return;
    }

    double totalBalance = 0.0;
    int totalTransactions = 0;

    std::cout << std::left << std::setw(15) << "Wallet ID"
              << std::setw(20) << "Owner"
              << std::setw(15) << "Balance"
              << std::setw(12) << "Transactions" << "\n";
    std::cout << std::string(62, '-') << "\n";

    for (const auto& wallet : wallets) {
        // Find wallet owner
        std::string ownerName = "Unknown";
        auto users = Database::getInstance().getAllUsers();
        for (const auto& user : users) {
            if (user->getWalletId() == wallet->getId()) {
                ownerName = user->getFullname();
                break;
            }
        }

        int transactionCount = wallet->getTransactionHistory().size();
        totalBalance += wallet->getBalance();
        totalTransactions += transactionCount;

        std::cout << std::left << std::setw(15) << wallet->getId()
                  << std::setw(20) << ownerName
                  << std::setw(15) << std::fixed << std::setprecision(2) << wallet->getBalance()
                  << std::setw(12) << transactionCount << "\n";
    }

    std::cout << std::string(62, '-') << "\n";
    std::cout << "Total Wallets: " << wallets.size() << "\n";
    std::cout << "Total Balance: " << std::fixed << std::setprecision(2) << totalBalance << " points\n";
    std::cout << "Total Transactions: " << totalTransactions << "\n";

    waitForEnter();
}





// Xem tat ca giao dich he thong
void Admin::adminViewAllTransactions() {
    clearScreen();
    std::cout << "=== All Transactions (Admin) ===\n\n";

    // Thu thap tat ca giao dich tu moi vi
    std::vector<Transaction> allTransactions;
    auto wallets = Database::getInstance().getAllWallets();

    for (const auto& wallet : wallets) {
        const auto& transactions = wallet->getTransactionHistory();
        allTransactions.insert(allTransactions.end(), transactions.begin(), transactions.end());
    }

    if (allTransactions.empty()) {
        std::cout << "No transactions found in the system.\n";
        waitForEnter();
        return;
    }

    // Sap xep theo thoi gian (moi nhat truoc)
    std::sort(allTransactions.begin(), allTransactions.end(),
              [](const Transaction& a, const Transaction& b) {
                  return a.timestamp > b.timestamp;
              });

    std::cout << "Total transactions: " << allTransactions.size() << "\n\n";
    std::cout << "Recent transactions (showing last 20):\n";
    std::cout << std::string(100, '-') << "\n";

    int count = 0;
    double totalAmount = 0.0;

    for (const auto& transaction : allTransactions) {
        if (count >= 20) break; // Chi hien thi 20 giao dich gan nhat

        auto time = std::chrono::system_clock::to_time_t(transaction.timestamp);

        std::cout << "Transaction ID: " << transaction.id << "\n";
        std::cout << "From: " << transaction.fromWalletId << " -> To: " << transaction.toWalletId << "\n";
        std::cout << "Amount: " << std::fixed << std::setprecision(2) << transaction.amount << " points\n";
        std::cout << "Status: " << transaction.status << "\n";
        std::cout << "Description: " << transaction.description << "\n";
        std::cout << "Time: " << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "\n";
        std::cout << std::string(50, '-') << "\n";

        totalAmount += transaction.amount;
        count++;
    }

    // Tinh tong khoi luong giao dich
    double totalVolume = 0.0;
    for (const auto& transaction : allTransactions) {
        totalVolume += transaction.amount;
    }

    std::cout << "\nTransaction Summary:\n";
    std::cout << "Total transactions: " << allTransactions.size() << "\n";
    std::cout << "Total volume: " << std::fixed << std::setprecision(2) << totalVolume << " points\n";
    std::cout << "Average transaction: " << std::fixed << std::setprecision(2)
              << (allTransactions.empty() ? 0.0 : totalVolume / allTransactions.size()) << " points\n";

    waitForEnter();
}

