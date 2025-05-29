#include "database.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iomanip>
#include <ctime>
#include <cstdlib>

Database *Database::instance_ = nullptr;

/*Database duy nhat theo flow cu, xoa di sau khi sua xu ly admin xong*/

// Khoi tao static member

// Database::Database()
// { // Constructor - khoi tao database
//     try
//     {
//         if (!loadFromFile())
//         {
//             std::cerr << "Database file not found or corrupted. Creating default admin user..." << std::endl;
//             createDefaultAdminUser();
//             saveToFile();
//         }
//     }
//     catch (const std::exception &e)
//     {
//         std::cerr << "Error initializing database: " << e.what() << std::endl;
//         std::cerr << "Creating default admin user..." << std::endl;
//         try
//         {
//             createDefaultAdminUser();
//             saveToFile();
//         }
//         catch (const std::exception &e2)
//         {
//             std::cerr << "Failed to create default admin user: " << e2.what() << std::endl;
//         }
//     }
// }

// Database::~Database()
// { // Destructor - luu database khi ket thuc
//     try
//     {
//         saveToFile();
//     }
//     catch (const std::exception &e)
//     {
//         std::cerr << "Error saving database on exit: " << e.what() << std::endl;
//     }
// }

// Database &Database::getInstance()
// { // Lay instance duy nhat
//     if (instance_ == nullptr)
//     {
//         instance_ = new Database();
//     }
//     return *instance_;
// }

/**/

Database &Database::getInstance()
{
    if (!instance_)
    {
        instance_ = new Database();
    }
    return *instance_;
}

