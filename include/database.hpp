#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <filesystem>
#include <chrono>
#include "wallet.hpp"
#include "transaction.hpp"

class Database {
private:
    static Database* instance;
    static Database *instance_;

    std::map<std::string, std::shared_ptr<Wallet>> wallets;
    std::vector<Transaction> transactions_;

    // File paths

    std::string WALLETS_FILE;
    std::string TRANSACTIONS_FILE;

    // Last modification times

    std::filesystem::file_time_type lastWalletsModification;
    std::filesystem::file_time_type lastTransactionsModification;

    // Private constructor for singleton
    Database();
    ~Database();

    // Helper functions
    bool isFileModified();

    void serializeWallet(std::ofstream &file, const Wallet &wallet);
    void serializeTransaction(std::ofstream &file, const Transaction &transaction);

    Wallet deserializeWallet(std::ifstream &file);
    Transaction deserializeTransaction(std::ifstream &file);

    // File operations

    bool saveWalletsToFile();
    bool saveTransactionsToFile();

    bool loadWalletsFromFile();
    bool loadTransactionsFromFile();

public:
    // Singleton instance
    static Database& getInstance();

    // Ngan chan copy va assignment
    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;

    // File operations
    bool saveToFiles();
    bool loadFromFiles();


    // Wallet operations
    bool addWallet(const Wallet &wallet);
    bool updateWallet(const Wallet &wallet);
    bool deleteWallet(const std::string &walletId);
    std::shared_ptr<Wallet> getWallet(const std::string &walletId);
    std::vector<std::shared_ptr<Wallet>> getAllWallets();

    // Transaction operations
    bool addTransaction(const Transaction &transaction);
    std::vector<Transaction> getWalletTransactions(const std::string &walletId);

    // Database path operations
    void setBasePath(const std::filesystem::path &path);
};