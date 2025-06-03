#include "../include/database.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <filesystem>

Database* Database::instance = nullptr;

// Helper function to check if any database file has been modified
bool Database::isFileModified() {
    try {
        bool modified = false;

        // Check users file
        if (std::filesystem::exists(USERS_FILE)) {
            auto lastWriteTime = std::filesystem::last_write_time(USERS_FILE);
            if (lastWriteTime != lastUsersModification) {
                lastUsersModification = lastWriteTime;
                modified = true;
            }
        }

        // Check wallets file
        // TODO check wallet file

        // Check transactions file
        // TODO check transactions file

        return modified;
    } catch (const std::exception& e) {
        std::cerr << "Critical error in isFileModified: " << e.what() << std::endl;
        throw;
    }
}

Database& Database::getInstance() {
    if (!instance) {
        instance = new Database();
    }
    return *instance;
}

Database::Database() {
    std::cout << "Database constructor started..." << std::endl;
    // TODO init database
}

Database::~Database() {
    std::cout << "Database destructor called..." << std::endl;
    saveToFiles();
}

bool Database::addUser(const User& user) {
    std::cout << "Adding user: " << user.getUsername() << std::endl;

    // Only check for file modifications if we're not in initialization
    if (!users.empty() && isFileModified()) {
        std::cout << "Database file was modified, reloading..." << std::endl;
        if (!loadFromFiles()) {
            std::cerr << "Failed to reload database" << std::endl;
            return false;
        }
    }

    auto username = user.getUsername();
    if (users.find(username) != users.end()) {
        std::cout << "User already exists: " << username << std::endl;
        return false;
    }

    // Add the user to the in-memory map
    users[username] = std::make_shared<User>(user);
    std::cout << "User added to memory: " << username << std::endl;

    // Save changes to file
    if (!saveToFiles()) {
        std::cerr << "Failed to save database after adding user" << std::endl;
        // Remove the user from memory if save failed
        users.erase(username);
        return false;
    }

    std::cout << "User successfully added and saved: " << username << std::endl;
    return true;
}

bool Database::updateUser(const User& user) {
    if (isFileModified()) {
        if (!loadFromFiles()) {
            return false;
        }
    }

    auto username = user.getUsername();
    if (users.find(username) == users.end()) {
        return false;
    }
    users[username] = std::make_shared<User>(user);
    return saveToFiles();
}

bool Database::deleteUser(const std::string& username) {
    if (isFileModified()) {
        if (!loadFromFiles()) {
            return false;
        }
    }

    // Get the user's wallet ID before deleting the user
    auto user = getUser(username);
    if (!user) {
        return false;
    }
    std::string walletId = user->getWalletId();

    // Delete the user
    bool result = users.erase(username) > 0;
    if (result) {
        // Delete the associated wallet
        // TODO delete wallets
        saveToFiles();
    }
    return result;
}

std::shared_ptr<User> Database::getUser(const std::string& username) {
    auto it = users.find(username);
    return it != users.end() ? it->second : nullptr;
}

std::vector<std::shared_ptr<User>> Database::getAllUsers() {
    std::vector<std::shared_ptr<User>> result;
    for (const auto& pair : users) {
        result.push_back(pair.second);
    }
    return result;
}

void Database::serializeUser(std::ofstream& file, const User& user) {
    try {
        std::cout << "Serializing user: " << user.getUsername() << std::endl;

        // Write username
        std::string username = user.getUsername();
        size_t len = username.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(username.c_str(), len);

        // Write fullname
        std::string fullname = user.getFullname();
        len = fullname.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(fullname.c_str(), len);

        // Write date of birth
        auto dob = user.getDateOfBirth();
        file.write(reinterpret_cast<const char*>(&dob), sizeof(dob));

        // Write admin status
        bool isAdmin = user.isAdmin();
        file.write(reinterpret_cast<const char*>(&isAdmin), sizeof(isAdmin));

        // Write password hash
        std::string passwordHash = user.getPasswordHash();
        len = passwordHash.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(passwordHash.c_str(), len);

        // Write 2FA info
        bool has2FA = user.has2FA();
        std::cout << "Serializing 2FA status: " << (has2FA ? "enabled" : "disabled") << std::endl;
        file.write(reinterpret_cast<const char*>(&has2FA), sizeof(has2FA));

        if (has2FA) {
            std::string secretKey = user.getSecretKey();
            std::cout << "Serializing 2FA secret key: " << secretKey << std::endl;
            len = secretKey.length();
            file.write(reinterpret_cast<const char*>(&len), sizeof(len));
            file.write(secretKey.c_str(), len);
        }

        // Write wallet ID
        std::string walletId = user.getWalletId();
        len = walletId.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(walletId.c_str(), len);

        if (file.fail()) {
            throw std::runtime_error("Failed to write user data");
        }
    } catch (const std::exception& e) {
        std::cerr << "Error serializing user: " << e.what() << std::endl;
        throw;
    }
}

