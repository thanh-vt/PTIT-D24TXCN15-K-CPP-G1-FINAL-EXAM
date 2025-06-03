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

UI &UI::getInstance()
{
    static UI instance;
    return instance;
}

UI::UI() : is_admin_logged_in_(false) {}

UI::~UI() {}

void UI::start()
{
    // Load database and create default admin
    Database &db = Database::getInstance();
    db.loadFromFile();
    db.createDefaultAdminUser();

    while (true)
    {
        clearScreen();
        std::cout << "=== E-Wallet Management System ===\n\n";
        std::cout << "1. Login\n";
        std::cout << "2. Register\n";
        std::cout << "3. Exit\n\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice)
        {
        case 1:
            handleLogin();
            break;
        case 2:
            handleRegister();
            break;
        case 3:
            return;
        default:
            std::cout << "Invalid choice. Please try again.\n";
            waitForEnter();
        }
    }
}

void UI::handleLogin()
{
    showHeader("Login");

    std::string username = getInput("Username: ");
    std::string password = getPassword("Password: ");

    Database &db = Database::getInstance();

    // Check if admin login
    if (username == "admin" && password == "admin")
    {
        if (db.authenticateAdmin(username, password))
        {
            current_admin_username_ = username;
            is_admin_logged_in_ = true;
            showSuccess("Admin login successful!");
            waitForEnter();
            showAdminMenu();
        }
        else
        {
            showError("Admin authentication failed!");
            waitForEnter();
        }
    }
    else
    {
        showError("User login not implemented yet. Use admin/admin for admin access.");
        waitForEnter();
    }
}

void UI::handleRegister()
{
    showError("User registration not implemented yet.");
    waitForEnter();
}

void UI::showMainMenu()
{
    clearScreen();
    std::cout << "=== Main Menu ===\n\n";
    std::cout << "1. User Management\n";
    std::cout << "2. Wallet Management\n";
    std::cout << "3. Logout\n\n";
    std::cout << "Enter your choice: ";
}

void UI::clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void UI::waitForEnter()
{
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string UI::getInput(const std::string &prompt)
{
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
}

double UI::getAmountInput(const std::string &prompt)
{
    double amount;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> amount && amount > 0)
        {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return amount;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid amount. Please try again.\n";
    }
}

std::chrono::system_clock::time_point UI::getDateInput(const std::string &prompt)
{
    while (true)
    {
        std::string dateStr = getInput(prompt);
        std::tm tm = {};
        std::stringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%d");

        if (!ss.fail())
        {
            return std::chrono::system_clock::from_time_t(std::mktime(&tm));
        }
        std::cout << "Invalid date format. Please use YYYY-MM-DD.\n";
    }
}

// Helper functions implementation
std::string UI::getPassword(const std::string &prompt)
{
    std::cout << prompt;
    std::string password;
    std::getline(std::cin, password);
    return password;
}

bool UI::getConfirmation(const std::string &message)
{
    std::string response = getInput(message + " (yes/no): ");
    return (response == "yes" || response == "YES" || response == "y" || response == "Y");
}

int UI::getChoice(const std::string &prompt, int min, int max)
{
    int choice;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> choice && choice >= min && choice <= max)
        {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return choice;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid choice. Please enter a number between " << min << " and " << max << ".\n";
    }
}

// Display functions implementation
void UI::showHeader(const std::string &title)
{
    clearScreen();
    std::cout << "=== " << title << " ===\n\n";
}

void UI::showError(const std::string &message)
{
    std::cout << "\n[ERROR] " << message << std::endl;
}

void UI::showSuccess(const std::string &message)
{
    std::cout << "\n[SUCCESS] " << message << std::endl;
}

void UI::showWarning(const std::string &message)
{
    std::cout << "\n[WARNING] " << message << std::endl;
}

// Admin functions implementation
void UI::showAdminLoginScreen()
{
    showHeader("Admin Login");

    std::string username = getInput("Admin Username: ");
    std::string password = getPassword("Admin Password: ");

    Database &db = Database::getInstance();
    if (db.authenticateAdmin(username, password))
    {
        current_admin_username_ = username;
        is_admin_logged_in_ = true;
        showSuccess("Admin login successful!");
        waitForEnter();
        showAdminMenu();
    }
    else
    {
        showError("Invalid admin credentials!");
        waitForEnter();
    }
}

