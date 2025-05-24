#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <cstdint>

// Struct don gian cho admin data
struct AdminUser {
    std::string username;
    std::string password_hash;
    std::string full_name;
    std::string birth_date;
    bool is_admin;
    bool is_2fa_enabled;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point last_login;

    AdminUser(const std::string& user, const std::string& pass, const std::string& name,
              const std::string& birth, bool admin = false)
        : username(user), password_hash(pass), full_name(name), birth_date(birth),
          is_admin(admin), is_2fa_enabled(false),
          created_at(std::chrono::system_clock::now()),
          last_login(std::chrono::system_clock::now()) {}
};

struct AdminWallet {
    std::string wallet_id;
    std::string owner_username;
    double balance;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point last_transaction_time;

    AdminWallet(const std::string& id, const std::string& owner, double bal = 0.0)
        : wallet_id(id), owner_username(owner), balance(bal),
          created_at(std::chrono::system_clock::now()),
          last_transaction_time(std::chrono::system_clock::now()) {}
};

struct AdminTransaction {
    std::string transaction_id;
    std::string from_wallet;
    std::string to_wallet;
    double amount;
    std::string description;
    std::string status;
    std::chrono::system_clock::time_point created_at;

    AdminTransaction(const std::string& id, const std::string& from, const std::string& to,
                    double amt, const std::string& desc, const std::string& stat = "COMPLETED")
        : transaction_id(id), from_wallet(from), to_wallet(to), amount(amt),
          description(desc), status(stat), created_at(std::chrono::system_clock::now()) {}
};

class Database {
private:
    static Database* instance_;

    // Constructor va destructor private cho Singleton
    Database();
    ~Database();

    // Cac container luu tru du lieu admin
    std::unordered_map<std::string, std::shared_ptr<AdminUser>> users_;
    std::unordered_map<std::string, std::shared_ptr<AdminWallet>> wallets_;
    std::vector<AdminTransaction> central_transactions_log_;

    // Quan ly file
    const std::string DB_FILE_PATH_ = "database.dat";
    std::filesystem::file_time_type last_file_modification_time_;

    // Hang so dinh dang file
    static constexpr uint32_t MAGIC_NUMBER = 0x4557414C; // "EWAL"
    static constexpr uint32_t VERSION = 1;

    // Cac ham ho tro thao tac file
    bool isFileModified() const; // Kiem tra file co bi sua doi
    uint32_t calculateCRC32(const char* data, size_t length) const; // Tinh checksum

    // Cac ham serialize admin data
    void serializeAdminUser(std::ostream& file, const AdminUser& user) const;
    void serializeAdminWallet(std::ostream& file, const AdminWallet& wallet) const;
    void serializeAdminTransaction(std::ostream& file, const AdminTransaction& transaction) const;

    // Cac ham deserialize admin data
    AdminUser deserializeAdminUser(std::istream& file) const;
    AdminWallet deserializeAdminWallet(std::istream& file) const;
    AdminTransaction deserializeAdminTransaction(std::istream& file) const;

    // Ham ho tro serialize chuoi
    void writeString(std::ofstream& file, const std::string& str) const; // Ghi chuoi
    std::string readString(std::ifstream& file) const; // Doc chuoi

    // Ham ho tro serialize thoi gian
    void writeTimePoint(std::ofstream& file, const std::chrono::system_clock::time_point& timePoint) const; // Ghi thoi gian
    std::chrono::system_clock::time_point readTimePoint(std::ifstream& file) const; // Doc thoi gian

public:
    // Truy cap Singleton
    static Database& getInstance(); // Lay instance duy nhat

    // Ngan chan copy va assignment
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    // === ADMIN FUNCTIONS ONLY ===

    // Cac thao tac file database
    bool saveToFile(); // Luu database ra file
    bool loadFromFile(); // Tai database tu file

    // Cac ham sao luu va phuc hoi
    bool backupDatabase(const std::string& backupDirectory) const; // Sao luu database
    bool restoreDatabase(const std::string& backupFilePath); // Phuc hoi database

    // Cac ham tien ich
    bool isDatabaseEmpty() const; // Kiem tra database rong
    void clearAllData(); // Xoa tat ca du lieu

    // Cac ham ho tro admin
    bool createDefaultAdminUser(); // Tao admin mac dinh
    size_t getUserCount() const; // Dem so user
    size_t getWalletCount() const; // Dem so vi
    size_t getTransactionCount() const; // Dem so giao dich

    // Cac ham admin authentication
    bool authenticateAdmin(const std::string& username, const std::string& password) const; // Xac thuc admin
    bool isAdminUser(const std::string& username) const; // Kiem tra la admin

    // Cac ham admin user management
    bool adminCreateUser(const std::string& username, const std::string& fullName,
                        const std::string& birthDate, std::string& generatedPassword); // Admin tao user moi
    bool adminViewAllUsers() const; // Admin xem tat ca user
    bool adminUpdateUserInfo(const std::string& username, const std::string& newFullName,
                           const std::string& newBirthDate, const std::string& otpCode); // Admin cap nhat user
    bool adminDeleteUser(const std::string& username); // Admin xoa user

    // Cac ham admin wallet management
    bool adminViewAllWallets() const; // Admin xem tat ca vi
    bool adminViewWalletDetails(const std::string& walletId) const; // Admin xem chi tiet vi

    // Cac ham admin system management
    bool adminBackupDatabase(const std::string& backupPath = "") const; // Admin sao luu database
    bool adminRestoreDatabase(const std::string& backupPath); // Admin phuc hoi database
    bool adminViewSystemStats() const; // Admin xem thong ke he thong
    bool adminViewTransactionLog() const; // Admin xem log giao dich

    // Cac ham admin utility
    std::string generateRandomPassword(int length = 8) const; // Tao mat khau ngau nhien
    bool validateOTPForUser(const std::string& username, const std::string& otpCode) const; // Xac thuc OTP
    std::string getCurrentTimestamp() const; // Lay timestamp hien tai
};
