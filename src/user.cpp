#include "user.hpp"
#include "hmac.hpp"
#include <sstream>
#include <iomanip>
#include <random>
#include <ctime>
// #include <cotp/cotp.hpp>  // 2FA not implemented yet

User::User(const std::string &username,
           const std::string &fullname,
           const std::chrono::system_clock::time_point &dob,
           bool isAdmin)
    : username_(username)
      , fullname_(fullname)
      , dateOfBirth_(dob)
      , isAdmin_(isAdmin)
      , has2FA_(false) {
    // Don't generate initial password by default
    // Password must be set explicitly using setPassword
}

void User::setPassword(const std::string &newPassword) {
    passwordHash_ = hashPassword(newPassword);
}

std::string User::enable2FA(const std::string &existingSecretKey) {
    has2FA_ = true;
    // TODO implement 2FA
    return "";
}

void User::disable2FA() {
    has2FA_ = false;
    secretKey_.clear();
}

void User::setWalletId(const std::string &walletId) {
    walletId_ = walletId;
}

bool User::verifyPassword(const std::string &password) const {
    return hashPassword(password) == passwordHash_;
}

bool User::verify2FA(const std::string &code) const {
    // TODO implement 2FA
    return true;
}

std::string User::generatePassword() {
    const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, charset.length() - 1);

    std::string password;
    password.reserve(12);
    for (int i = 0; i < 12; ++i) {
        password += charset[dis(gen)];
    }
    return password;
}

std::string User::hashPassword(const std::string &password) {
    std::vector<uint8_t> password_bytes(password.begin(), password.end());
    auto hash = HMAC::sha1(password_bytes, password_bytes);

    std::stringstream ss;
    for (uint8_t byte: hash) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return ss.str();
}

const std::string &User::getPassword() const {
    return passwordHash_;
}
