#define NOMINMAX
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
#else
#include <cstdlib>
#endif

UI& UI::getInstance() {
    static UI instance;
    return instance;
}

UI::UI() {}

UI::~UI() {}

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
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

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
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

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
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        switch (choice) {
            case 1:
                listUsers();
                break;
            case 2:
                createUser();
                break;
            case 3:
                updateUser(admin);
                break;
            case 4:
                deleteUser();
                break;
            case 5:
                viewAllWallets();
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
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

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

std::shared_ptr<User> UI::login() {
    clearScreen();
    std::cout << "=== Login ===\n\n";
    
    std::string username = getInput("Username: ");
    std::string password = getInput("Password: ");

    auto user = Database::getInstance().getUser(username);
    if (!user) {
        std::cout << "Error: User with username '" << username << "' not found.\n";
        waitForEnter();
        return nullptr;
    }
    if (!user->verifyPassword(password)) {
         std::cout << "Error: Invalid password.\n";
        waitForEnter();
        return nullptr;
    }

    if (user->has2FA() && !verify2FA(user)) {
        std::cout << "Error: 2FA verification failed.\n";
        waitForEnter();
        return nullptr;
    }

    std::cout << "Login successful! Welcome, " << user->getFullname() << "!\n";
    waitForEnter();
    return user;
}

bool UI::authenticateUser(const std::string& username, const std::string& password) {
    auto user = Database::getInstance().getUser(username);
    return user && user->verifyPassword(password);
}

bool UI::verify2FA(std::shared_ptr<User> user) {
    if (!user->has2FA()) return true;
    
    std::string otp = getInput("Enter 2FA code: ");
    return user->verify2FA(otp);
}

void UI::createUser() {
    clearScreen();
    std::cout << "=== Create User ===\n\n";

    std::string username = getInput("Username: ");
    if (Database::getInstance().getUser(username)) {
        std::cout << "Error: Username '" << username << "' already exists.\n";
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
        std::cout << "Error: Failed to create wallet for user.\n";
        waitForEnter();
        return;
    }

    // Set the wallet ID for the user
    newUser.setWalletId(walletId);

    // Then add the user
    if (Database::getInstance().addUser(newUser)) {
        std::cout << "\nSuccess: User created successfully!\n";
        std::cout << "Generated password: " << generatedPassword << "\n";
        std::cout << "Please save this password securely.\n";
        
        // Verify the save was successful
        if (!Database::getInstance().saveToFile()) {
            std::cout << "Warning: Failed to save changes to database.\n";
        }
    } else {
        std::cout << "Error: Failed to create user.\n";
        // Clean up the wallet if user creation failed
        Database::getInstance().deleteWallet(walletId);
    }
    waitForEnter();
}

void UI::deleteUser() {
    clearScreen();
    std::cout << "=== Delete User ===\n\n";

    std::string username = getInput("Enter username to delete: ");
    if (username == "admin") {
        std::cout << "Error: Cannot delete admin user.\n";
        waitForEnter();
        return;
    }

    // Add confirmation step
    std::string confirmation = getInput("Are you sure you want to delete user '" + username + "'? (yes/no): ");
    if (confirmation != "yes") {
        std::cout << "User deletion cancelled.\n";
        waitForEnter();
        return;
    }

    if (Database::getInstance().deleteUser(username)) {
        std::cout << "Success: User '" << username << "' deleted successfully.\n";
    } else {
        // We need a way to know *why* it failed. Assuming it's because the user wasn't found for now.
        std::cout << "Error: Failed to delete user '" << username << "'. User not found or an error occurred.\n";
    }
    waitForEnter();
}

void UI::listUsers() {
    clearScreen();
    std::cout << "=== User List ===\n\n";

    auto users = Database::getInstance().getAllUsers();

    if (users.empty()) {
        std::cout << "No users found.\n";
    } else {
        // Add table headers
        std::cout << std::left << std::setw(20) << "Username"
                  << std::setw(25) << "Full Name"
                  << std::setw(15) << "2FA Enabled"
                  << std::setw(10) << "Is Admin" << "\n";
        std::cout << std::string(80, '-') << "\n"; // Separator line

        for (const auto& user : users) {
            std::cout << std::left << std::setw(20) << user->getUsername()
                      << std::setw(25) << user->getFullname()
                      << std::setw(15) << (user->has2FA() ? "Yes" : "No")
                      << std::setw(10) << (user->isAdmin() ? "Yes" : "No") << "\n";
        }
        std::cout << std::string(80, '-') << "\n"; // Separator line
    }
    waitForEnter();
}

void UI::viewBalance(std::shared_ptr<User> user) {
    clearScreen();
    std::cout << "=== Wallet Balance ===\n\n";

    auto wallet = Database::getInstance().getWallet(user->getWalletId());
    if (wallet) {
        std::cout << "Wallet ID: " << wallet->getId() << "\n";
        std::cout << "Current Balance: " << std::fixed << std::setprecision(2) << wallet->getBalance() << " points\n";
    } else {
        std::cout << "Error: Wallet not found for user '" << user->getUsername() << "'.\n";
    }
    waitForEnter();
}

void UI::transferPoints(std::shared_ptr<User> user) {
    clearScreen();
    std::cout << "=== Transfer Points ===\n\n";

    std::string destinationId = getInput("Enter destination wallet ID: ");
    auto destinationWallet = Database::getInstance().getWallet(destinationId);
    if (!destinationWallet) {
        std::cout << "Error: Destination wallet with ID '" << destinationId << "' not found.\n";
        waitForEnter();
        return;
    }

    double amount = getAmountInput("Enter amount to transfer (positive number): "); // Added prompt clarification
    if (amount <= 0) {
        std::cout << "Error: Transfer amount must be a positive number.\n";
        waitForEnter();
        return;
    }

    std::string description = getInput("Enter description (optional): ");

    auto sourceWallet = Database::getInstance().getWallet(user->getWalletId());
    if (!sourceWallet) {
        std::cout << "Error: Source wallet not found for user '" << user->getUsername() << "'.\n";
        waitForEnter();
        return;
    }

    // Assuming transfer function returns false for insufficient balance or other errors
    if (sourceWallet->transfer(*destinationWallet, amount, description)) {
        std::cout << "Success: Transfer of " << std::fixed << std::setprecision(2) << amount << " points to wallet '" << destinationId << "' successful.\n";
    } else {
        std::cout << "Error: Transfer failed. Insufficient balance or an error occurred.\n";
    }
    waitForEnter();
}

void UI::viewTransactionHistory(std::shared_ptr<User> user) {
    if (!user) {
        std::cout << "Error: No user selected.\n";
        return;
    }

    clearScreen();
    std::cout << "=== Transaction History for " << user->getUsername() << " ===\n\n";
    auto wallet = Database::getInstance().getWallet(user->getWalletId());
    if (wallet) {
        std::cout << "Wallet ID: " << wallet->getId() << "\n";
        if (wallet->getTransactionHistory().empty()) {
            std::cout << "No transaction history found for this wallet.\n";
        } else {
            // Add table headers
            std::cout << std::left << std::setw(18) << "ID Giao Dịch"
                      << std::setw(22) << "Thời gian"
                      << std::setw(15) << "Từ Ví"
                      << std::setw(15) << "Đến Ví"
                      << std::right << std::setw(12) << "Số điểm"
                      << std::left << "  " << std::setw(12) << "Trạng thái"
                      << "  Mô tả\n";
            std::cout << std::string(110, '-') << "\n"; // Separator line (increased width)

            for (const auto& transaction : wallet->getTransactionHistory()) {
                auto time_c = std::chrono::system_clock::to_time_t(transaction.timestamp);
                std::cout << std::left << std::setw(18) << transaction.id
                          << std::setw(22) << std::put_time(std::localtime(&time_c), "%Y-%m-%d %H:%M:%S")
                          << std::setw(15) << transaction.fromWalletId
                          << std::setw(15) << transaction.toWalletId
                          << std::right << std::fixed << std::setprecision(2) << std::setw(12) << transaction.amount
                          << std::left << "  " << std::setw(12) << transaction.status
                          << "  " << transaction.description << "\n";
            }
            std::cout << std::string(110, '-') << "\n"; // Separator line (increased width)
        }
    } else {
        std::cout << "Error: No wallet found for this user.\n";
    }
    waitForEnter();
}

void UI::viewProfile(std::shared_ptr<User> user) {
    clearScreen();
    std::cout << "=== User Profile ===\n\n";
    if (user) {
        std::cout << "Username: " << user->getUsername() << "\n";
        std::cout << "Full Name: " << user->getFullname() << "\n";
        // Assuming User class has getDob() method
        // std::cout << "Date of Birth: " << user->getDob() << "\n"; 
        std::cout << "2FA Enabled: " << (user->has2FA() ? "Yes" : "No") << "\n";
        std::cout << "Is Admin: " << (user->isAdmin() ? "Yes" : "No") << "\n";
        // Add other profile details here
    } else {
        std::cout << "User data not available.\n";
    }
    waitForEnter();
}

void UI::changePassword(std::shared_ptr<User> user) {
    clearScreen();
    std::cout << "=== Change Password ===\n\n";
    // Implementation for changing password
    std::cout << "Change password functionality is not yet fully implemented.\n";
    waitForEnter();
}

void UI::toggle2FA(std::shared_ptr<User> user) {
    clearScreen();
    std::cout << "=== Enable/Disable 2FA ===\n\n";
    // Implementation for toggling 2FA
    std::cout << "Toggle 2FA functionality is not yet fully implemented.\n";
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
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
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
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            return amount;
        }
        std::cin.clear();
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
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

void UI::registerUser() {
    clearScreen();
    std::cout << "=== User Registration ===\n\n";

    // Get username
    std::string username = getInput("Username: ");
    if (Database::getInstance().getUser(username)) {
        std::cout << "Username already exists.\n";
        waitForEnter();
        return;
    }

    // Get full name
    std::string fullname = getInput("Full Name: ");

    // Get date of birth
    auto dob = getDateInput("Date of Birth (YYYY-MM-DD): ");

    // Get password
    std::string password;
    while (true) {
        password = getInput("Password: ");
        if (password.length() < 8) {
            std::cout << "Password must be at least 8 characters long.\n";
            continue;
        }
        std::string confirmPassword = getInput("Confirm Password: ");
        if (password != confirmPassword) {
            std::cout << "Passwords do not match.\n";
            continue;
        }
        break;
    }

    // Create new user
    User newUser(username, fullname, dob);
    newUser.setPassword(password);

    // Create wallet for the user
    std::string walletId = "W" + username;  // Simple wallet ID generation
    Wallet wallet(walletId);  // Create a Wallet object directly
    if (Database::getInstance().addWallet(wallet)) {  // Pass the Wallet object by reference
        newUser.setWalletId(walletId);
        
        // Add user to database
        if (Database::getInstance().addUser(newUser)) {
            std::cout << "\nRegistration successful!\n";
            std::cout << "You can now login with your username and password.\n";
        } else {
            std::cout << "\nUser registration failed. Please try again.\n";
        }
    } else {
        std::cout << "\nWallet creation failed. Please try again.\n";
    }
    waitForEnter();
}

void UI::updateUser(std::shared_ptr<User> admin) {
    clearScreen();
    std::cout << "=== Update User ===\n\n";
    // Implementation for updating user
    std::cout << "Update user functionality is not yet fully implemented.\n";
    waitForEnter();
}

void UI::viewAllWallets() {
    clearScreen();
    std::cout << "=== All Wallets ===\n\n";
    // Implementation for viewing all wallets
    std::cout << "View all wallets functionality is not yet fully implemented.\n";
    waitForEnter();
} 