User Database::deserializeUser(std::ifstream& file) {
    try {
        // Read username
        size_t len;
        if (!file.read(reinterpret_cast<char*>(&len), sizeof(len)) || len > 1000) {
            throw std::runtime_error("Invalid username length");
        }
        std::string username(len, '\0');
        if (!file.read(&username[0], len)) {
            throw std::runtime_error("Failed to read username");
        }
        std::cout << "Deserializing user: " << username << std::endl;

        // Read fullname
        if (!file.read(reinterpret_cast<char*>(&len), sizeof(len)) || len > 1000) {
            throw std::runtime_error("Invalid fullname length");
        }
        std::string fullname(len, '\0');
        if (!file.read(&fullname[0], len)) {
            throw std::runtime_error("Failed to read fullname");
        }

        // Read date of birth
        std::chrono::system_clock::time_point dob;
        if (!file.read(reinterpret_cast<char*>(&dob), sizeof(dob))) {
            throw std::runtime_error("Failed to read date of birth");
        }

        // Read admin status
        bool isAdmin;
        if (!file.read(reinterpret_cast<char*>(&isAdmin), sizeof(isAdmin))) {
            throw std::runtime_error("Failed to read admin status");
        }

        // Create user with admin status
        User user(username, fullname, dob, isAdmin);

        // Read password hash
        if (!file.read(reinterpret_cast<char*>(&len), sizeof(len)) || len > 1000) {
            throw std::runtime_error("Invalid password hash length");
        }
        std::string passwordHash(len, '\0');
        if (!file.read(&passwordHash[0], len)) {
            throw std::runtime_error("Failed to read password hash");
        }
        user.setPasswordHash(passwordHash);

        // Read 2FA info
        bool has2FA;
        if (!file.read(reinterpret_cast<char*>(&has2FA), sizeof(has2FA))) {
            throw std::runtime_error("Failed to read 2FA status");
        }
        std::cout << "Deserializing 2FA status: " << (has2FA ? "enabled" : "disabled") << std::endl;

        if (has2FA) {
            if (!file.read(reinterpret_cast<char*>(&len), sizeof(len)) || len > 1000) {
                throw std::runtime_error("Invalid secret key length");
            }
            std::string secretKey(len, '\0');
            if (!file.read(&secretKey[0], len)) {
                throw std::runtime_error("Failed to read secret key");
            }
            std::cout << "Deserializing 2FA secret key: " << secretKey << std::endl;
            user.enable2FA(secretKey);
        }

        // Read wallet ID
        if (!file.read(reinterpret_cast<char*>(&len), sizeof(len)) || len > 1000) {
            throw std::runtime_error("Invalid wallet ID length");
        }
        std::string walletId(len, '\0');
        if (!file.read(&walletId[0], len)) {
            throw std::runtime_error("Failed to read wallet ID");
        }
        user.setWalletId(walletId);

        return user;
    } catch (const std::exception& e) {
        std::cerr << "Error deserializing user: " << e.what() << std::endl;
        throw;
    }
}

bool Database::saveToFiles() {
    // TODO save to files
    return true;
}

bool Database::loadFromFiles() {
    // TODO load from files
    return true;
}

bool Database::saveUsersToFile() {
    try {
        std::cout << "Saving users to: " << USERS_FILE << std::endl;

        // Create a temporary file
        std::string tempFile = USERS_FILE + ".tmp";
        std::ofstream file(tempFile, std::ios::binary | std::ios::trunc);
        if (!file) {
            std::cerr << "Failed to open temporary file for writing: " << tempFile << std::endl;
            return false;
        }

        // Write magic number and version
        const uint32_t MAGIC = 0x55534552;  // "USER" in hex
        const uint32_t VERSION = 1;
        file.write(reinterpret_cast<const char*>(&MAGIC), sizeof(MAGIC));
        file.write(reinterpret_cast<const char*>(&VERSION), sizeof(VERSION));

        // Write number of users
        size_t numUsers = users.size();
        std::cout << "Saving " << numUsers << " users" << std::endl;
        file.write(reinterpret_cast<const char*>(&numUsers), sizeof(numUsers));

        // Write users
        for (const auto& pair : users) {
            serializeUser(file, *pair.second);
        }

        file.close();

        // Replace the old file with the new one
        if (std::filesystem::exists(USERS_FILE)) {
            std::filesystem::remove(USERS_FILE);
        }
        std::filesystem::rename(tempFile, USERS_FILE);
        lastUsersModification = std::filesystem::last_write_time(USERS_FILE);

        std::cout << "Successfully saved users to file" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving users: " << e.what() << std::endl;
        return false;
    }
}

bool Database::loadUsersFromFile() {
    try {
        if (!std::filesystem::exists(USERS_FILE)) {
            std::cout << "Users file not found. Creating new users database..." << std::endl;
            return false;
        }

        std::cout << "Loading users from: " << USERS_FILE << std::endl;
        std::ifstream file(USERS_FILE, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open users file for reading: " << USERS_FILE << std::endl;
            return false;
        }

        // Read and verify magic number
        uint32_t magic;
        file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
        if (file.fail() || magic != 0x55534552) {  // "USER" in hex
            std::cout << "Invalid users file format. Creating new users database..." << std::endl;
            return false;
        }

        // Read and verify version
        uint32_t version;
        file.read(reinterpret_cast<char*>(&version), sizeof(version));
        if (file.fail() || version != 1) {
            std::cout << "Invalid users file version. Creating new users database..." << std::endl;
            return false;
        }

        // Clear existing users
        users.clear();

        // Read number of users
        size_t numUsers;
        file.read(reinterpret_cast<char*>(&numUsers), sizeof(numUsers));
        if (file.fail() || numUsers > 1000000) {  // Sanity check
            std::cout << "Invalid number of users in database. Creating new users database..." << std::endl;
            return false;
        }
        std::cout << "Loading " << numUsers << " users" << std::endl;

        // Read users
        for (size_t i = 0; i < numUsers; ++i) {
            User user = deserializeUser(file);
            if (file.fail()) {
                std::cout << "Error reading user data. Creating new users database..." << std::endl;
                return false;
            }
            users[user.getUsername()] = std::make_shared<User>(user);
        }

        file.close();
        lastUsersModification = std::filesystem::last_write_time(USERS_FILE);
        std::cout << "Successfully loaded users from file" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading users: " << e.what() << std::endl;
        return false;
    }
}
