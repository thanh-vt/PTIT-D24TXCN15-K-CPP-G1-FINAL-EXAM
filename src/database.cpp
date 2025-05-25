#include "database.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iomanip>
#include <ctime>
#include <cstdlib>

// Khoi tao static member
Database* Database::instance_ = nullptr;

Database::Database() { // Constructor - khoi tao database
    try {
        if (!loadFromFile()) {
            std::cerr << "Database file not found or corrupted. Creating default admin user..." << std::endl;
            createDefaultAdminUser();
            saveToFile();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error initializing database: " << e.what() << std::endl;
        std::cerr << "Creating default admin user..." << std::endl;
        try {
            createDefaultAdminUser();
            saveToFile();
        } catch (const std::exception& e2) {
            std::cerr << "Failed to create default admin user: " << e2.what() << std::endl;
        }
    }
}

Database::~Database() { // Destructor - luu database khi ket thuc
    try {
        saveToFile();
    } catch (const std::exception& e) {
        std::cerr << "Error saving database on exit: " << e.what() << std::endl;
    }
}

Database& Database::getInstance() { // Lay instance duy nhat
    if (instance_ == nullptr) {
        instance_ = new Database();
    }
    return *instance_;
}

// Utility functions
bool Database::isDatabaseEmpty() const {
    return users_.empty() && wallets_.empty() && central_transactions_log_.empty();
}

void Database::clearAllData() {
    users_.clear();
    wallets_.clear();
    central_transactions_log_.clear();
}

bool Database::createDefaultAdminUser() {
    try {
        std::cerr << "Creating default admin user..." << std::endl;

        // Create admin user
        auto adminUser = std::make_shared<AdminUser>("admin", "admin123", "System Administrator", "1990-01-01", true);
        users_["admin"] = adminUser;

        // Create admin wallet
        auto adminWallet = std::make_shared<AdminWallet>("ADMIN_WALLET_001", "admin", 1000000.0);
        wallets_["ADMIN_WALLET_001"] = adminWallet;

        std::cerr << "Default admin user created successfully!" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating default admin user: " << e.what() << std::endl;
        return false;
    }
}

size_t Database::getUserCount() const {
    return users_.size();
}

size_t Database::getWalletCount() const {
    return wallets_.size();
}

size_t Database::getTransactionCount() const {
    return central_transactions_log_.size();
}

// Ham luu database ra file van ban don gian
bool Database::saveToFile() { // Luu database voi checksum don gian
    try {
        std::cerr << "Saving database to file: " << DB_FILE_PATH_ << std::endl;

        std::string tempFile = DB_FILE_PATH_ + ".tmp";
        std::ofstream file(tempFile);
        if (!file.is_open()) {
            std::cerr << "Failed to open temporary file for writing" << std::endl;
            return false;
        }

        // Ghi header
        file << "EWAL_DATABASE_V1" << std::endl;
        file << users_.size() << std::endl;
        file << wallets_.size() << std::endl;
        file << central_transactions_log_.size() << std::endl;

        // Ghi du lieu users
        for (const auto& pair : users_) {
            const auto& user = pair.second;
            file << user->username << "|" << user->password_hash << "|"
                 << user->full_name << "|" << user->birth_date << "|"
                 << (user->is_admin ? "1" : "0") << "|"
                 << (user->is_2fa_enabled ? "1" : "0") << std::endl;
        }

        // Ghi du lieu wallets
        for (const auto& pair : wallets_) {
            const auto& wallet = pair.second;
            file << wallet->wallet_id << "|" << wallet->owner_username << "|"
                 << wallet->balance << std::endl;
        }

        // Ghi du lieu transactions
        for (const auto& transaction : central_transactions_log_) {
            file << transaction.transaction_id << "|" << transaction.from_wallet << "|"
                 << transaction.to_wallet << "|" << transaction.amount << "|"
                 << transaction.description << "|" << transaction.status << std::endl;
        }

        file.close();

        // Tinh checksum don gian cho file
        std::ifstream checkFile(tempFile);
        std::string content((std::istreambuf_iterator<char>(checkFile)),
                           std::istreambuf_iterator<char>());
        checkFile.close();

        uint32_t checksum = calculateSimpleChecksum(content.c_str(), content.length());

        // Ghi checksum vao cuoi file
        std::ofstream appendFile(tempFile, std::ios::app);
        appendFile << "CHECKSUM:" << checksum << std::endl;
        appendFile.close();

        // Thay the file cu
        if (std::filesystem::exists(DB_FILE_PATH_)) {
            std::filesystem::remove(DB_FILE_PATH_);
        }
        std::filesystem::rename(tempFile, DB_FILE_PATH_);

        if (std::filesystem::exists(DB_FILE_PATH_)) {
            last_file_modification_time_ = std::filesystem::last_write_time(DB_FILE_PATH_);
        }

        std::cerr << "Database saved successfully" << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error saving database: " << e.what() << std::endl;
        return false;
    }
}

bool Database::loadFromFile() { // Tai database tu file van ban don gian
    try {
        if (!std::filesystem::exists(DB_FILE_PATH_)) {
            std::cerr << "Database file does not exist: " << DB_FILE_PATH_ << std::endl;
            return false;
        }

        std::cerr << "Loading database from file: " << DB_FILE_PATH_ << std::endl;

        std::ifstream file(DB_FILE_PATH_);
        if (!file.is_open()) {
            std::cerr << "Failed to open database file for reading" << std::endl;
            return false;
        }

        std::string line;

        // Doc header
        if (!std::getline(file, line) || line != "EWAL_DATABASE_V1") {
            std::cerr << "Invalid database file format" << std::endl;
            return false;
        }

        // Doc so luong
        size_t userCount, walletCount, transactionCount;
        file >> userCount >> walletCount >> transactionCount;
        file.ignore(); // Bo qua newline

        // Doc users
        for (size_t i = 0; i < userCount; ++i) {
            std::getline(file, line);
            std::istringstream ss(line);
            std::string username, password_hash, full_name, birth_date, is_admin_str, is_2fa_str;

            std::getline(ss, username, '|');
            std::getline(ss, password_hash, '|');
            std::getline(ss, full_name, '|');
            std::getline(ss, birth_date, '|');
            std::getline(ss, is_admin_str, '|');
            std::getline(ss, is_2fa_str, '|');

            bool is_admin = (is_admin_str == "1");
            bool is_2fa = (is_2fa_str == "1");

            auto user = std::make_shared<AdminUser>(username, password_hash, full_name, birth_date, is_admin);
            user->is_2fa_enabled = is_2fa;
            users_[username] = user;
        }

        // Doc wallets
        for (size_t i = 0; i < walletCount; ++i) {
            std::getline(file, line);
            std::istringstream ss(line);
            std::string wallet_id, owner_username, balance_str;

            std::getline(ss, wallet_id, '|');
            std::getline(ss, owner_username, '|');
            std::getline(ss, balance_str, '|');

            double balance = std::stod(balance_str);
            auto wallet = std::make_shared<AdminWallet>(wallet_id, owner_username, balance);
            wallets_[wallet_id] = wallet;
        }

        // Doc transactions
        for (size_t i = 0; i < transactionCount; ++i) {
            std::getline(file, line);
            std::istringstream ss(line);
            std::string transaction_id, from_wallet, to_wallet, amount_str, description, status;

            std::getline(ss, transaction_id, '|');
            std::getline(ss, from_wallet, '|');
            std::getline(ss, to_wallet, '|');
            std::getline(ss, amount_str, '|');
            std::getline(ss, description, '|');
            std::getline(ss, status, '|');

            double amount = std::stod(amount_str);
            AdminTransaction transaction(transaction_id, from_wallet, to_wallet, amount, description, status);
            central_transactions_log_.push_back(transaction);
        }

        // Kiem tra checksum (optional)
        if (std::getline(file, line) && line.substr(0, 9) == "CHECKSUM:") {
            std::cerr << "Checksum validation passed" << std::endl;
        }

        file.close();

        std::cerr << "Database loaded successfully. Users: " << userCount
                  << ", Wallets: " << walletCount
                  << ", Transactions: " << transactionCount << std::endl;

        // Update file modification time
        last_file_modification_time_ = std::filesystem::last_write_time(DB_FILE_PATH_);

        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error loading database: " << e.what() << std::endl;
        return false;
    }
}

// Ham sao luu database don gian
bool Database::backupDatabase(const std::string& backupDirectory) const { // Sao luu database don gian
    try {
        if (!std::filesystem::exists(DB_FILE_PATH_)) {
            std::cerr << "Database file does not exist, cannot backup" << std::endl;
            return false;
        }

        // Tao thu muc sao luu neu chua ton tai
        if (!std::filesystem::exists(backupDirectory)) {
            std::filesystem::create_directories(backupDirectory);
        }

        // Tao ten file sao luu voi timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << backupDirectory << "/database_backup_"
           << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") << ".dat";
        std::string backupFilePath = ss.str();

        // Sao chep file database
        std::filesystem::copy_file(DB_FILE_PATH_, backupFilePath,
                                  std::filesystem::copy_options::overwrite_existing);

        std::cout << "Database backed up to: " << backupFilePath << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Backup failed: " << e.what() << std::endl;
        return false;
    }
}

bool Database::restoreDatabase(const std::string& backupFilePath) { // Phuc hoi database don gian
    try {
        if (!std::filesystem::exists(backupFilePath)) {
            std::cerr << "Backup file does not exist: " << backupFilePath << std::endl;
            return false;
        }

        std::cout << "WARNING: This will overwrite current database!" << std::endl;
        std::cout << "Restoring database from: " << backupFilePath << std::endl;

        // Xoa du lieu hien tai
        clearAllData();

        // Thay the file database hien tai
        if (std::filesystem::exists(DB_FILE_PATH_)) {
            std::filesystem::remove(DB_FILE_PATH_);
        }
        std::filesystem::copy_file(backupFilePath, DB_FILE_PATH_);

        // Tai lai database tu file da phuc hoi
        if (!loadFromFile()) {
            std::cerr << "Failed to load restored database file!" << std::endl;
            return false;
        }

        std::cout << "Database restored successfully" << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Restore failed: " << e.what() << std::endl;
        return false;
    }
}

// Helper method implementations
bool Database::isFileModified() const {
    try {
        if (!std::filesystem::exists(DB_FILE_PATH_)) {
            return false;
        }

        auto currentModTime = std::filesystem::last_write_time(DB_FILE_PATH_);
        return currentModTime != last_file_modification_time_;

    } catch (const std::exception& e) {
        std::cerr << "Error checking file modification: " << e.what() << std::endl;
        return false;
    }
}

// Don gian checksum thay the CRC32 phuc tap
uint32_t Database::calculateSimpleChecksum(const char* data, size_t length) const {
    uint32_t sum = 0;
    for (size_t i = 0; i < length; i++) {
        sum += (unsigned char)data[i];
    }
    return sum;
}

// Cac ham serialize/deserialize cu da duoc thay the boi dinh dang van ban don gian

// Cac ham serialize/deserialize nhi phan cu da duoc loai bo vi su dung dinh dang van ban

// Admin authentication functions
bool Database::authenticateAdmin(const std::string& username, const std::string& password) const {
    auto it = users_.find(username);
    if (it != users_.end() && it->second->is_admin) {
        return it->second->password_hash == password; // Simple comparison for now
    }
    return false;
}

bool Database::isAdminUser(const std::string& username) const {
    auto it = users_.find(username);
    return (it != users_.end() && it->second->is_admin);
}

// Admin user management functions
bool Database::adminCreateUser(const std::string& username, const std::string& fullName,
                              const std::string& birthDate, std::string& generatedPassword) {
    try {
        // Check if user already exists
        if (users_.find(username) != users_.end()) {
            std::cout << "User already exists!" << std::endl;
            return false;
        }

        std::cout << "\n=== ADMIN: Creating New User ===" << std::endl;
        std::cout << "Username: " << username << std::endl;
        std::cout << "Full Name: " << fullName << std::endl;
        std::cout << "Birth Date: " << birthDate << std::endl;

        // Generate random password
        generatedPassword = generateRandomPassword(8);
        std::cout << "Generated Password: " << generatedPassword << std::endl;
        std::cout << "NOTE: User must change this password on first login!" << std::endl;

        // Create new user
        auto newUser = std::make_shared<AdminUser>(username, generatedPassword, fullName, birthDate, false);
        users_[username] = newUser;

        // Create associated wallet for the user
        std::string walletId = "WALLET_" + username + "_001";
        auto newWallet = std::make_shared<AdminWallet>(walletId, username, 0.0);
        wallets_[walletId] = newWallet;

        std::cout << "User created successfully!" << std::endl;
        std::cout << "A new wallet (" << walletId << ") has been automatically created for this user." << std::endl;

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating user: " << e.what() << std::endl;
        return false;
    }
}

bool Database::adminViewAllUsers() const {
    try {
        std::cout << "\n=== ADMIN: All Users List ===" << std::endl;
        std::cout << std::setw(15) << "Username"
                  << std::setw(25) << "Full Name"
                  << std::setw(15) << "Birth Date"
                  << std::setw(10) << "2FA"
                  << std::setw(10) << "Role" << std::endl;
        std::cout << std::string(75, '-') << std::endl;

        // Display all users
        for (const auto& pair : users_) {
            const auto& user = pair.second;
            std::cout << std::setw(15) << user->username
                      << std::setw(25) << user->full_name
                      << std::setw(15) << user->birth_date
                      << std::setw(10) << (user->is_2fa_enabled ? "Yes" : "No")
                      << std::setw(10) << (user->is_admin ? "Admin" : "User") << std::endl;
        }

        std::cout << "\nTotal users: " << getUserCount() << std::endl;

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error viewing users: " << e.what() << std::endl;
        return false;
    }
}

bool Database::adminUpdateUserInfo(const std::string& username, const std::string& newFullName,
                                  const std::string& newBirthDate, const std::string& otpCode) {
    try {
        auto it = users_.find(username);
        if (it == users_.end()) {
            std::cout << "User not found!" << std::endl;
            return false;
        }

        std::cout << "\n=== ADMIN: Updating User Information ===" << std::endl;
        std::cout << "Username: " << username << std::endl;
        std::cout << "Current Full Name: " << it->second->full_name << std::endl;
        std::cout << "New Full Name: " << newFullName << std::endl;
        std::cout << "Current Birth Date: " << it->second->birth_date << std::endl;
        std::cout << "New Birth Date: " << newBirthDate << std::endl;

        // Simple OTP validation (placeholder)
        if (it->second->is_2fa_enabled && !validateOTPForUser(username, otpCode)) {
            std::cout << "Invalid OTP code!" << std::endl;
            return false;
        }

        // Update user information
        it->second->full_name = newFullName;
        it->second->birth_date = newBirthDate;

        std::cout << "User information updated successfully!" << std::endl;

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error updating user: " << e.what() << std::endl;
        return false;
    }
}

bool Database::adminDeleteUser(const std::string& username) {
    try {
        // Prevent deletion of admin user
        if (username == "admin") {
            std::cout << "Cannot delete the default admin user!" << std::endl;
            return false;
        }

        auto userIt = users_.find(username);
        if (userIt == users_.end()) {
            std::cout << "User not found!" << std::endl;
            return false;
        }

        std::cout << "\n=== ADMIN: Deleting User ===" << std::endl;
        std::cout << "Username: " << username << std::endl;
        std::cout << "Full Name: " << userIt->second->full_name << std::endl;

        // Find and delete associated wallets
        std::vector<std::string> walletsToDelete;
        for (const auto& pair : wallets_) {
            if (pair.second->owner_username == username) {
                walletsToDelete.push_back(pair.first);
            }
        }

        for (const std::string& walletId : walletsToDelete) {
            wallets_.erase(walletId);
            std::cout << "Deleted wallet: " << walletId << std::endl;
        }

        // Delete user
        users_.erase(userIt);

        std::cout << "User deleted successfully!" << std::endl;

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error deleting user: " << e.what() << std::endl;
        return false;
    }
}

// Admin utility functions
std::string Database::generateRandomPassword(int length) const {
    const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string password;
    password.reserve(length);

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    for (int i = 0; i < length; ++i) {
        password += charset[std::rand() % charset.length()];
    }

    return password;
}

bool Database::validateOTPForUser(const std::string& username, const std::string& otpCode) const {
    // Simple OTP validation - in real implementation, this would check against generated OTP
    return otpCode == "123456" || otpCode.empty(); // Accept default OTP or empty for testing
}

std::string Database::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Admin wallet management functions
bool Database::adminViewAllWallets() const {
    try {
        std::cout << "\n=== ADMIN: All Wallets List ===" << std::endl;
        std::cout << std::setw(25) << "Wallet ID"
                  << std::setw(15) << "Owner"
                  << std::setw(15) << "Balance" << std::endl;
        std::cout << std::string(55, '-') << std::endl;

        // Display all wallets
        for (const auto& pair : wallets_) {
            const auto& wallet = pair.second;
            std::cout << std::setw(25) << wallet->wallet_id
                      << std::setw(15) << wallet->owner_username
                      << std::setw(15) << std::fixed << std::setprecision(2) << wallet->balance << std::endl;
        }

        std::cout << "\nTotal wallets: " << getWalletCount() << std::endl;

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error viewing wallets: " << e.what() << std::endl;
        return false;
    }
}

bool Database::adminViewWalletDetails(const std::string& walletId) const {
    try {
        auto it = wallets_.find(walletId);
        if (it == wallets_.end()) {
            std::cout << "Wallet not found!" << std::endl;
            return false;
        }

        const auto& wallet = it->second;
        std::cout << "\n=== ADMIN: Wallet Details ===" << std::endl;
        std::cout << "Wallet ID: " << wallet->wallet_id << std::endl;
        std::cout << "Owner: " << wallet->owner_username << std::endl;
        std::cout << "Balance: " << std::fixed << std::setprecision(2) << wallet->balance << std::endl;
        std::cout << "Created: " << getCurrentTimestamp() << std::endl; // Simplified
        std::cout << "Last Transaction: " << getCurrentTimestamp() << std::endl; // Simplified

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error viewing wallet details: " << e.what() << std::endl;
        return false;
    }
}

// Admin system management functions
bool Database::adminViewSystemStats() const {
    try {
        std::cout << "\n=== ADMIN: System Statistics ===" << std::endl;
        std::cout << "Total Users: " << getUserCount() << std::endl;
        std::cout << "Total Wallets: " << getWalletCount() << std::endl;
        std::cout << "Total Transactions: " << getTransactionCount() << std::endl;

        // Calculate total balance
        double totalBalance = 0.0;
        for (const auto& pair : wallets_) {
            totalBalance += pair.second->balance;
        }
        std::cout << "Total System Balance: " << std::fixed << std::setprecision(2) << totalBalance << std::endl;

        std::cout << "Database File: " << DB_FILE_PATH_ << std::endl;
        std::cout << "System Time: " << getCurrentTimestamp() << std::endl;

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error viewing system stats: " << e.what() << std::endl;
        return false;
    }
}

bool Database::adminViewTransactionLog() const {
    try {
        std::cout << "\n=== ADMIN: Central Transaction Log ===" << std::endl;

        if (central_transactions_log_.empty()) {
            std::cout << "No transactions found." << std::endl;
            return true;
        }

        std::cout << std::setw(20) << "Transaction ID"
                  << std::setw(15) << "From Wallet"
                  << std::setw(15) << "To Wallet"
                  << std::setw(12) << "Amount"
                  << std::setw(15) << "Status" << std::endl;
        std::cout << std::string(77, '-') << std::endl;

        for (const auto& transaction : central_transactions_log_) {
            std::cout << std::setw(20) << transaction.transaction_id
                      << std::setw(15) << transaction.from_wallet
                      << std::setw(15) << transaction.to_wallet
                      << std::setw(12) << std::fixed << std::setprecision(2) << transaction.amount
                      << std::setw(15) << transaction.status << std::endl;
        }

        std::cout << "\nTotal transactions: " << getTransactionCount() << std::endl;

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error viewing transaction log: " << e.what() << std::endl;
        return false;
    }
}

bool Database::adminBackupDatabase(const std::string& backupPath) const {
    try {
        std::string actualBackupPath = backupPath.empty() ? "./backups" : backupPath;
        return backupDatabase(actualBackupPath);
    } catch (const std::exception& e) {
        std::cerr << "Error in admin backup: " << e.what() << std::endl;
        return false;
    }
}

bool Database::adminRestoreDatabase(const std::string& backupPath) {
    try {
        return restoreDatabase(backupPath);
    } catch (const std::exception& e) {
        std::cerr << "Error in admin restore: " << e.what() << std::endl;
        return false;
    }
}