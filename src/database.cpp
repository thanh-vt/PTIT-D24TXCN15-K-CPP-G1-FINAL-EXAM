#include "database.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <filesystem>

Database *Database::instance = nullptr;

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
        if (std::filesystem::exists(WALLETS_FILE)) {
            auto lastWriteTime = std::filesystem::last_write_time(WALLETS_FILE);
            if (lastWriteTime != lastWalletsModification) {
                lastWalletsModification = lastWriteTime;
                modified = true;
            }
        }

        // Check transactions file
        if (std::filesystem::exists(TRANSACTIONS_FILE)) {
            auto lastWriteTime = std::filesystem::last_write_time(TRANSACTIONS_FILE);
            if (lastWriteTime != lastTransactionsModification) {
                lastTransactionsModification = lastWriteTime;
                modified = true;
            }
        }

        return modified;
    } catch (const std::exception &e) {
        std::cerr << "Critical error in isFileModified: " << e.what() << std::endl;
        throw;
    }
}

Database &Database::getInstance() {
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

bool Database::addUser(const User &user) {
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

bool Database::updateUser(const User &user) {
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

bool Database::deleteUser(const std::string &username) {
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

std::shared_ptr<User> Database::getUser(const std::string &username) {
    auto it = users.find(username);
    return it != users.end() ? it->second : nullptr;
}

std::vector<std::shared_ptr<User> > Database::getAllUsers() {
    std::vector<std::shared_ptr<User> > result;
    for (const auto &pair: users) {
        result.push_back(pair.second);
    }
    return result;
}

bool Database::addWallet(const Wallet &wallet) {
    std::cout << "Adding wallet: " << wallet.getId() << std::endl;

    // Only check for file modifications if we're not in initialization
    if (!wallets.empty() && isFileModified()) {
        std::cout << "Database file was modified, reloading..." << std::endl;
        if (!loadFromFiles()) {
            std::cerr << "Failed to reload database" << std::endl;
            return false;
        }
    }

    auto id = wallet.getId();
    if (wallets.find(id) != wallets.end()) {
        std::cout << "Wallet already exists: " << id << std::endl;
        return false;
    }

    // Add the wallet to the in-memory map
    wallets[id] = std::make_shared<Wallet>(wallet);
    std::cout << "Wallet added to memory: " << id << std::endl;

    // Save changes to file
    if (!saveToFiles()) {
        std::cerr << "Failed to save database after adding wallet" << std::endl;
        // Remove the wallet from memory if save failed
        wallets.erase(id);
        return false;
    }

    std::cout << "Wallet successfully added and saved: " << id << std::endl;
    return true;
}

bool Database::updateWallet(const Wallet &wallet) {
    if (isFileModified()) {
        if (!loadFromFiles()) {
            return false;
        }
    }

    auto id = wallet.getId();
    if (wallets.find(id) == wallets.end()) {
        return false;
    }
    wallets[id] = std::make_shared<Wallet>(wallet);
    return saveToFiles();
}

bool Database::deleteWallet(const std::string &walletId) {
    if (isFileModified()) {
        if (!loadFromFiles()) {
            return false;
        }
    }

    bool result = wallets.erase(walletId) > 0;
    if (result) {
        saveToFiles();
    }
    return result;
}

std::shared_ptr<Wallet> Database::getWallet(const std::string &walletId) {
    auto it = wallets.find(walletId);
    return it != wallets.end() ? it->second : nullptr;
}

std::vector<std::shared_ptr<Wallet> > Database::getAllWallets() {
    std::vector<std::shared_ptr<Wallet> > result;
    for (const auto &pair: wallets) {
        result.push_back(pair.second);
    }
    return result;
}

bool Database::addTransaction(const Transaction &transaction) {
    if (isFileModified()) {
        if (!loadFromFiles()) {
            return false;
        }
    }

    transactions_.push_back(transaction);
    return saveToFiles();
}

std::vector<Transaction> Database::getWalletTransactions(const std::string &walletId) {
    std::vector<Transaction> result;
    for (const auto &transaction: transactions_) {
        if (transaction.fromWalletId == walletId || transaction.toWalletId == walletId) {
            result.push_back(transaction);
        }
    }
    return result;
}

void Database::serializeUser(std::ofstream &file, const User &user) {
    try {
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

        if (has2FA) {
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

        if (file.fail()) {
            throw std::runtime_error("Failed to write user data");
        }
    } catch (const std::exception &e) {
        std::cerr << "Error serializing user: " << e.what() << std::endl;
        throw;
    }
}


void Database::serializeWallet(std::ofstream &file, const Wallet &wallet) {
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

    for (const auto &transaction: transactions) {
        serializeTransaction(file, transaction);
    }
}

void Database::serializeTransaction(std::ofstream &file, const Transaction &transaction) {
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

User Database::deserializeUser(std::ifstream &file) {
    try {
        // Read username
        size_t len;
        if (!file.read(reinterpret_cast<char *>(&len), sizeof(len)) || len > 1000) {
            throw std::runtime_error("Invalid username length");
        }
        std::string username(len, '\0');
        if (!file.read(&username[0], len)) {
            throw std::runtime_error("Failed to read username");
        }
        std::cout << "Deserializing user: " << username << std::endl;

        // Read fullname
        if (!file.read(reinterpret_cast<char *>(&len), sizeof(len)) || len > 1000) {
            throw std::runtime_error("Invalid fullname length");
        }
        std::string fullname(len, '\0');
        if (!file.read(&fullname[0], len)) {
            throw std::runtime_error("Failed to read fullname");
        }

        // Read date of birth
        std::chrono::system_clock::time_point dob;
        if (!file.read(reinterpret_cast<char *>(&dob), sizeof(dob))) {
            throw std::runtime_error("Failed to read date of birth");
        }

        // Read admin status
        bool isAdmin;
        if (!file.read(reinterpret_cast<char *>(&isAdmin), sizeof(isAdmin))) {
            throw std::runtime_error("Failed to read admin status");
        }

        // Create user with admin status
        User user(username, fullname, dob, isAdmin);

        // Read password hash
        if (!file.read(reinterpret_cast<char *>(&len), sizeof(len)) || len > 1000) {
            throw std::runtime_error("Invalid password hash length");
        }
        std::string passwordHash(len, '\0');
        if (!file.read(&passwordHash[0], len)) {
            throw std::runtime_error("Failed to read password hash");
        }
        user.setPasswordHash(passwordHash);

        // Read 2FA info
        bool has2FA;
        if (!file.read(reinterpret_cast<char *>(&has2FA), sizeof(has2FA))) {
            throw std::runtime_error("Failed to read 2FA status");
        }
        std::cout << "Deserializing 2FA status: " << (has2FA ? "enabled" : "disabled") << std::endl;

        if (has2FA) {
            if (!file.read(reinterpret_cast<char *>(&len), sizeof(len)) || len > 1000) {
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
        if (!file.read(reinterpret_cast<char *>(&len), sizeof(len)) || len > 1000) {
            throw std::runtime_error("Invalid wallet ID length");
        }
        std::string walletId(len, '\0');
        if (!file.read(&walletId[0], len)) {
            throw std::runtime_error("Failed to read wallet ID");
        }
        user.setWalletId(walletId);

        return user;
    } catch (const std::exception &e) {
        std::cerr << "Error deserializing user: " << e.what() << std::endl;
        throw;
    }
}

Wallet Database::deserializeWallet(std::ifstream &file) {
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

    for (size_t i = 0; i < numTransactions; ++i) {
        Transaction transaction = deserializeTransaction(file);
        wallet.addTransaction(transaction);
    }

    return wallet;
}

Transaction Database::deserializeTransaction(std::ifstream &file) {
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

bool Database::saveToFiles() {
    // TODO save to files
    return true;
}

bool Database::loadFromFiles() {
    // TODO load from files
    return true;
}

bool Database::saveUsersToFile() {
    // TODO
    return true;
}


bool Database::saveWalletsToFile() {
    // TODO
    return true;
}

bool Database::saveTransactionsToFile() {
    // TODO
    return true;
}

bool Database::loadUsersFromFile() {
    // TODO
    return true;
}

bool Database::loadWalletsFromFile() {
    // TODO
    return true;
}

bool Database::loadTransactionsFromFile() {
    // TODO
    return true;
}

void Database::setBasePath(const std::filesystem::path &path) {
    // Create database file paths in the specified directory
    USERS_FILE = (path / "users.dat").string();
    WALLETS_FILE = (path / "wallets.dat").string();
    TRANSACTIONS_FILE = (path / "transactions.dat").string();

    std::cout << "Users database path: " << USERS_FILE << std::endl;
    std::cout << "Wallets database path: " << WALLETS_FILE << std::endl;
    std::cout << "Transactions database path: " << TRANSACTIONS_FILE << std::endl;

    // Check if we can write to the directory
    std::string testFile = (path / "test_write.tmp").string();
    std::cout << "Testing write permissions with file: " << testFile << std::endl; {
        std::ofstream test(testFile);
        if (!test) {
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
    if (!loadSuccess || (users.empty() && wallets.empty())) {
        std::cout << "Creating new database with admin user..." << std::endl;

        try {
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
            if (!saveToFiles()) {
                throw std::runtime_error("Failed to save initial database!");
            }

            std::cout << "Initial database created successfully!" << std::endl;
        } catch (const std::exception &e) {
            throw std::runtime_error("Error creating initial database: " + std::string(e.what()));
        }
    } else {
        std::cout << "Existing database loaded successfully!" << std::endl;
    }
}
