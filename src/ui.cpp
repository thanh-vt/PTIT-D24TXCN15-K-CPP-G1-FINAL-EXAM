#include "ui.h"
#include "database.hpp"
#include <iostream>
#include <limits>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

UI& UI::getInstance() {
    static UI instance;
    return instance;
}

UI::UI() : is_admin_logged_in_(false) {}

UI::~UI() {}

void UI::start() {
    // Database is already loaded by the Database singleton constructor
    // No need to load it again here

    while (true) {
        clearScreen();
        std::cout << "=== E-Wallet Management System ===\n\n";
        std::cout << "1. User Login\n";
        std::cout << "2. User Register\n";
        std::cout << "3. Admin Login\n";
        std::cout << "4. Exit\n\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                std::cout << "User login not implemented yet.\n";
                waitForEnter();
                break;
            case 2:
                std::cout << "User register not implemented yet.\n";
                waitForEnter();
                break;
            case 3:
                showAdminLoginScreen();
                break;
            case 4:
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

std::string UI::getInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
}

double UI::getAmountInput(const std::string& prompt) {
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

std::chrono::system_clock::time_point UI::getDateInput(const std::string& prompt) {
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

// Helper functions implementation
std::string UI::getPassword(const std::string& prompt) {
    std::cout << prompt;
    std::string password;
    std::getline(std::cin, password);
    return password;
}

bool UI::getConfirmation(const std::string& message) {
    std::string response = getInput(message + " (yes/no): ");
    return (response == "yes" || response == "YES" || response == "y" || response == "Y");
}

int UI::getChoice(const std::string& prompt, int min, int max) {
    int choice;
    while (true) {
        std::cout << prompt;
        if (std::cin >> choice && choice >= min && choice <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return choice;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid choice. Please enter a number between " << min << " and " << max << ".\n";
    }
}

// Display functions implementation
void UI::showHeader(const std::string& title) {
    clearScreen();
    std::cout << "=== " << title << " ===\n\n";
}

void UI::showError(const std::string& message) {
    std::cout << "\n[ERROR] " << message << std::endl;
}

void UI::showSuccess(const std::string& message) {
    std::cout << "\n[SUCCESS] " << message << std::endl;
}

void UI::showWarning(const std::string& message) {
    std::cout << "\n[WARNING] " << message << std::endl;
}

// Admin functions implementation
void UI::showAdminLoginScreen() {
    showHeader("Admin Login");

    std::string username = getInput("Admin Username: ");
    std::string password = getPassword("Admin Password: ");

    Database& db = Database::getInstance();
    if (db.authenticateAdmin(username, password)) {
        current_admin_username_ = username;
        is_admin_logged_in_ = true;
        showSuccess("Admin login successful!");
        waitForEnter();
        showAdminMenu();
    } else {
        showError("Invalid admin credentials!");
        waitForEnter();
    }
}

void UI::showAdminMenu() {
    while (is_admin_logged_in_) {
        showHeader("Admin Menu - Welcome " + current_admin_username_);

        std::cout << "1. View All Users\n";
        std::cout << "2. Create User\n";
        std::cout << "3. Update User\n";
        std::cout << "4. Delete User\n";
        std::cout << "5. View All Wallets\n";
        std::cout << "6. View Wallet Details\n";
        std::cout << "7. View System Statistics\n";
        std::cout << "8. View Transaction Log\n";
        std::cout << "9. Backup Database\n";
        std::cout << "10. Restore Database\n";
        std::cout << "11. Logout\n\n";

        int choice = getChoice("Enter your choice: ", 1, 11);

        switch (choice) {
            case 1: handleAdminViewAllUsers(); break;
            case 2: handleAdminCreateUser(); break;
            case 3: handleAdminUpdateUser(); break;
            case 4: handleAdminDeleteUser(); break;
            case 5: handleAdminViewAllWallets(); break;
            case 6: handleAdminViewWalletDetails(); break;
            case 7: handleAdminViewSystemStats(); break;
            case 8: handleAdminViewTransactionLog(); break;
            case 9: handleAdminBackupDatabase(); break;
            case 10: handleAdminRestoreDatabase(); break;
            case 11:
                is_admin_logged_in_ = false;
                current_admin_username_.clear();
                showSuccess("Admin logged out successfully!");
                waitForEnter();
                return;
        }
    }
}

// Admin user management implementation
void UI::handleAdminViewAllUsers() {
    showHeader("View All Users");
    Database& db = Database::getInstance();
    db.adminViewAllUsers();
    waitForEnter();
}

void UI::handleAdminCreateUser() {
    showHeader("Create New User");

    std::string username = getInput("Enter username: ");
    std::string fullName = getInput("Enter full name: ");
    std::string birthDate = getInput("Enter birth date (YYYY-MM-DD): ");

    Database& db = Database::getInstance();
    std::string generatedPassword;

    if (db.adminCreateUser(username, fullName, birthDate, generatedPassword)) {
        showSuccess("User created successfully!");
        std::cout << "\nGenerated password: " << generatedPassword << std::endl;
        std::cout << "Please provide this password to the user securely." << std::endl;
        std::cout << "User must change this password on first login." << std::endl;
    } else {
        showError("Failed to create user!");
    }
    waitForEnter();
}

void UI::handleAdminUpdateUser() {
    showHeader("Update User Information");

    std::string username = getInput("Enter username to update: ");
    std::string newFullName = getInput("Enter new full name: ");
    std::string newBirthDate = getInput("Enter new birth date (YYYY-MM-DD): ");
    std::string otpCode = getInput("Enter OTP code from user (if 2FA enabled): ");

    Database& db = Database::getInstance();
    if (db.adminUpdateUserInfo(username, newFullName, newBirthDate, otpCode)) {
        showSuccess("User information updated successfully!");
    } else {
        showError("Failed to update user information!");
    }
    waitForEnter();
}

void UI::handleAdminDeleteUser() {
    showHeader("Delete User");

    std::string username = getInput("Enter username to delete: ");

    if (getConfirmation("Are you sure you want to delete user '" + username + "'?")) {
        Database& db = Database::getInstance();
        if (db.adminDeleteUser(username)) {
            showSuccess("User deleted successfully!");
        } else {
            showError("Failed to delete user!");
        }
    } else {
        std::cout << "Delete operation cancelled." << std::endl;
    }
    waitForEnter();
}

// Admin wallet management implementation
void UI::handleAdminViewAllWallets() {
    showHeader("View All Wallets");
    Database& db = Database::getInstance();
    db.adminViewAllWallets();
    waitForEnter();
}

void UI::handleAdminViewWalletDetails() {
    showHeader("View Wallet Details");

    std::string walletId = getInput("Enter Wallet ID: ");

    Database& db = Database::getInstance();
    db.adminViewWalletDetails(walletId);
    waitForEnter();
}

// Admin system management implementation
void UI::handleAdminViewSystemStats() {
    showHeader("System Statistics");
    Database& db = Database::getInstance();
    db.adminViewSystemStats();
    waitForEnter();
}

void UI::handleAdminViewTransactionLog() {
    showHeader("Transaction Log");
    Database& db = Database::getInstance();
    db.adminViewTransactionLog();
    waitForEnter();
}

void UI::handleAdminBackupDatabase() {
    showHeader("Backup Database");

    std::string backupPath = getInput("Enter backup directory (or press Enter for default): ");

    Database& db = Database::getInstance();
    if (db.adminBackupDatabase(backupPath)) {
        showSuccess("Database backup completed successfully!");
    } else {
        showError("Database backup failed!");
    }
    waitForEnter();
}

void UI::handleAdminRestoreDatabase() {
    showHeader("Restore Database");

    std::string backupPath = getInput("Enter backup file path: ");

    showWarning("This will overwrite all current data!");
    if (getConfirmation("Are you sure you want to restore from '" + backupPath + "'?")) {
        Database& db = Database::getInstance();
        if (db.adminRestoreDatabase(backupPath)) {
            showSuccess("Database restore completed successfully!");
        } else {
            showError("Database restore failed!");
        }
    } else {
        std::cout << "Restore operation cancelled." << std::endl;
    }
    waitForEnter();
}