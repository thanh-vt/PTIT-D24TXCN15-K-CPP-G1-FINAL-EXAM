#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <memory>
#include "user.hpp"
#include "wallet.hpp"

class UI {
public:
    static UI &getInstance();

    void start();

    void showMainMenu();

    // Login and authentication
    void handleLogin();

    void handleRegister();

    // Admin functions
    void showAdminLoginScreen(); // Hien thi man hinh dang nhap admin
    void showAdminMenu(); // Hien thi menu admin chinh

    // Admin user management
    void handleAdminViewAllUsers(); // Xu ly xem tat ca user
    void handleAdminCreateUser(); // Xu ly tao user moi
    void handleAdminUpdateUser(); // Xu ly cap nhat user
    void handleAdminDeleteUser(); // Xu ly xoa user

    // Admin wallet management
    void handleAdminViewAllWallets(); // Xu ly xem tat ca vi
    void handleAdminViewWalletDetails(); // Xu ly xem chi tiet vi

    // Admin system management
    void handleAdminViewSystemStats(); // Xu ly xem thong ke he thong
    void handleAdminViewTransactionLog(); // Xu ly xem log giao dich
    void handleAdminBackupDatabase(); // Xu ly sao luu database
    void handleAdminRestoreDatabase(); // Xu ly phuc hoi database

    void showUserMenu(std::shared_ptr<User> user);

    void showAdminMenu(std::shared_ptr<User> admin);

    void showWalletMenu(std::shared_ptr<User> user);

    void toggle2FA(std::shared_ptr<User> user);

private:
    UI();

    ~UI();

    // Prevent copying
    UI(const UI &) = delete;

    UI &operator=(const UI &) = delete;

    // Authentication
    std::shared_ptr<User> login();

    bool authenticateUser(const std::string &username, const std::string &password);

    bool verify2FA(std::shared_ptr<User> user);

    void registerUser();

    // User management
    void createUser();

    void updateUser(std::shared_ptr<User> user);

    void deleteUser();

    void listUsers();

    void changePassword(std::shared_ptr<User> user);

    void viewProfile(std::shared_ptr<User> user);

    // Wallet management
    void createWallet(std::shared_ptr<User> user);

    void transferPoints(std::shared_ptr<User> user);

    void viewBalance(std::shared_ptr<User> user);

    void viewTransactionHistory(std::shared_ptr<User> user);

    // Helper functions
    void clearScreen();

    void waitForEnter();

    std::string getInput(const std::string &prompt);

    std::string getPasswordInput(const std::string &prompt);

    double getAmountInput(const std::string &prompt);

    std::chrono::system_clock::time_point getDateInput(const std::string &prompt);

    bool getConfirmation(const std::string &message); // Lay xac nhan yes/no
    int getChoice(const std::string &prompt, int min, int max); // Lay lua chon

    // Display functions
    void showHeader(const std::string &title); // Hien thi header
    void showError(const std::string &message); // Hien thi loi
    void showSuccess(const std::string &message); // Hien thi thanh cong
    void showWarning(const std::string &message); // Hien thi canh bao

    // State variables
    std::string current_admin_username_; // Ten admin hien tai
    bool is_admin_logged_in_; // Trang thai dang nhap admin
};
