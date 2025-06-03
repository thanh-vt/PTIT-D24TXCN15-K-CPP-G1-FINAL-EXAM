#pragma once

#include "user.hpp"
#include "wallet.hpp"
#include <memory>

// Lop quan ly chuc nang admin
class Admin {
public:
    // Lay instance duy nhat
    static Admin& getInstance();

    // Kiem tra quyen admin
    static bool isAuthorized(std::shared_ptr<User> user);

    // Hien thi menu admin
    void showAdminMenu(std::shared_ptr<User> admin);

    // Quan ly nguoi dung (theo Project.md)
    void adminViewAllUsers();        // b.1 - Theo doi danh sach nhom
    void adminCreateUser();          // b.2 - Tao them tai khoan moi
    void adminUpdateUser();          // b.3 - Dieu chinh thong tin tai khoan

    // Quan ly vi (theo Project.md)
    void adminViewAllWallets();      // Bao cao so du
    void adminViewAllTransactions(); // Lich su giao dich

private:
    Admin();
    ~Admin();

    // Ngan chan copy
    Admin(const Admin&) = delete;
    Admin& operator=(const Admin&) = delete;

    // Ham ho tro
    void clearScreen();
    void waitForEnter();
    std::string getInput(const std::string& prompt);
    std::string getPasswordInput(const std::string& prompt);
    double getAmountInput(const std::string& prompt);
    std::chrono::system_clock::time_point getDateInput(const std::string& prompt);
};
