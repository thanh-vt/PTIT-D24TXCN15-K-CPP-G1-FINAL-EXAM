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

// Ham luu database ra file nhi phan
bool Database::saveToFile() { // Luu database voi checksum
    try {
        std::cerr << "Saving database to file: " << DB_FILE_PATH_ << std::endl;

        std::string tempFile = DB_FILE_PATH_ + ".tmp";
        std::ostringstream dataStream;

        uint32_t magic = MAGIC_NUMBER;
        uint32_t version = VERSION;
        dataStream.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
        dataStream.write(reinterpret_cast<const char*>(&version), sizeof(version));

        size_t userCount = users_.size();
        size_t walletCount = wallets_.size();
        size_t transactionCount = central_transactions_log_.size();

        dataStream.write(reinterpret_cast<const char*>(&userCount), sizeof(userCount));
        dataStream.write(reinterpret_cast<const char*>(&walletCount), sizeof(walletCount));
        dataStream.write(reinterpret_cast<const char*>(&transactionCount), sizeof(transactionCount));

        // Serialize admin users
        for (const auto& pair : users_) {
            serializeAdminUser(dataStream, *pair.second);
        }

        // Serialize admin wallets
        for (const auto& pair : wallets_) {
            serializeAdminWallet(dataStream, *pair.second);
        }

        // Serialize admin transactions
        for (const auto& transaction : central_transactions_log_) {
            serializeAdminTransaction(dataStream, transaction);
        }

        std::string data = dataStream.str();
        uint32_t checksum = calculateCRC32(data.c_str(), data.length());

        std::ofstream file(tempFile, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open temporary file for writing" << std::endl;
            return false;
        }

        file.write(data.c_str(), data.length());
        file.write(reinterpret_cast<const char*>(&checksum), sizeof(checksum));
        file.close();

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

bool Database::loadFromFile() { // Tai database tu file voi kiem tra checksum
    try {
        if (!std::filesystem::exists(DB_FILE_PATH_)) {
            std::cerr << "Database file does not exist: " << DB_FILE_PATH_ << std::endl;
            return false;
        }

        std::cerr << "Loading database from file: " << DB_FILE_PATH_ << std::endl;

        std::ifstream file(DB_FILE_PATH_, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open database file for reading" << std::endl;
            return false;
        }

        // Read entire file except checksum
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        if (fileSize < sizeof(uint32_t)) {
            std::cerr << "Database file too small" << std::endl;
            return false;
        }

        size_t dataSize = fileSize - sizeof(uint32_t);
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(dataSize);
        file.read(buffer.data(), dataSize);

        // Read stored checksum
        uint32_t storedChecksum;
        file.read(reinterpret_cast<char*>(&storedChecksum), sizeof(storedChecksum));
        file.close();

        // Verify checksum
        uint32_t calculatedChecksum = calculateCRC32(buffer.data(), dataSize);
        if (calculatedChecksum != storedChecksum) {
            std::cerr << "Database checksum mismatch! Data corrupted." << std::endl;
            return false;
        }

        // Parse data
        std::istringstream dataStream(std::string(buffer.begin(), buffer.end()));

        // Read and verify magic number and version
        uint32_t magic, version;
        dataStream.read(reinterpret_cast<char*>(&magic), sizeof(magic));
        dataStream.read(reinterpret_cast<char*>(&version), sizeof(version));

        if (magic != MAGIC_NUMBER) {
            std::cerr << "Invalid database file format" << std::endl;
            return false;
        }

        if (version != VERSION) {
            std::cerr << "Unsupported database version: " << version << std::endl;
            return false;
        }

        // Read data counts
        size_t userCount, walletCount, transactionCount;
        dataStream.read(reinterpret_cast<char*>(&userCount), sizeof(userCount));
        dataStream.read(reinterpret_cast<char*>(&walletCount), sizeof(walletCount));
        dataStream.read(reinterpret_cast<char*>(&transactionCount), sizeof(transactionCount));

        // Deserialize admin users
        for (size_t i = 0; i < userCount; ++i) {
            AdminUser user = deserializeAdminUser(dataStream);
            users_[user.username] = std::make_shared<AdminUser>(user);
        }

        // Deserialize admin wallets
        for (size_t i = 0; i < walletCount; ++i) {
            AdminWallet wallet = deserializeAdminWallet(dataStream);
            wallets_[wallet.wallet_id] = std::make_shared<AdminWallet>(wallet);
        }

        // Deserialize admin transactions
        for (size_t i = 0; i < transactionCount; ++i) {
            AdminTransaction transaction = deserializeAdminTransaction(dataStream);
            central_transactions_log_.push_back(transaction);
        }

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

// Ham sao luu va phuc hoi database
bool Database::backupDatabase(const std::string& backupDirectory) const { // Sao luu database
    try {
        if (!std::filesystem::exists(DB_FILE_PATH_)) {
            std::cerr << "Database file does not exist, cannot backup" << std::endl;
            return false;
        }

        // Create backup directory if it doesn't exist
        if (!std::filesystem::exists(backupDirectory)) {
            std::filesystem::create_directories(backupDirectory);
        }

        // Generate backup filename with timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "database_backup_%Y%m%d_%H%M%S.dat");

        std::string backupFilePath = backupDirectory + "/" + ss.str();

        // Copy database file to backup location
        std::filesystem::copy_file(DB_FILE_PATH_, backupFilePath);

        std::cerr << "Database backed up successfully to: " << backupFilePath << std::endl;
        return true;

    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error during backup: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error backing up database: " << e.what() << std::endl;
        return false;
    }
}

bool Database::restoreDatabase(const std::string& backupFilePath) { // Phuc hoi database
    try {
        if (!std::filesystem::exists(backupFilePath)) {
            std::cerr << "Backup file does not exist: " << backupFilePath << std::endl;
            return false;
        }

        std::cerr << "WARNING: This will overwrite current database!" << std::endl;
        std::cerr << "Restoring database from: " << backupFilePath << std::endl;

        // Create temporary restore file
        std::string tempRestoreFile = DB_FILE_PATH_ + ".restore_tmp";

        // Copy backup to temporary file
        std::filesystem::copy_file(backupFilePath, tempRestoreFile);

        // Clear current data
        clearAllData();

        // Replace current database file
        if (std::filesystem::exists(DB_FILE_PATH_)) {
            std::filesystem::remove(DB_FILE_PATH_);
        }
        std::filesystem::rename(tempRestoreFile, DB_FILE_PATH_);

        // Load the restored database
        if (!loadFromFile()) {
            std::cerr << "Failed to load restored database file!" << std::endl;
            return false;
        }

        std::cerr << "Database restored successfully" << std::endl;
        return true;

    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error during restore: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error restoring database: " << e.what() << std::endl;
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

// Simple CRC32 implementation for data integrity
uint32_t Database::calculateCRC32(const char* data, size_t length) const {
    // CRC32 polynomial: 0xEDB88320
    static const uint32_t crc32_table[256] = {
        0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
        0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
        0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
        0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
        0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
        0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
        0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
        0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
        0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
        0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
        0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
        0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
        0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
        0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
        0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
        0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
        0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
        0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
        0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
        0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
        0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
        0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
        0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
        0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
        0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
        0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
        0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
        0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
        0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
        0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
        0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
        0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
        0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
        0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
        0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
        0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
        0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
        0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
        0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
        0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
        0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
        0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
        0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
    };

    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; i++) {
        crc = crc32_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}

// String and time serialization helpers
void Database::writeString(std::ofstream& file, const std::string& str) const {
    size_t length = str.length();
    file.write(reinterpret_cast<const char*>(&length), sizeof(length));
    file.write(str.c_str(), length);
}

std::string Database::readString(std::ifstream& file) const {
    size_t length;
    file.read(reinterpret_cast<char*>(&length), sizeof(length));
    std::string str(length, '\0');
    file.read(&str[0], length);
    return str;
}

void Database::writeTimePoint(std::ofstream& file, const std::chrono::system_clock::time_point& timePoint) const {
    auto duration = timePoint.time_since_epoch();
    auto count = duration.count();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
}

std::chrono::system_clock::time_point Database::readTimePoint(std::ifstream& file) const {
    std::chrono::system_clock::duration::rep count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    std::chrono::system_clock::duration duration(count);
    return std::chrono::system_clock::time_point(duration);
}

// Serialize/Deserialize methods for admin data
void Database::serializeAdminUser(std::ostream& file, const AdminUser& user) const {
    // For ostream, we need to use different approach
    std::string username = user.username;
    std::string password_hash = user.password_hash;
    std::string full_name = user.full_name;
    std::string birth_date = user.birth_date;

    size_t len = username.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(username.c_str(), len);

    len = password_hash.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(password_hash.c_str(), len);

    len = full_name.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(full_name.c_str(), len);

    len = birth_date.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(birth_date.c_str(), len);

    file.write(reinterpret_cast<const char*>(&user.is_admin), sizeof(user.is_admin));
    file.write(reinterpret_cast<const char*>(&user.is_2fa_enabled), sizeof(user.is_2fa_enabled));

    auto duration = user.created_at.time_since_epoch();
    auto count = duration.count();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));

    duration = user.last_login.time_since_epoch();
    count = duration.count();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
}

void Database::serializeAdminWallet(std::ostream& file, const AdminWallet& wallet) const {
    size_t len = wallet.wallet_id.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(wallet.wallet_id.c_str(), len);

    len = wallet.owner_username.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(wallet.owner_username.c_str(), len);

    file.write(reinterpret_cast<const char*>(&wallet.balance), sizeof(wallet.balance));

    auto duration = wallet.created_at.time_since_epoch();
    auto count = duration.count();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));

    duration = wallet.last_transaction_time.time_since_epoch();
    count = duration.count();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
}

void Database::serializeAdminTransaction(std::ostream& file, const AdminTransaction& transaction) const {
    size_t len = transaction.transaction_id.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(transaction.transaction_id.c_str(), len);

    len = transaction.from_wallet.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(transaction.from_wallet.c_str(), len);

    len = transaction.to_wallet.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(transaction.to_wallet.c_str(), len);

    file.write(reinterpret_cast<const char*>(&transaction.amount), sizeof(transaction.amount));

    len = transaction.description.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(transaction.description.c_str(), len);

    len = transaction.status.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(transaction.status.c_str(), len);

    auto duration = transaction.created_at.time_since_epoch();
    auto count = duration.count();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
}

AdminUser Database::deserializeAdminUser(std::istream& file) const {
    size_t len;
    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string username(len, '\0');
    file.read(&username[0], len);

    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string password_hash(len, '\0');
    file.read(&password_hash[0], len);

    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string full_name(len, '\0');
    file.read(&full_name[0], len);

    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string birth_date(len, '\0');
    file.read(&birth_date[0], len);

    bool is_admin, is_2fa_enabled;
    file.read(reinterpret_cast<char*>(&is_admin), sizeof(is_admin));
    file.read(reinterpret_cast<char*>(&is_2fa_enabled), sizeof(is_2fa_enabled));

    std::chrono::system_clock::duration::rep count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    std::chrono::system_clock::duration duration1(count);
    auto created_at = std::chrono::system_clock::time_point(duration1);

    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    std::chrono::system_clock::duration duration2(count);
    auto last_login = std::chrono::system_clock::time_point(duration2);

    AdminUser user(username, password_hash, full_name, birth_date, is_admin);
    user.is_2fa_enabled = is_2fa_enabled;
    user.created_at = created_at;
    user.last_login = last_login;

    return user;
}

AdminWallet Database::deserializeAdminWallet(std::istream& file) const {
    size_t len;
    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string wallet_id(len, '\0');
    file.read(&wallet_id[0], len);

    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string owner_username(len, '\0');
    file.read(&owner_username[0], len);

    double balance;
    file.read(reinterpret_cast<char*>(&balance), sizeof(balance));

    std::chrono::system_clock::duration::rep count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    std::chrono::system_clock::duration duration1(count);
    auto created_at = std::chrono::system_clock::time_point(duration1);

    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    std::chrono::system_clock::duration duration2(count);
    auto last_transaction_time = std::chrono::system_clock::time_point(duration2);

    AdminWallet wallet(wallet_id, owner_username, balance);
    wallet.created_at = created_at;
    wallet.last_transaction_time = last_transaction_time;

    return wallet;
}

AdminTransaction Database::deserializeAdminTransaction(std::istream& file) const {
    size_t len;
    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string transaction_id(len, '\0');
    file.read(&transaction_id[0], len);

    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string from_wallet(len, '\0');
    file.read(&from_wallet[0], len);

    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string to_wallet(len, '\0');
    file.read(&to_wallet[0], len);

    double amount;
    file.read(reinterpret_cast<char*>(&amount), sizeof(amount));

    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string description(len, '\0');
    file.read(&description[0], len);

    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string status(len, '\0');
    file.read(&status[0], len);

    std::chrono::system_clock::duration::rep count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    std::chrono::system_clock::duration duration(count);
    auto created_at = std::chrono::system_clock::time_point(duration);

    AdminTransaction transaction(transaction_id, from_wallet, to_wallet, amount, description, status);
    transaction.created_at = created_at;

    return transaction;
}

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