# Hướng dẫn sử dụng Admin Functions cho UI Team

**Phụ trách:** Nguyễn Viết Tùng - K24DTCN633  
**Dành cho:** Vũ Tất Thành (UI Team)

## Tổng quan

File này cung cấp hướng dẫn chi tiết về cách sử dụng các chức năng admin đã được triển khai trong Database class để tích hợp vào UI.

## Cách sử dụng Database trong UI

```cpp
#include "database.hpp"

// Trong UI class, lấy Database instance
Database& db = Database::getInstance();
```

## 1. Admin Authentication (Xác thực Admin)

### 1.1 Đăng nhập Admin
```cpp
bool Database::authenticateAdmin(const std::string& username, const std::string& password) const;

// Sử dụng:
std::string username, password;
std::cout << "Admin Username: ";
std::getline(std::cin, username);
std::cout << "Admin Password: ";
std::getline(std::cin, password);

if (db.authenticateAdmin(username, password)) {
    std::cout << "Login successful! Welcome to Admin Menu." << std::endl;
    // Chuyển đến Admin Menu
} else {
    std::cout << "Login failed! Please try again." << std::endl;
}
```

### 1.2 Kiểm tra quyền Admin
```cpp
bool Database::isAdminUser(const std::string& username) const;

// Sử dụng:
if (db.isAdminUser(currentUsername)) {
    // Hiển thị Admin Menu
} else {
    // Hiển thị User Menu
}
```

## 2. Admin User Management (Quản lý người dùng)

### 2.1 Xem danh sách tất cả người dùng
```cpp
bool Database::adminViewAllUsers() const;

// Menu option: "1. View All Users"
if (choice == 1) {
    db.adminViewAllUsers();
    waitForEnter();
}
```

### 2.2 Tạo người dùng mới
```cpp
bool Database::adminCreateUser(const std::string& username, const std::string& fullName, 
                              const std::string& birthDate, std::string& generatedPassword);

// Menu option: "2. Create User"
if (choice == 2) {
    std::string username, fullName, birthDate, generatedPassword;
    
    std::cout << "Enter username: ";
    std::getline(std::cin, username);
    
    std::cout << "Enter full name: ";
    std::getline(std::cin, fullName);
    
    std::cout << "Enter birth date (YYYY-MM-DD): ";
    std::getline(std::cin, birthDate);
    
    if (db.adminCreateUser(username, fullName, birthDate, generatedPassword)) {
        std::cout << "\nUser created successfully!" << std::endl;
        std::cout << "Generated password: " << generatedPassword << std::endl;
        std::cout << "Please provide this password to the user securely." << std::endl;
        std::cout << "User must change this password on first login." << std::endl;
    } else {
        std::cout << "Failed to create user!" << std::endl;
    }
    waitForEnter();
}
```

### 2.3 Cập nhật thông tin người dùng
```cpp
bool Database::adminUpdateUserInfo(const std::string& username, const std::string& newFullName, 
                                  const std::string& newBirthDate, const std::string& otpCode);

// Menu option: "3. Update User"
if (choice == 3) {
    std::string username, newFullName, newBirthDate, otpCode;
    
    std::cout << "Enter username to update: ";
    std::getline(std::cin, username);
    
    std::cout << "Enter new full name: ";
    std::getline(std::cin, newFullName);
    
    std::cout << "Enter new birth date (YYYY-MM-DD): ";
    std::getline(std::cin, newBirthDate);
    
    std::cout << "Enter OTP code from user (if 2FA enabled): ";
    std::getline(std::cin, otpCode);
    
    if (db.adminUpdateUserInfo(username, newFullName, newBirthDate, otpCode)) {
        std::cout << "User information updated successfully!" << std::endl;
    } else {
        std::cout << "Failed to update user information!" << std::endl;
    }
    waitForEnter();
}
```

### 2.4 Xóa người dùng
```cpp
bool Database::adminDeleteUser(const std::string& username);

// Menu option: "4. Delete User"
if (choice == 4) {
    std::string username, confirmation;
    
    std::cout << "Enter username to delete: ";
    std::getline(std::cin, username);
    
    std::cout << "Are you sure you want to delete user '" << username << "'? (yes/no): ";
    std::getline(std::cin, confirmation);
    
    if (confirmation == "yes" || confirmation == "YES") {
        if (db.adminDeleteUser(username)) {
            std::cout << "User deleted successfully!" << std::endl;
        } else {
            std::cout << "Failed to delete user!" << std::endl;
        }
    } else {
        std::cout << "Delete operation cancelled." << std::endl;
    }
    waitForEnter();
}
```

