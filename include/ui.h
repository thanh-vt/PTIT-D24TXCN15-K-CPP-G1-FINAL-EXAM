#pragma once

#include <chrono>
#include <memory>
#include <string>

class UI {
public:
    static UI& getInstance();

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
    std::string getPassword(const std::string& prompt); // Lay password (an)
    bool getConfirmation(const std::string& message); // Lay xac nhan yes/no
    int getChoice(const std::string& prompt, int min, int max); // Lay lua chon
    double getAmountInput(const std::string& prompt);
    std::chrono::system_clock::time_point getDateInput(const std::string& prompt);

    // Display functions
    void showHeader(const std::string& title); // Hien thi header
    void showError(const std::string& message); // Hien thi loi
    void showSuccess(const std::string& message); // Hien thi thanh cong
    void showWarning(const std::string& message); // Hien thi canh bao

    // State variables
    std::string current_admin_username_; // Ten admin hien tai
    bool is_admin_logged_in_; // Trang thai dang nhap admin

};