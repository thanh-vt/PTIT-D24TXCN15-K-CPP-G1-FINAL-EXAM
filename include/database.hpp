#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <chrono>
#include <filesystem>
#include "user.hpp"
// TODO include wallet header
// TODO include transaction header

class Database {
private:
    static Database* instance;
    std::map<std::string, std::shared_ptr<User>> users;
    // TODO wallets
    // TODO transactions

    // File paths
    std::string USERS_FILE;
    // TODO  wallet file
    // TODO  transaction file

    // Last modification times
    std::filesystem::file_time_type lastUsersModification;
    // TODO last wallet modification
    // TODO last transaction modification

    // Private constructor for singleton
    Database();
    ~Database();

    // Helper functions
    bool isFileModified();
    void serializeUser(std::ofstream& file, const User& user);
    // TODO serialize wallet
    // TODO serialize tranasaction
    User deserializeUser(std::ifstream& file);
    // TODO deserialize wallet
    // TODO deserialize tranasaction

    // File operations
    bool saveUsersToFile();
    // TODO save wallet to file
    // TODO save transactions to file
    bool loadUsersFromFile();
    // TODO load wallet from file
    // TODO load transactions from file

public:
    // Singleton instance
    static Database& getInstance();

    // File operations
    bool saveToFiles();
    bool loadFromFiles();

    // User operations
    bool addUser(const User& user);
    bool updateUser(const User& user);
    bool deleteUser(const std::string& username);
    std::shared_ptr<User> getUser(const std::string& username);
    std::vector<std::shared_ptr<User>> getAllUsers();

    // Wallet operations
    // TODO wallet operations

    // Transaction operations
    // TODO transaction operations

    // Database path operations
    // TODO database path
};