## 3. Admin Wallet Management (Quản lý ví)

### 3.1 Xem danh sách tất cả ví
```cpp
bool Database::adminViewAllWallets() const;

// Menu option: "5. View All Wallets"
if (choice == 5) {
    db.adminViewAllWallets();
    waitForEnter();
}
```

### 3.2 Xem chi tiết ví
```cpp
bool Database::adminViewWalletDetails(const std::string& walletId) const;

// Menu option: "6. View Wallet Details"
if (choice == 6) {
    std::string walletId;
    std::cout << "Enter Wallet ID: ";
    std::getline(std::cin, walletId);
    
    db.adminViewWalletDetails(walletId);
    waitForEnter();
}
```

## 4. Admin System Management (Quản lý hệ thống)

### 4.1 Xem thống kê hệ thống
```cpp
bool Database::adminViewSystemStats() const;

// Menu option: "7. View System Statistics"
if (choice == 7) {
    db.adminViewSystemStats();
    waitForEnter();
}
```

### 4.2 Xem log giao dịch
```cpp
bool Database::adminViewTransactionLog() const;

// Menu option: "8. View Transaction Log"
if (choice == 8) {
    db.adminViewTransactionLog();
    waitForEnter();
}
```

### 4.3 Sao lưu database
```cpp
bool Database::adminBackupDatabase(const std::string& backupPath = "") const;

// Menu option: "9. Backup Database"
if (choice == 9) {
    std::string backupPath;
    std::cout << "Enter backup directory (or press Enter for default): ";
    std::getline(std::cin, backupPath);
    
    if (db.adminBackupDatabase(backupPath)) {
        std::cout << "Database backup completed successfully!" << std::endl;
    } else {
        std::cout << "Database backup failed!" << std::endl;
    }
    waitForEnter();
}
```

### 4.4 Phục hồi database
```cpp
bool Database::adminRestoreDatabase(const std::string& backupPath);

// Menu option: "10. Restore Database"
if (choice == 10) {
    std::string backupPath, confirmation;
    
    std::cout << "Enter backup file path: ";
    std::getline(std::cin, backupPath);
    
    std::cout << "WARNING: This will overwrite all current data!" << std::endl;
    std::cout << "Are you sure you want to restore from '" << backupPath << "'? (yes/no): ";
    std::getline(std::cin, confirmation);
    
    if (confirmation == "yes" || confirmation == "YES") {
        if (db.adminRestoreDatabase(backupPath)) {
            std::cout << "Database restore completed successfully!" << std::endl;
        } else {
            std::cout << "Database restore failed!" << std::endl;
        }
    } else {
        std::cout << "Restore operation cancelled." << std::endl;
    }
    waitForEnter();
}
```

## 5. Gợi ý Admin Menu Structure

```cpp
void UI::showAdminMenu() {
    while (true) {
        clearScreen();
        std::cout << "=== ADMIN MENU ===" << std::endl;
        std::cout << "1. View All Users" << std::endl;
        std::cout << "2. Create User" << std::endl;
        std::cout << "3. Update User" << std::endl;
        std::cout << "4. Delete User" << std::endl;
        std::cout << "5. View All Wallets" << std::endl;
        std::cout << "6. View Wallet Details" << std::endl;
        std::cout << "7. View System Statistics" << std::endl;
        std::cout << "8. View Transaction Log" << std::endl;
        std::cout << "9. Backup Database" << std::endl;
        std::cout << "10. Restore Database" << std::endl;
        std::cout << "11. Logout" << std::endl;
        std::cout << "Enter your choice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1: /* View All Users */ break;
            case 2: /* Create User */ break;
            // ... implement other cases
            case 11: return; // Logout
            default:
                std::cout << "Invalid choice!" << std::endl;
                waitForEnter();
        }
    }
}
```

## 6. Lưu ý quan trọng

1. **Tài khoản admin mặc định**: username="admin", password="admin123"
2. **Bảo mật**: Luôn yêu cầu xác nhận cho các thao tác nguy hiểm (xóa user, restore database)
3. **OTP**: Một số thao tác yêu cầu OTP từ người dùng nếu họ có bật 2FA
4. **Error handling**: Luôn kiểm tra giá trị trả về của các hàm admin
5. **User experience**: Sử dụng waitForEnter() sau mỗi thao tác để người dùng có thể đọc kết quả

## 7. Test

Để test các chức năng admin, chạy:
```bash
g++ -std=c++17 test_admin_functions.cpp src/database.cpp -I. -o test_admin
./test_admin
```