void UI::showAdminMenu()
{
    while (is_admin_logged_in_)
    {
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

        switch (choice)
        {
        case 1:
            handleAdminViewAllUsers();
            break;
        case 2:
            handleAdminCreateUser();
            break;
        case 3:
            handleAdminUpdateUser();
            break;
        case 4:
            handleAdminDeleteUser();
            break;
        case 5:
            handleAdminViewAllWallets();
            break;
        case 6:
            handleAdminViewWalletDetails();
            break;
        case 7:
            handleAdminViewSystemStats();
            break;
        case 8:
            handleAdminViewTransactionLog();
            break;
        case 9:
            handleAdminBackupDatabase();
            break;
        case 10:
            handleAdminRestoreDatabase();
            break;
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
void UI::handleAdminViewAllUsers()
{
    showHeader("View All Users");
    Database &db = Database::getInstance();
    db.adminViewAllUsers();
    waitForEnter();
}

void UI::handleAdminCreateUser()
{
    showHeader("Create New User");

    std::string username = getInput("Enter username: ");
    std::string fullName = getInput("Enter full name: ");
    std::string birthDate = getInput("Enter birth date (YYYY-MM-DD): ");

    Database &db = Database::getInstance();
    std::string generatedPassword;

    if (db.adminCreateUser(username, fullName, birthDate, generatedPassword))
    {
        showSuccess("User created successfully!");
        std::cout << "\nGenerated password: " << generatedPassword << std::endl;
        std::cout << "Please provide this password to the user securely." << std::endl;
        std::cout << "User must change this password on first login." << std::endl;
    }
    else
    {
        showError("Failed to create user!");
    }
    waitForEnter();
}

void UI::handleAdminUpdateUser()
{
    showHeader("Update User Information");

    std::string username = getInput("Enter username to update: ");
    std::string newFullName = getInput("Enter new full name: ");
    std::string newBirthDate = getInput("Enter new birth date (YYYY-MM-DD): ");
    std::string otpCode = getInput("Enter OTP code from user (if 2FA enabled): ");

    Database &db = Database::getInstance();
    if (db.adminUpdateUserInfo(username, newFullName, newBirthDate, otpCode))
    {
        showSuccess("User information updated successfully!");
    }
    else
    {
        showError("Failed to update user information!");
    }
    waitForEnter();
}

void UI::handleAdminDeleteUser()
{
    showHeader("Delete User");

    std::string username = getInput("Enter username to delete: ");

    if (getConfirmation("Are you sure you want to delete user '" + username + "'?"))
    {
        Database &db = Database::getInstance();
        if (db.adminDeleteUser(username))
        {
            showSuccess("User deleted successfully!");
        }
        else
        {
            showError("Failed to delete user!");
        }
    }
    else
    {
        std::cout << "Delete operation cancelled." << std::endl;
    }
    waitForEnter();
}

// Admin wallet management implementation
void UI::handleAdminViewAllWallets()
{
    showHeader("View All Wallets");
    Database &db = Database::getInstance();
    db.adminViewAllWallets();
    waitForEnter();
}

void UI::handleAdminViewWalletDetails()
{
    showHeader("View Wallet Details");

    std::string walletId = getInput("Enter Wallet ID: ");

    Database &db = Database::getInstance();
    db.adminViewWalletDetails(walletId);
    waitForEnter();
}

// Admin system management implementation
void UI::handleAdminViewSystemStats()
{
    showHeader("System Statistics");
    Database &db = Database::getInstance();
    db.adminViewSystemStats();
    waitForEnter();
}

void UI::handleAdminViewTransactionLog()
{
    showHeader("Transaction Log");
    Database &db = Database::getInstance();
    db.adminViewTransactionLog();
    waitForEnter();
}

void UI::handleAdminBackupDatabase()
{
    showHeader("Backup Database");

    std::string backupPath = getInput("Enter backup directory (or press Enter for default): ");

    Database &db = Database::getInstance();
    if (db.adminBackupDatabase(backupPath))
    {
        showSuccess("Database backup completed successfully!");
    }
    else
    {
        showError("Database backup failed!");
    }
    waitForEnter();
}

void UI::handleAdminRestoreDatabase()
{
    showHeader("Restore Database");

    std::string backupPath = getInput("Enter backup file path: ");

    showWarning("This will overwrite all current data!");
    if (getConfirmation("Are you sure you want to restore from '" + backupPath + "'?"))
    {
        Database &db = Database::getInstance();
        if (db.adminRestoreDatabase(backupPath))
        {
            showSuccess("Database restore completed successfully!");
        }
        else
        {
            showError("Database restore failed!");
        }
    }
    else
    {
        std::cout << "Restore operation cancelled." << std::endl;
    }
    waitForEnter();
}

void UI::showUserMenu(std::shared_ptr<User> user)
{
    while (true)
    {
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

        switch (choice)
        {
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

void UI::showWalletMenu(std::shared_ptr<User> user)
{
    while (true)
    {
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

        switch (choice)
        {
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

std::shared_ptr<User> UI::login()
{
    clearScreen();
    std::cout << "=== Login ===\n\n";

    std::string username = getInput("Username: ");
    std::string password = getPassword("Password: ");

    auto user = Database::getInstance().getUser(username);
    if (!user || !user->verifyPassword(password))
    {
        std::cout << "Invalid username or password.\n";
        waitForEnter();
        return nullptr;
    }

    if (user->has2FA() && !verify2FA(user))
    {
        std::cout << "2FA verification failed.\n";
        waitForEnter();
        return nullptr;
    }

    return user;
}

bool UI::authenticateUser(const std::string &username, const std::string &password)
{
    auto user = Database::getInstance().getUser(username);
    return user && user->verifyPassword(password);
}

bool UI::verify2FA(std::shared_ptr<User> user)
{
    if (!user->has2FA())
        return true;

    std::string otp = getInput("Enter 2FA code: ");
    return user->verify2FA(otp);
}

void UI::createUser()
{
    clearScreen();
    std::cout << "=== Create User ===\n\n";

    std::string username = getInput("Username: ");
    if (Database::getInstance().getUser(username))
    {
        std::cout << "Username already exists.\n";
        waitForEnter();
        return;
    }

    std::string fullname = getInput("Full Name: ");
    auto dob = getDateInput("Date of Birth (YYYY-MM-DD): ");

    // Create new user with a generated password
    User newUser(username, fullname, dob);
    std::string generatedPassword = newUser.generatePassword(); // Generate a new password
    newUser.setPassword(generatedPassword);                     // Set the generated password

    // Create wallet for the user
    std::string walletId = "W" + username; // Simple wallet ID generation
    Wallet wallet(walletId, 0.0);          // Create wallet with 0 balance

    // First add the wallet
    if (!Database::getInstance().addWallet(wallet))
    {
        std::cout << "Failed to create wallet for user.\n";
        waitForEnter();
        return;
    }

    // Set the wallet ID for the user
    newUser.setWalletId(walletId);

    // Then add the user
    if (Database::getInstance().addUser(newUser))
    {
        std::cout << "\nUser created successfully!\n";
        std::cout << "Generated password: " << generatedPassword << "\n";
        std::cout << "Please save this password securely.\n";

        // Verify the save was successful
        if (!Database::getInstance().saveToFile())
        {
            std::cout << "Warning: Failed to save changes to database.\n";
        }
    }
    else
    {
        std::cout << "Failed to create user.\n";
        // Clean up the wallet if user creation failed
        Database::getInstance().deleteWallet(walletId);
    }
    waitForEnter();
}

void UI::deleteUser()
{
    clearScreen();
    std::cout << "=== Delete User ===\n\n";

    std::string username = getInput("Enter username to delete: ");
    if (username == "admin")
    {
        std::cout << "Cannot delete admin user.\n";
        waitForEnter();
        return;
    }

    if (Database::getInstance().deleteUser(username))
    {
        std::cout << "User deleted successfully.\n";
    }
    else
    {
        std::cout << "Failed to delete user.\n";
    }
    waitForEnter();
}

void UI::listUsers()
{
    clearScreen();
    std::cout << "=== User List ===\n\n";

    auto users = Database::getInstance().getAllUsers();
    for (const auto &user : users)
    {
        std::cout << "Username: " << user->getUsername() << "\n";
        std::cout << "Full Name: " << user->getFullname() << "\n";
        std::cout << "2FA Enabled: " << (user->has2FA() ? "Yes" : "No") << "\n";
        std::cout << "Is Admin: " << (user->isAdmin() ? "Yes" : "No") << "\n";
        std::cout << "------------------------\n";
    }
    waitForEnter();
}

void UI::viewBalance(std::shared_ptr<User> user)
{
    clearScreen();
    std::cout << "=== Wallet Balance ===\n\n";

    auto wallet = Database::getInstance().getWallet(user->getWalletId());
    if (wallet)
    {
        std::cout << "Wallet ID: " << wallet->getId() << "\n";
        std::cout << "Balance: " << std::fixed << std::setprecision(2) << wallet->getBalance() << " points\n";
    }
    else
    {
        std::cout << "Wallet not found.\n";
    }
    waitForEnter();
}

void UI::transferPoints(std::shared_ptr<User> user)
{
    clearScreen();
    std::cout << "=== Transfer Points ===\n\n";

    std::string destinationId = getInput("Enter destination wallet ID: ");
    auto destinationWallet = Database::getInstance().getWallet(destinationId);
    if (!destinationWallet)
    {
        std::cout << "Destination wallet not found.\n";
        waitForEnter();
        return;
    }

    double amount = getAmountInput("Enter amount to transfer: ");
    std::string description = getInput("Enter description (optional): ");

    auto sourceWallet = Database::getInstance().getWallet(user->getWalletId());
    if (!sourceWallet)
    {
        std::cout << "Source wallet not found.\n";
        waitForEnter();
        return;
    }

    if (sourceWallet->transfer(*destinationWallet, amount, description))
    {
        std::cout << "Transfer successful.\n";
    }
    else
    {
        std::cout << "Transfer failed.\n";
    }
    waitForEnter();
}

void UI::viewTransactionHistory(std::shared_ptr<User> user)
{
    if (!user)
    {
        std::cout << "No user selected.\n";
        return;
    }

    std::cout << "\nTransaction History for " << user->getUsername() << ":\n";
    auto wallet = Database::getInstance().getWallet(user->getWalletId());
    if (wallet)
    {
        std::cout << "\nWallet ID: " << wallet->getId() << "\n";
        for (const auto &transaction : wallet->getTransactionHistory())
        {
            std::cout << "Transaction ID: " << transaction.id << "\n";
            std::cout << "From: " << transaction.fromWalletId << "\n";
            std::cout << "To: " << transaction.toWalletId << "\n";
            std::cout << "Amount: " << transaction.amount << "\n";
            std::cout << "Status: " << transaction.status << "\n";
            std::cout << "Description: " << transaction.description << "\n";

            // Fix timestamp conversion
            auto time = std::chrono::system_clock::to_time_t(transaction.timestamp);
            std::cout << "Timestamp: " << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "\n";
            std::cout << "------------------------\n";
        }
    }
    else
    {
        std::cout << "No wallet found for this user.\n";
    }
    waitForEnter();
}

void UI::registerUser()
{
    clearScreen();
    std::cout << "=== User Registration ===\n\n";

    // Get username
    std::string username = getInput("Username: ");
    if (Database::getInstance().getUser(username))
    {
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
    while (true)
    {
        password = getPassword("Password: ");
        if (password.length() < 8)
        {
            std::cout << "Password must be at least 8 characters long.\n";
            continue;
        }
        std::string confirmPassword = getPassword("Confirm Password: ");
        if (password != confirmPassword)
        {
            std::cout << "Passwords do not match.\n";
            continue;
        }
        break;
    }

    // Create new user
    User newUser(username, fullname, dob);
    newUser.setPassword(password);

    // Create wallet for the user
    std::string walletId = "W" + username; // Simple wallet ID generation
    Wallet wallet(walletId);               // Create a Wallet object directly
    if (Database::getInstance().addWallet(wallet))
    { // Pass the Wallet object by reference
        newUser.setWalletId(walletId);

        // Add user to database
        if (Database::getInstance().addUser(newUser))
        {
            std::cout << "\nRegistration successful!\n";
            std::cout << "You can now login with your username and password.\n";
        }
        else
        {
            std::cout << "\nUser registration failed. Please try again.\n";
        }
    }
    else
    {
        std::cout << "\nWallet creation failed. Please try again.\n";
    }
    waitForEnter();
}

void UI::toggle2FA(std::shared_ptr<User> user)
{
    clearScreen();
    std::cout << "=== 2FA Settings ===\n\n";

    if (user->has2FA())
    {
        std::cout << "2FA is currently enabled.\n";
        std::string otp = getInput("Enter current 2FA code to disable: ");
        if (user->verify2FA(otp))
        {
            user->disable2FA();
            // Save changes to database
            if (Database::getInstance().updateUser(*user))
            {
                std::cout << "2FA has been disabled successfully.\n";
            }
            else
            {
                std::cout << "Failed to save changes to database.\n";
            }
        }
        else
        {
            std::cout << "Invalid 2FA code. 2FA remains enabled.\n";
        }
    }
    else
    {
        std::cout << "2FA is currently disabled.\n";
        std::cout << "Enabling 2FA will require you to enter a code from your authenticator app each time you log in.\n";
        std::string choice = getInput("Do you want to enable 2FA? (yes/no): ");

        if (choice == "yes" || choice == "y" || choice == "Y" || choice == "Yes")
        {
            std::string secretKey = user->enable2FA();
            // Save changes to database
            if (Database::getInstance().updateUser(*user))
            {
                std::cout << "\n2FA has been enabled!\n";
                std::cout << "Please scan this QR code with your authenticator app:\n";
                std::cout << "Secret Key: " << secretKey << "\n";
                std::cout << "You can also manually enter this key in your authenticator app.\n";
            }
            else
            {
                std::cout << "Failed to save changes to database.\n";
                // Revert the changes if save failed
                user->disable2FA();
            }
        }
        else
        {
            std::cout << "2FA remains disabled.\n";
        }
    }
    waitForEnter();
}

void UI::changePassword(std::shared_ptr<User> user)
{
    clearScreen();
    std::cout << "=== Change Password ===\n\n";

    // Get current password
    std::string currentPassword = getPassword("Enter current password: ");
    if (!user->verifyPassword(currentPassword))
    {
        std::cout << "Current password is incorrect.\n";
        waitForEnter();
        return;
    }

    // Get new password
    std::string newPassword;
    while (true)
    {
        newPassword = getPassword("Enter new password: ");
        if (newPassword.length() < 8)
        {
            std::cout << "Password must be at least 8 characters long.\n";
            continue;
        }
        std::string confirmPassword = getPassword("Confirm new password: ");
        if (newPassword != confirmPassword)
        {
            std::cout << "Passwords do not match.\n";
            continue;
        }
        break;
    }

    // Update password
    user->setPassword(newPassword);

    // Save changes to database
    if (Database::getInstance().updateUser(*user))
    {
        std::cout << "Password changed successfully.\n";
    }
    else
    {
        std::cout << "Failed to save changes to database.\n";
    }
    waitForEnter();
}

void UI::viewProfile(std::shared_ptr<User> user)
{
    clearScreen();
    std::cout << "=== User Profile ===\n\n";

    // Display user information
    std::cout << "Username: " << user->getUsername() << "\n";
    std::cout << "Full Name: " << user->getFullname() << "\n";

    // Format and display date of birth
    auto dob = user->getDateOfBirth();
    auto time = std::chrono::system_clock::to_time_t(dob);
    std::cout << "Date of Birth: " << std::put_time(std::localtime(&time), "%Y-%m-%d") << "\n";

    // Display 2FA status
    std::cout << "2FA Status: " << (user->has2FA() ? "Enabled" : "Disabled") << "\n";

    // Display admin status
    std::cout << "Account Type: " << (user->isAdmin() ? "Administrator" : "Regular User") << "\n";

    // Display wallet information
    auto wallet = Database::getInstance().getWallet(user->getWalletId());
    if (wallet)
    {
        std::cout << "\nWallet Information:\n";
        std::cout << "Wallet ID: " << wallet->getId() << "\n";
        std::cout << "Balance: " << std::fixed << std::setprecision(2) << wallet->getBalance() << " points\n";
    }
    else
    {
        std::cout << "\nNo wallet associated with this account.\n";
    }

    waitForEnter();
}