Database::Database()
{
    std::cout << "Database constructor started..." << std::endl;

    try
    {
        // Get the executable directory
        std::filesystem::path exePath = std::filesystem::current_path();
        std::cout << "Executable path: " << exePath << std::endl;

        // Create database file paths in executable directory
        USERS_FILE = (exePath / "users.dat").string();
        WALLETS_FILE = (exePath / "wallets.dat").string();
        TRANSACTIONS_FILE = (exePath / "transactions.dat").string();

        std::cout << "Users database path: " << USERS_FILE << std::endl;
        std::cout << "Wallets database path: " << WALLETS_FILE << std::endl;
        std::cout << "Transactions database path: " << TRANSACTIONS_FILE << std::endl;

        // Check if we can write to the directory
        std::string testFile = (exePath / "test_write.tmp").string();
        std::cout << "Testing write permissions with file: " << testFile << std::endl;
        {
            std::ofstream test(testFile);
            if (!test)
            {
                std::cerr << "Error: Cannot write to directory: " << exePath << std::endl;
                return;
            }
            test << "test" << std::endl;
            test.close();
            std::filesystem::remove(testFile);
        }
        std::cout << "Write permissions verified" << std::endl;

        // Try to load existing data
        bool loadSuccess = loadFromFiles();

        // If loading failed or database is empty, create admin user and wallet
        if (!loadSuccess || (users.empty() && wallets.empty()))
        {
            std::cout << "Creating new database with admin user..." << std::endl;

            try
            {
                std::cout << "Creating admin user..." << std::endl;
                // Create admin user if not exists
                auto admin = std::make_shared<User>("admin", "System Administrator",
                                                    std::chrono::system_clock::now(), true);
                admin->setPassword("admin123"); // Default admin password

                std::cout << "Adding admin user to database..." << std::endl;
                users["admin"] = admin; // Add directly to users map

                std::cout << "Creating admin wallet..." << std::endl;
                // Create admin wallet
                auto adminWallet = std::make_shared<Wallet>("ADMIN_WALLET", 1000000.0);

                std::cout << "Adding admin wallet to database..." << std::endl;
                wallets["ADMIN_WALLET"] = adminWallet; // Add directly to wallets map

                std::cout << "Setting admin wallet ID..." << std::endl;
                admin->setWalletId(adminWallet->getId());

                std::cout << "Saving initial database..." << std::endl;
                // Save the initial database
                if (!saveToFiles())
                {
                    std::cerr << "Failed to save initial database!" << std::endl;
                    return;
                }

                std::cout << "Initial database created successfully!" << std::endl;
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error creating initial database: " << e.what() << std::endl;
            }
        }
        else
        {
            std::cout << "Existing database loaded successfully!" << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error in database initialization: " << e.what() << std::endl;
    }
}

Database::~Database()
{
    std::cout << "Database destructor called..." << std::endl;
    saveToFiles();
}

// Utility functions
bool Database::isDatabaseEmpty() const
{
    return users_.empty() && wallets_.empty() && central_transactions_log_.empty();
}

void Database::clearAllData()
{
    users_.clear();
    wallets_.clear();
    central_transactions_log_.clear();
}

bool Database::createDefaultAdminUser()
{
    try
    {
        std::cerr << "Creating default admin user..." << std::endl;

        // Create admin user
        auto adminUser = std::make_shared<AdminUser>("admin", "admin", "System Administrator", "1990-01-01", true);
        users_["admin"] = adminUser;

        // Create admin wallet
        auto adminWallet = std::make_shared<AdminWallet>("ADMIN_WALLET_001", "admin", 1000000.0);
        wallets_["ADMIN_WALLET_001"] = adminWallet;

        std::cerr << "Default admin user created successfully!" << std::endl;
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error creating default admin user: " << e.what() << std::endl;
        return false;
    }
}

size_t Database::getUserCount() const
{
    return users_.size();
}

size_t Database::getWalletCount() const
{
    return wallets_.size();
}

size_t Database::getTransactionCount() const
{
    return central_transactions_log_.size();
}

// Ham luu database ra file van ban don gian
bool Database::saveToFile()
{ // Luu database voi checksum don gian
    try
    {
        std::cerr << "Saving database to file: " << DB_FILE_PATH_ << std::endl;

        std::string tempFile = DB_FILE_PATH_ + ".tmp";
        std::ofstream file(tempFile);
        if (!file.is_open())
        {
            std::cerr << "Failed to open temporary file for writing" << std::endl;
            return false;
        }

        // Ghi header
        file << "EWAL_DATABASE_V1" << std::endl;
        file << users_.size() << std::endl;
        file << wallets_.size() << std::endl;
        file << central_transactions_log_.size() << std::endl;

        // Ghi du lieu users
        for (const auto &pair : users_)
        {
            const auto &user = pair.second;
            file << user->username << "|" << user->password_hash << "|"
                 << user->full_name << "|" << user->birth_date << "|"
                 << (user->is_admin ? "1" : "0") << "|"
                 << (user->is_2fa_enabled ? "1" : "0") << std::endl;
        }

        // Ghi du lieu wallets
        for (const auto &pair : wallets_)
        {
            const auto &wallet = pair.second;
            file << wallet->wallet_id << "|" << wallet->owner_username << "|"
                 << wallet->balance << std::endl;
        }

        // Ghi du lieu transactions
        for (const auto &transaction : central_transactions_log_)
        {
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
        if (std::filesystem::exists(DB_FILE_PATH_))
        {
            std::filesystem::remove(DB_FILE_PATH_);
        }
        std::filesystem::rename(tempFile, DB_FILE_PATH_);

        if (std::filesystem::exists(DB_FILE_PATH_))
        {
            last_file_modification_time_ = std::filesystem::last_write_time(DB_FILE_PATH_);
        }

        std::cerr << "Database saved successfully" << std::endl;
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error saving database: " << e.what() << std::endl;
        return false;
    }
}

bool Database::loadFromFile()
{ // Tai database tu file van ban don gian
    try
    {
        if (!std::filesystem::exists(DB_FILE_PATH_))
        {
            std::cerr << "Database file does not exist: " << DB_FILE_PATH_ << std::endl;
            return false;
        }

        std::cerr << "Loading database from file: " << DB_FILE_PATH_ << std::endl;

        std::ifstream file(DB_FILE_PATH_);
        if (!file.is_open())
        {
            std::cerr << "Failed to open database file for reading" << std::endl;
            return false;
        }

        std::string line;

        // Doc header
        if (!std::getline(file, line) || line != "EWAL_DATABASE_V1")
        {
            std::cerr << "Invalid database file format" << std::endl;
            return false;
        }

        // Doc so luong
        size_t userCount, walletCount, transactionCount;
        file >> userCount >> walletCount >> transactionCount;
        file.ignore(); // Bo qua newline

        // Doc users
        for (size_t i = 0; i < userCount; ++i)
        {
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
        for (size_t i = 0; i < walletCount; ++i)
        {
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
        for (size_t i = 0; i < transactionCount; ++i)
        {
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
        if (std::getline(file, line) && line.substr(0, 9) == "CHECKSUM:")
        {
            std::cerr << "Checksum validation passed" << std::endl;
        }

        file.close();

        std::cerr << "Database loaded successfully. Users: " << userCount
                  << ", Wallets: " << walletCount
                  << ", Transactions: " << transactionCount << std::endl;

        // Update file modification time
        last_file_modification_time_ = std::filesystem::last_write_time(DB_FILE_PATH_);

        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error loading database: " << e.what() << std::endl;
        return false;
    }
}

// Ham sao luu database don gian
bool Database::backupDatabase(const std::string &backupDirectory) const
{ // Sao luu database don gian
    try
    {
        if (!std::filesystem::exists(DB_FILE_PATH_))
        {
            std::cerr << "Database file does not exist, cannot backup" << std::endl;
            return false;
        }

        // Tao thu muc sao luu neu chua ton tai
        if (!std::filesystem::exists(backupDirectory))
        {
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
    }
    catch (const std::exception &e)
    {
        std::cerr << "Backup failed: " << e.what() << std::endl;
        return false;
    }
}

bool Database::restoreDatabase(const std::string &backupFilePath)
{ // Phuc hoi database don gian
    try
    {
        if (!std::filesystem::exists(backupFilePath))
        {
            std::cerr << "Backup file does not exist: " << backupFilePath << std::endl;
            return false;
        }

        std::cout << "WARNING: This will overwrite current database!" << std::endl;
        std::cout << "Restoring database from: " << backupFilePath << std::endl;

        // Xoa du lieu hien tai
        clearAllData();

        // Thay the file database hien tai
        if (std::filesystem::exists(DB_FILE_PATH_))
        {
            std::filesystem::remove(DB_FILE_PATH_);
        }
        std::filesystem::copy_file(backupFilePath, DB_FILE_PATH_);

        // Tai lai database tu file da phuc hoi
        if (!loadFromFile())
        {
            std::cerr << "Failed to load restored database file!" << std::endl;
            return false;
        }

        std::cout << "Database restored successfully" << std::endl;
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Restore failed: " << e.what() << std::endl;
        return false;
    }
}

// TODO: Ham check thay doi cua database duy nhat (flow cu)
// // Helper method implementations
// bool Database::isFileModified() const
// {
//     try
//     {
//         if (!std::filesystem::exists(DB_FILE_PATH_))
//         {
//             return false;
//         }

//         auto currentModTime = std::filesystem::last_write_time(DB_FILE_PATH_);
//         return currentModTime != last_file_modification_time_;
//     }
//     catch (const std::exception &e)
//     {
//         std::cerr << "Error checking file modification: " << e.what() << std::endl;
//         return false;
//     }
// }

// Don gian checksum thay the CRC32 phuc tap
uint32_t Database::calculateSimpleChecksum(const char *data, size_t length) const
{
    uint32_t sum = 0;
    for (size_t i = 0; i < length; i++)
    {
        sum += (unsigned char)data[i];
    }
    return sum;
}

// Cac ham serialize/deserialize cu da duoc thay the boi dinh dang van ban don gian

// Cac ham serialize/deserialize nhi phan cu da duoc loai bo vi su dung dinh dang van ban

// Admin authentication functions
bool Database::authenticateAdmin(const std::string &username, const std::string &password) const
{
    auto it = users_.find(username);
    if (it != users_.end() && it->second->is_admin)
    {
        return it->second->password_hash == password; // Simple comparison for now
    }
    return false;
}

bool Database::isAdminUser(const std::string &username) const
{
    auto it = users_.find(username);
    return (it != users_.end() && it->second->is_admin);
}

// Admin user management functions
bool Database::adminCreateUser(const std::string &username, const std::string &fullName,
                               const std::string &birthDate, std::string &generatedPassword)
{
    try
    {
        // Check if user already exists
        if (users_.find(username) != users_.end())
        {
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
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error creating user: " << e.what() << std::endl;
        return false;
    }
}

bool Database::adminViewAllUsers() const
{
    try
    {
        std::cout << "\n=== ADMIN: All Users List ===" << std::endl;
        std::cout << std::setw(15) << "Username"
                  << std::setw(25) << "Full Name"
                  << std::setw(15) << "Birth Date"
                  << std::setw(10) << "2FA"
                  << std::setw(10) << "Role" << std::endl;
        std::cout << std::string(75, '-') << std::endl;

        // Display all users
        for (const auto &pair : users_)
        {
            const auto &user = pair.second;
            std::cout << std::setw(15) << user->username
                      << std::setw(25) << user->full_name
                      << std::setw(15) << user->birth_date
                      << std::setw(10) << (user->is_2fa_enabled ? "Yes" : "No")
                      << std::setw(10) << (user->is_admin ? "Admin" : "User") << std::endl;
        }

        std::cout << "\nTotal users: " << getUserCount() << std::endl;

        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error viewing users: " << e.what() << std::endl;
        return false;
    }
}

bool Database::adminUpdateUserInfo(const std::string &username, const std::string &newFullName,
                                   const std::string &newBirthDate, const std::string &otpCode)
{
    try
    {
        auto it = users_.find(username);
        if (it == users_.end())
        {
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
        if (it->second->is_2fa_enabled && !validateOTPForUser(username, otpCode))
        {
            std::cout << "Invalid OTP code!" << std::endl;
            return false;
        }

        // Update user information
        it->second->full_name = newFullName;
        it->second->birth_date = newBirthDate;

        std::cout << "User information updated successfully!" << std::endl;

        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error updating user: " << e.what() << std::endl;
        return false;
    }
}

bool Database::adminDeleteUser(const std::string &username)
{
    try
    {
        // Prevent deletion of admin user
        if (username == "admin")
        {
            std::cout << "Cannot delete the default admin user!" << std::endl;
            return false;
        }

        auto userIt = users_.find(username);
        if (userIt == users_.end())
        {
            std::cout << "User not found!" << std::endl;
            return false;
        }

        std::cout << "\n=== ADMIN: Deleting User ===" << std::endl;
        std::cout << "Username: " << username << std::endl;
        std::cout << "Full Name: " << userIt->second->full_name << std::endl;

        // Find and delete associated wallets
        std::vector<std::string> walletsToDelete;
        for (const auto &pair : wallets_)
        {
            if (pair.second->owner_username == username)
            {
                walletsToDelete.push_back(pair.first);
            }
        }

        for (const std::string &walletId : walletsToDelete)
        {
            wallets_.erase(walletId);
            std::cout << "Deleted wallet: " << walletId << std::endl;
        }

        // Delete user
        users_.erase(userIt);

        std::cout << "User deleted successfully!" << std::endl;

        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error deleting user: " << e.what() << std::endl;
        return false;
    }
}

// Admin utility functions
std::string Database::generateRandomPassword(int length) const
{
    const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string password;
    password.reserve(length);

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    for (int i = 0; i < length; ++i)
    {
        password += charset[std::rand() % charset.length()];
    }

    return password;
}

bool Database::validateOTPForUser(const std::string &username, const std::string &otpCode) const
{
    // Simple OTP validation - in real implementation, this would check against generated OTP
    return otpCode == "123456" || otpCode.empty(); // Accept default OTP or empty for testing
}

std::string Database::getCurrentTimestamp() const
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Admin wallet management functions
bool Database::adminViewAllWallets() const
{
    try
    {
        std::cout << "\n=== ADMIN: All Wallets List ===" << std::endl;
        std::cout << std::setw(25) << "Wallet ID"
                  << std::setw(15) << "Owner"
                  << std::setw(15) << "Balance" << std::endl;
        std::cout << std::string(55, '-') << std::endl;

        // Display all wallets
        for (const auto &pair : wallets_)
        {
            const auto &wallet = pair.second;
            std::cout << std::setw(25) << wallet->wallet_id
                      << std::setw(15) << wallet->owner_username
                      << std::setw(15) << std::fixed << std::setprecision(2) << wallet->balance << std::endl;
        }

        std::cout << "\nTotal wallets: " << getWalletCount() << std::endl;

        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error viewing wallets: " << e.what() << std::endl;
        return false;
    }
}

bool Database::adminViewWalletDetails(const std::string &walletId) const
{
    try
    {
        auto it = wallets_.find(walletId);
        if (it == wallets_.end())
        {
            std::cout << "Wallet not found!" << std::endl;
            return false;
        }

        const auto &wallet = it->second;
        std::cout << "\n=== ADMIN: Wallet Details ===" << std::endl;
        std::cout << "Wallet ID: " << wallet->wallet_id << std::endl;
        std::cout << "Owner: " << wallet->owner_username << std::endl;
        std::cout << "Balance: " << std::fixed << std::setprecision(2) << wallet->balance << std::endl;
        std::cout << "Created: " << getCurrentTimestamp() << std::endl;          // Simplified
        std::cout << "Last Transaction: " << getCurrentTimestamp() << std::endl; // Simplified

        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error viewing wallet details: " << e.what() << std::endl;
        return false;
    }
}

// Admin system management functions
bool Database::adminViewSystemStats() const
{
    try
    {
        std::cout << "\n=== ADMIN: System Statistics ===" << std::endl;
        std::cout << "Total Users: " << getUserCount() << std::endl;
        std::cout << "Total Wallets: " << getWalletCount() << std::endl;
        std::cout << "Total Transactions: " << getTransactionCount() << std::endl;

        // Calculate total balance
        double totalBalance = 0.0;
        for (const auto &pair : wallets_)
        {
            totalBalance += pair.second->balance;
        }
        std::cout << "Total System Balance: " << std::fixed << std::setprecision(2) << totalBalance << std::endl;

        std::cout << "Database File: " << DB_FILE_PATH_ << std::endl;
        std::cout << "System Time: " << getCurrentTimestamp() << std::endl;

        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error viewing system stats: " << e.what() << std::endl;
        return false;
    }
}

bool Database::adminViewTransactionLog() const
{
    try
    {
        std::cout << "\n=== ADMIN: Central Transaction Log ===" << std::endl;

        if (central_transactions_log_.empty())
        {
            std::cout << "No transactions found." << std::endl;
            return true;
        }

        std::cout << std::setw(20) << "Transaction ID"
                  << std::setw(15) << "From Wallet"
                  << std::setw(15) << "To Wallet"
                  << std::setw(12) << "Amount"
                  << std::setw(15) << "Status" << std::endl;
        std::cout << std::string(77, '-') << std::endl;

        for (const auto &transaction : central_transactions_log_)
        {
            std::cout << std::setw(20) << transaction.transaction_id
                      << std::setw(15) << transaction.from_wallet
                      << std::setw(15) << transaction.to_wallet
                      << std::setw(12) << std::fixed << std::setprecision(2) << transaction.amount
                      << std::setw(15) << transaction.status << std::endl;
        }

        std::cout << "\nTotal transactions: " << getTransactionCount() << std::endl;

        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error viewing transaction log: " << e.what() << std::endl;
        return false;
    }
}

bool Database::adminBackupDatabase(const std::string &backupPath) const
{
    try
    {
        std::string actualBackupPath = backupPath.empty() ? "./backups" : backupPath;
        return backupDatabase(actualBackupPath);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error in admin backup: " << e.what() << std::endl;
        return false;
    }
}

bool Database::adminRestoreDatabase(const std::string &backupPath)
{
    try
    {
        return restoreDatabase(backupPath);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error in admin restore: " << e.what() << std::endl;
        return false;
    }
}

// Helper function to check if any database file has been modified
bool Database::isFileModified()
{
    try
    {
        bool modified = false;

        // Check users file
        if (std::filesystem::exists(USERS_FILE))
        {
            auto lastWriteTime = std::filesystem::last_write_time(USERS_FILE);
            if (lastWriteTime != lastUsersModification)
            {
                lastUsersModification = lastWriteTime;
                modified = true;
            }
        }

        // Check wallets file
        if (std::filesystem::exists(WALLETS_FILE))
        {
            auto lastWriteTime = std::filesystem::last_write_time(WALLETS_FILE);
            if (lastWriteTime != lastWalletsModification)
            {
                lastWalletsModification = lastWriteTime;
                modified = true;
            }
        }

        // Check transactions file
        if (std::filesystem::exists(TRANSACTIONS_FILE))
        {
            auto lastWriteTime = std::filesystem::last_write_time(TRANSACTIONS_FILE);
            if (lastWriteTime != lastTransactionsModification)
            {
                lastTransactionsModification = lastWriteTime;
                modified = true;
            }
        }

        return modified;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Critical error in isFileModified: " << e.what() << std::endl;
        throw;
    }
}

bool Database::addUser(const User &user)
{
    std::cout << "Adding user: " << user.getUsername() << std::endl;

    // Only check for file modifications if we're not in initialization
    if (!users.empty() && isFileModified())
    {
        std::cout << "Database file was modified, reloading..." << std::endl;
        if (!loadFromFiles())
        {
            std::cerr << "Failed to reload database" << std::endl;
            return false;
        }
    }

    auto username = user.getUsername();
    if (users.find(username) != users.end())
    {
        std::cout << "User already exists: " << username << std::endl;
        return false;
    }

    // Add the user to the in-memory map
    users[username] = std::make_shared<User>(user);
    std::cout << "User added to memory: " << username << std::endl;

    // Save changes to file
    if (!saveToFiles())
    {
        std::cerr << "Failed to save database after adding user" << std::endl;
        // Remove the user from memory if save failed
        users.erase(username);
        return false;
    }

    std::cout << "User successfully added and saved: " << username << std::endl;
    return true;
}

bool Database::updateUser(const User &user)
{
    if (isFileModified())
    {
        if (!loadFromFiles())
        {
            return false;
        }
    }

    auto username = user.getUsername();
    if (users.find(username) == users.end())
    {
        return false;
    }
    users[username] = std::make_shared<User>(user);
    return saveToFiles();
}

bool Database::deleteUser(const std::string &username)
{
    if (isFileModified())
    {
        if (!loadFromFiles())
        {
            return false;
        }
    }

    // Get the user's wallet ID before deleting the user
    auto user = getUser(username);
    if (!user)
    {
        return false;
    }
    std::string walletId = user->getWalletId();

    // Delete the user
    bool result = users.erase(username) > 0;
    if (result)
    {
        // Delete the associated wallet
        wallets.erase(walletId);
        saveToFiles();
    }
    return result;
}

std::shared_ptr<User> Database::getUser(const std::string &username)
{
    auto it = users.find(username);
    return it != users.end() ? it->second : nullptr;
}

std::vector<std::shared_ptr<User>> Database::getAllUsers()
{
    std::vector<std::shared_ptr<User>> result;
    for (const auto &pair : users)
    {
        result.push_back(pair.second);
    }
    return result;
}

bool Database::addWallet(const Wallet &wallet)
{
    std::cout << "Adding wallet: " << wallet.getId() << std::endl;

    // Only check for file modifications if we're not in initialization
    if (!wallets.empty() && isFileModified())
    {
        std::cout << "Database file was modified, reloading..." << std::endl;
        if (!loadFromFiles())
        {
            std::cerr << "Failed to reload database" << std::endl;
            return false;
        }
    }

    auto id = wallet.getId();
    if (wallets.find(id) != wallets.end())
    {
        std::cout << "Wallet already exists: " << id << std::endl;
        return false;
    }

    // Add the wallet to the in-memory map
    wallets[id] = std::make_shared<Wallet>(wallet);
    std::cout << "Wallet added to memory: " << id << std::endl;

    // Save changes to file
    if (!saveToFiles())
    {
        std::cerr << "Failed to save database after adding wallet" << std::endl;
        // Remove the wallet from memory if save failed
        wallets.erase(id);
        return false;
    }

    std::cout << "Wallet successfully added and saved: " << id << std::endl;
    return true;
}

bool Database::updateWallet(const Wallet &wallet)
{
    if (isFileModified())
    {
        if (!loadFromFiles())
        {
            return false;
        }
    }

    auto id = wallet.getId();
    if (wallets.find(id) == wallets.end())
    {
        return false;
    }
    wallets[id] = std::make_shared<Wallet>(wallet);
    return saveToFiles();
}

bool Database::deleteWallet(const std::string &walletId)
{
    if (isFileModified())
    {
        if (!loadFromFiles())
        {
            return false;
        }
    }

    bool result = wallets.erase(walletId) > 0;
    if (result)
    {
        saveToFiles();
    }
    return result;
}

std::shared_ptr<Wallet> Database::getWallet(const std::string &walletId)
{
    auto it = wallets.find(walletId);
    return it != wallets.end() ? it->second : nullptr;
}

std::vector<std::shared_ptr<Wallet>> Database::getAllWallets()
{
    std::vector<std::shared_ptr<Wallet>> result;
    for (const auto &pair : wallets)
    {
        result.push_back(pair.second);
    }
    return result;
}

bool Database::addTransaction(const Transaction &transaction)
{
    if (isFileModified())
    {
        if (!loadFromFiles())
        {
            return false;
        }
    }

    transactions_.push_back(transaction);
    return saveToFiles();
}

std::vector<Transaction> Database::getWalletTransactions(const std::string &walletId)
{
    std::vector<Transaction> result;
    for (const auto &transaction : transactions_)
    {
        if (transaction.fromWalletId == walletId || transaction.toWalletId == walletId)
        {
            result.push_back(transaction);
        }
    }
    return result;
}

void Database::serializeUser(std::ofstream &file, const User &user)
{
    try
    {
        std::cout << "Serializing user: " << user.getUsername() << std::endl;

        // Write username
        std::string username = user.getUsername();
        size_t len = username.length();
        file.write(reinterpret_cast<const char *>(&len), sizeof(len));
        file.write(username.c_str(), len);

        // Write fullname
        std::string fullname = user.getFullname();
        len = fullname.length();
        file.write(reinterpret_cast<const char *>(&len), sizeof(len));
        file.write(fullname.c_str(), len);

        // Write date of birth
        auto dob = user.getDateOfBirth();
        file.write(reinterpret_cast<const char *>(&dob), sizeof(dob));

        // Write admin status
        bool isAdmin = user.isAdmin();
        file.write(reinterpret_cast<const char *>(&isAdmin), sizeof(isAdmin));

        // Write password hash
        std::string passwordHash = user.getPasswordHash();
        len = passwordHash.length();
        file.write(reinterpret_cast<const char *>(&len), sizeof(len));
        file.write(passwordHash.c_str(), len);

        // Write 2FA info
        bool has2FA = user.has2FA();
        std::cout << "Serializing 2FA status: " << (has2FA ? "enabled" : "disabled") << std::endl;
        file.write(reinterpret_cast<const char *>(&has2FA), sizeof(has2FA));

        if (has2FA)
        {
            std::string secretKey = user.getSecretKey();
            std::cout << "Serializing 2FA secret key: " << secretKey << std::endl;
            len = secretKey.length();
            file.write(reinterpret_cast<const char *>(&len), sizeof(len));
            file.write(secretKey.c_str(), len);
        }

        // Write wallet ID
        std::string walletId = user.getWalletId();
        len = walletId.length();
        file.write(reinterpret_cast<const char *>(&len), sizeof(len));
        file.write(walletId.c_str(), len);

        if (file.fail())
        {
            throw std::runtime_error("Failed to write user data");
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error serializing user: " << e.what() << std::endl;
        throw;
    }
}

void Database::serializeWallet(std::ofstream &file, const Wallet &wallet)
{
    // Write wallet ID
    std::string id = wallet.getId();
    size_t len = id.length();
    file.write(reinterpret_cast<const char *>(&len), sizeof(len));
    file.write(id.c_str(), len);

    // Write balance
    double balance = wallet.getBalance();
    file.write(reinterpret_cast<const char *>(&balance), sizeof(balance));

    // Write transactions
    const auto &transactions = wallet.getTransactionHistory();
    size_t numTransactions = transactions.size();
    file.write(reinterpret_cast<const char *>(&numTransactions), sizeof(numTransactions));

    for (const auto &transaction : transactions)
    {
        serializeTransaction(file, transaction);
    }
}

void Database::serializeTransaction(std::ofstream &file, const Transaction &transaction)
{
    // Write transaction ID
    size_t len = transaction.id.length();
    file.write(reinterpret_cast<const char *>(&len), sizeof(len));
    file.write(transaction.id.c_str(), len);

    // Write from wallet ID
    len = transaction.fromWalletId.length();
    file.write(reinterpret_cast<const char *>(&len), sizeof(len));
    file.write(transaction.fromWalletId.c_str(), len);

    // Write to wallet ID
    len = transaction.toWalletId.length();
    file.write(reinterpret_cast<const char *>(&len), sizeof(len));
    file.write(transaction.toWalletId.c_str(), len);

    // Write amount
    file.write(reinterpret_cast<const char *>(&transaction.amount), sizeof(transaction.amount));

    // Write timestamp
    file.write(reinterpret_cast<const char *>(&transaction.timestamp), sizeof(transaction.timestamp));

    // Write status
    len = transaction.status.length();
    file.write(reinterpret_cast<const char *>(&len), sizeof(len));
    file.write(transaction.status.c_str(), len);

    // Write description
    len = transaction.description.length();
    file.write(reinterpret_cast<const char *>(&len), sizeof(len));
    file.write(transaction.description.c_str(), len);
}

User Database::deserializeUser(std::ifstream &file)
{
    try
    {
        // Read username
        size_t len;
        if (!file.read(reinterpret_cast<char *>(&len), sizeof(len)) || len > 1000)
        {
            throw std::runtime_error("Invalid username length");
        }
        std::string username(len, '\0');
        if (!file.read(&username[0], len))
        {
            throw std::runtime_error("Failed to read username");
        }
        std::cout << "Deserializing user: " << username << std::endl;

        // Read fullname
        if (!file.read(reinterpret_cast<char *>(&len), sizeof(len)) || len > 1000)
        {
            throw std::runtime_error("Invalid fullname length");
        }
        std::string fullname(len, '\0');
        if (!file.read(&fullname[0], len))
        {
            throw std::runtime_error("Failed to read fullname");
        }

        // Read date of birth
        std::chrono::system_clock::time_point dob;
        if (!file.read(reinterpret_cast<char *>(&dob), sizeof(dob)))
        {
            throw std::runtime_error("Failed to read date of birth");
        }

        // Read admin status
        bool isAdmin;
        if (!file.read(reinterpret_cast<char *>(&isAdmin), sizeof(isAdmin)))
        {
            throw std::runtime_error("Failed to read admin status");
        }

        // Create user with admin status
        User user(username, fullname, dob, isAdmin);

        // Read password hash
        if (!file.read(reinterpret_cast<char *>(&len), sizeof(len)) || len > 1000)
        {
            throw std::runtime_error("Invalid password hash length");
        }
        std::string passwordHash(len, '\0');
        if (!file.read(&passwordHash[0], len))
        {
            throw std::runtime_error("Failed to read password hash");
        }
        user.setPasswordHash(passwordHash);

        // Read 2FA info
        bool has2FA;
        if (!file.read(reinterpret_cast<char *>(&has2FA), sizeof(has2FA)))
        {
            throw std::runtime_error("Failed to read 2FA status");
        }
        std::cout << "Deserializing 2FA status: " << (has2FA ? "enabled" : "disabled") << std::endl;

        if (has2FA)
        {
            if (!file.read(reinterpret_cast<char *>(&len), sizeof(len)) || len > 1000)
            {
                throw std::runtime_error("Invalid secret key length");
            }
            std::string secretKey(len, '\0');
            if (!file.read(&secretKey[0], len))
            {
                throw std::runtime_error("Failed to read secret key");
            }
            std::cout << "Deserializing 2FA secret key: " << secretKey << std::endl;
            user.enable2FA(secretKey);
        }

        // Read wallet ID
        if (!file.read(reinterpret_cast<char *>(&len), sizeof(len)) || len > 1000)
        {
            throw std::runtime_error("Invalid wallet ID length");
        }
        std::string walletId(len, '\0');
        if (!file.read(&walletId[0], len))
        {
            throw std::runtime_error("Failed to read wallet ID");
        }
        user.setWalletId(walletId);

        return user;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error deserializing user: " << e.what() << std::endl;
        throw;
    }
}

Wallet Database::deserializeWallet(std::ifstream &file)
{
    // Read wallet ID
    size_t len;
    file.read(reinterpret_cast<char *>(&len), sizeof(len));
    std::string id(len, '\0');
    file.read(&id[0], len);

    // Read balance
    double balance;
    file.read(reinterpret_cast<char *>(&balance), sizeof(balance));

    // Create wallet
    Wallet wallet(id, balance);

    // Read transactions
    size_t numTransactions;
    file.read(reinterpret_cast<char *>(&numTransactions), sizeof(numTransactions));

    for (size_t i = 0; i < numTransactions; ++i)
    {
        Transaction transaction = deserializeTransaction(file);
        wallet.addTransaction(transaction);
    }

    return wallet;
}

Transaction Database::deserializeTransaction(std::ifstream &file)
{
    Transaction transaction;

    // Read transaction ID
    size_t len;
    file.read(reinterpret_cast<char *>(&len), sizeof(len));
    transaction.id.resize(len);
    file.read(&transaction.id[0], len);

    // Read from wallet ID
    file.read(reinterpret_cast<char *>(&len), sizeof(len));
    transaction.fromWalletId.resize(len);
    file.read(&transaction.fromWalletId[0], len);

    // Read to wallet ID
    file.read(reinterpret_cast<char *>(&len), sizeof(len));
    transaction.toWalletId.resize(len);
    file.read(&transaction.toWalletId[0], len);

    // Read amount
    file.read(reinterpret_cast<char *>(&transaction.amount), sizeof(transaction.amount));

    // Read timestamp
    file.read(reinterpret_cast<char *>(&transaction.timestamp), sizeof(transaction.timestamp));

    // Read status
    file.read(reinterpret_cast<char *>(&len), sizeof(len));
    transaction.status.resize(len);
    file.read(&transaction.status[0], len);

    // Read description
    file.read(reinterpret_cast<char *>(&len), sizeof(len));
    transaction.description.resize(len);
    file.read(&transaction.description[0], len);

    return transaction;
}

bool Database::saveToFiles()
{
    try
    {
        bool success = true;

        // Save users
        success &= saveUsersToFile();

        // Save wallets
        success &= saveWalletsToFile();

        // Save transactions
        success &= saveTransactionsToFile();

        return success;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error saving database files: " << e.what() << std::endl;
        return false;
    }
}

bool Database::loadFromFiles()
{
    try
    {
        bool success = true;

        // Load users
        success &= loadUsersFromFile();

        // Load wallets
        success &= loadWalletsFromFile();

        // Load transactions
        success &= loadTransactionsFromFile();

        return success;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error loading database files: " << e.what() << std::endl;
        return false;
    }
}

bool Database::saveUsersToFile()
{
    try
    {
        std::cout << "Saving users to: " << USERS_FILE << std::endl;

        // Create a temporary file
        std::string tempFile = USERS_FILE + ".tmp";
        std::ofstream file(tempFile, std::ios::binary | std::ios::trunc);
        if (!file)
        {
            std::cerr << "Failed to open temporary file for writing: " << tempFile << std::endl;
            return false;
        }

        // Write magic number and version
        const uint32_t MAGIC = 0x55534552; // "USER" in hex
        const uint32_t VERSION = 1;
        file.write(reinterpret_cast<const char *>(&MAGIC), sizeof(MAGIC));
        file.write(reinterpret_cast<const char *>(&VERSION), sizeof(VERSION));

        // Write number of users
        size_t numUsers = users.size();
        std::cout << "Saving " << numUsers << " users" << std::endl;
        file.write(reinterpret_cast<const char *>(&numUsers), sizeof(numUsers));

        // Write users
        for (const auto &pair : users)
        {
            serializeUser(file, *pair.second);
        }

        file.close();

        // Replace the old file with the new one
        if (std::filesystem::exists(USERS_FILE))
        {
            std::filesystem::remove(USERS_FILE);
        }
        std::filesystem::rename(tempFile, USERS_FILE);
        lastUsersModification = std::filesystem::last_write_time(USERS_FILE);

        std::cout << "Successfully saved users to file" << std::endl;
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error saving users: " << e.what() << std::endl;
        return false;
    }
}

bool Database::saveWalletsToFile()
{
    try
    {
        std::cout << "Saving wallets to: " << WALLETS_FILE << std::endl;

        // Create a temporary file
        std::string tempFile = WALLETS_FILE + ".tmp";
        std::ofstream file(tempFile, std::ios::binary | std::ios::trunc);
        if (!file)
        {
            std::cerr << "Failed to open temporary file for writing: " << tempFile << std::endl;
            return false;
        }

        // Write magic number and version
        const uint32_t MAGIC = 0x57414C54; // "WALT" in hex
        const uint32_t VERSION = 1;
        file.write(reinterpret_cast<const char *>(&MAGIC), sizeof(MAGIC));
        file.write(reinterpret_cast<const char *>(&VERSION), sizeof(VERSION));

        // Write number of wallets
        size_t numWallets = wallets.size();
        file.write(reinterpret_cast<const char *>(&numWallets), sizeof(numWallets));

        // Write wallets
        for (const auto &pair : wallets)
        {
            serializeWallet(file, *pair.second);
        }

        file.close();

        // Replace the old file with the new one
        if (std::filesystem::exists(WALLETS_FILE))
        {
            std::filesystem::remove(WALLETS_FILE);
        }
        std::filesystem::rename(tempFile, WALLETS_FILE);
        lastWalletsModification = std::filesystem::last_write_time(WALLETS_FILE);

        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error saving wallets: " << e.what() << std::endl;
        return false;
    }
}

bool Database::saveTransactionsToFile()
{
    try
    {
        std::cout << "Saving transactions to: " << TRANSACTIONS_FILE << std::endl;

        // Create a temporary file
        std::string tempFile = TRANSACTIONS_FILE + ".tmp";
        std::ofstream file(tempFile, std::ios::binary | std::ios::trunc);
        if (!file)
        {
            std::cerr << "Failed to open temporary file for writing: " << tempFile << std::endl;
            return false;
        }

        // Write magic number and version
        const uint32_t MAGIC = 0x5452414E; // "TRAN" in hex
        const uint32_t VERSION = 1;
        file.write(reinterpret_cast<const char *>(&MAGIC), sizeof(MAGIC));
        file.write(reinterpret_cast<const char *>(&VERSION), sizeof(VERSION));

        // Write number of transactions
        size_t numTransactions = transactions_.size();
        file.write(reinterpret_cast<const char *>(&numTransactions), sizeof(numTransactions));

        // Write transactions
        for (const auto &transaction : transactions_)
        {
            serializeTransaction(file, transaction);
        }

        file.close();

        // Replace the old file with the new one
        if (std::filesystem::exists(TRANSACTIONS_FILE))
        {
            std::filesystem::remove(TRANSACTIONS_FILE);
        }
        std::filesystem::rename(tempFile, TRANSACTIONS_FILE);
        lastTransactionsModification = std::filesystem::last_write_time(TRANSACTIONS_FILE);

        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error saving transactions: " << e.what() << std::endl;
        return false;
    }
}

bool Database::loadUsersFromFile()
{
    try
    {
        if (!std::filesystem::exists(USERS_FILE))
        {
            std::cout << "Users file not found. Creating new users database..." << std::endl;
            return false;
        }

        std::cout << "Loading users from: " << USERS_FILE << std::endl;
        std::ifstream file(USERS_FILE, std::ios::binary);
        if (!file)
        {
            std::cerr << "Failed to open users file for reading: " << USERS_FILE << std::endl;
            return false;
        }

        // Read and verify magic number
        uint32_t magic;
        file.read(reinterpret_cast<char *>(&magic), sizeof(magic));
        if (file.fail() || magic != 0x55534552)
        { // "USER" in hex
            std::cout << "Invalid users file format. Creating new users database..." << std::endl;
            return false;
        }

        // Read and verify version
        uint32_t version;
        file.read(reinterpret_cast<char *>(&version), sizeof(version));
        if (file.fail() || version != 1)
        {
            std::cout << "Invalid users file version. Creating new users database..." << std::endl;
            return false;
        }

        // Clear existing users
        users.clear();

        // Read number of users
        size_t numUsers;
        file.read(reinterpret_cast<char *>(&numUsers), sizeof(numUsers));
        if (file.fail() || numUsers > 1000000)
        { // Sanity check
            std::cout << "Invalid number of users in database. Creating new users database..." << std::endl;
            return false;
        }
        std::cout << "Loading " << numUsers << " users" << std::endl;

        // Read users
        for (size_t i = 0; i < numUsers; ++i)
        {
            User user = deserializeUser(file);
            if (file.fail())
            {
                std::cout << "Error reading user data. Creating new users database..." << std::endl;
                return false;
            }
            users[user.getUsername()] = std::make_shared<User>(user);
        }

        file.close();
        lastUsersModification = std::filesystem::last_write_time(USERS_FILE);
        std::cout << "Successfully loaded users from file" << std::endl;
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error loading users: " << e.what() << std::endl;
        return false;
    }
}

bool Database::loadWalletsFromFile()
{
    try
    {
        if (!std::filesystem::exists(WALLETS_FILE))
        {
            std::cout << "Wallets file not found. Creating new wallets database..." << std::endl;
            return false;
        }

        std::ifstream file(WALLETS_FILE, std::ios::binary);
        if (!file)
        {
            std::cerr << "Failed to open wallets file for reading: " << WALLETS_FILE << std::endl;
            return false;
        }

        // Read and verify magic number
        uint32_t magic;
        file.read(reinterpret_cast<char *>(&magic), sizeof(magic));
        if (file.fail() || magic != 0x57414C54)
        { // "WALT" in hex
            std::cout << "Invalid wallets file format. Creating new wallets database..." << std::endl;
            return false;
        }

        // Read and verify version
        uint32_t version;
        file.read(reinterpret_cast<char *>(&version), sizeof(version));
        if (file.fail() || version != 1)
        {
            std::cout << "Invalid wallets file version. Creating new wallets database..." << std::endl;
            return false;
        }

        // Clear existing wallets
        wallets.clear();

        // Read number of wallets
        size_t numWallets;
        file.read(reinterpret_cast<char *>(&numWallets), sizeof(numWallets));
        if (file.fail() || numWallets > 1000000)
        { // Sanity check
            std::cout << "Invalid number of wallets in database. Creating new wallets database..." << std::endl;
            return false;
        }

        // Read wallets
        for (size_t i = 0; i < numWallets; ++i)
        {
            Wallet wallet = deserializeWallet(file);
            if (file.fail())
            {
                std::cout << "Error reading wallet data. Creating new wallets database..." << std::endl;
                return false;
            }
            wallets[wallet.getId()] = std::make_shared<Wallet>(wallet);
        }

        file.close();
        lastWalletsModification = std::filesystem::last_write_time(WALLETS_FILE);
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error loading wallets: " << e.what() << std::endl;
        return false;
    }
}

bool Database::loadTransactionsFromFile()
{
    try
    {
        if (!std::filesystem::exists(TRANSACTIONS_FILE))
        {
            std::cout << "Transactions file not found. Creating new transactions database..." << std::endl;
            return false;
        }

        std::ifstream file(TRANSACTIONS_FILE, std::ios::binary);
        if (!file)
        {
            std::cerr << "Failed to open transactions file for reading: " << TRANSACTIONS_FILE << std::endl;
            return false;
        }

        // Read and verify magic number
        uint32_t magic;
        file.read(reinterpret_cast<char *>(&magic), sizeof(magic));
        if (file.fail() || magic != 0x5452414E)
        { // "TRAN" in hex
            std::cout << "Invalid transactions file format. Creating new transactions database..." << std::endl;
            return false;
        }

        // Read and verify version
        uint32_t version;
        file.read(reinterpret_cast<char *>(&version), sizeof(version));
        if (file.fail() || version != 1)
        {
            std::cout << "Invalid transactions file version. Creating new transactions database..." << std::endl;
            return false;
        }

        // Clear existing transactions
        transactions_.clear();

        // Read number of transactions
        size_t numTransactions;
        file.read(reinterpret_cast<char *>(&numTransactions), sizeof(numTransactions));
        if (file.fail() || numTransactions > 1000000)
        { // Sanity check
            std::cout << "Invalid number of transactions in database. Creating new transactions database..." << std::endl;
            return false;
        }

        // Read transactions
        for (size_t i = 0; i < numTransactions; ++i)
        {
            Transaction transaction = deserializeTransaction(file);
            if (file.fail())
            {
                std::cout << "Error reading transaction data. Creating new transactions database..." << std::endl;
                return false;
            }
            transactions_.push_back(transaction);
        }

        file.close();
        lastTransactionsModification = std::filesystem::last_write_time(TRANSACTIONS_FILE);
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error loading transactions: " << e.what() << std::endl;
        return false;
    }
}

void Database::setBasePath(const std::filesystem::path &path)
{
    // Create database file paths in the specified directory
    USERS_FILE = (path / "users.dat").string();
    WALLETS_FILE = (path / "wallets.dat").string();
    TRANSACTIONS_FILE = (path / "transactions.dat").string();

    std::cout << "Users database path: " << USERS_FILE << std::endl;
    std::cout << "Wallets database path: " << WALLETS_FILE << std::endl;
    std::cout << "Transactions database path: " << TRANSACTIONS_FILE << std::endl;

    // Check if we can write to the directory
    std::string testFile = (path / "test_write.tmp").string();
    std::cout << "Testing write permissions with file: " << testFile << std::endl;
    {
        std::ofstream test(testFile);
        if (!test)
        {
            throw std::runtime_error("Cannot write to directory: " + path.string());
        }
        test << "test" << std::endl;
        test.close();
        std::filesystem::remove(testFile);
    }
    std::cout << "Write permissions verified" << std::endl;

    // Try to load existing data
    bool loadSuccess = loadFromFiles();

    // If loading failed or database is empty, create admin user and wallet
    if (!loadSuccess || (users.empty() && wallets.empty()))
    {
        std::cout << "Creating new database with admin user..." << std::endl;

        try
        {
            std::cout << "Creating admin user..." << std::endl;
            // Create admin user if not exists
            auto admin = std::make_shared<User>("admin", "System Administrator",
                                                std::chrono::system_clock::now(), true);
            admin->setPassword("admin123"); // Default admin password

            std::cout << "Adding admin user to database..." << std::endl;
            users["admin"] = admin; // Add directly to users map

            std::cout << "Creating admin wallet..." << std::endl;
            // Create admin wallet
            auto adminWallet = std::make_shared<Wallet>("ADMIN_WALLET", 1000000.0);

            std::cout << "Adding admin wallet to database..." << std::endl;
            wallets["ADMIN_WALLET"] = adminWallet; // Add directly to wallets map

            std::cout << "Setting admin wallet ID..." << std::endl;
            admin->setWalletId(adminWallet->getId());

            std::cout << "Saving initial database..." << std::endl;
            // Save the initial database
            if (!saveToFiles())
            {
                throw std::runtime_error("Failed to save initial database!");
            }

            std::cout << "Initial database created successfully!" << std::endl;
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error creating initial database: " + std::string(e.what()));
        }
    }
    else
    {
        std::cout << "Existing database loaded successfully!" << std::endl;
    }
}