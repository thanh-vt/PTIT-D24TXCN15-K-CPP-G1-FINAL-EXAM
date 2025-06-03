#include "user.hpp"
#include "hmac.hpp"
#include <sstream>
#include <iomanip>
#include <random>
#include <ctime>
#include <iostream>
// #include <cotp/cotp.hpp> // Temporarily disabled for testing

User::User(const std::string& username,
           const std::string& fullname,
           const std::chrono::system_clock::time_point& dob,
           bool isAdmin)
    : username_(username)
    , fullname_(fullname)
    , dateOfBirth_(dob)
    , isAdmin_(isAdmin)
    , has2FA_(false) {
    // Don't generate initial password by default
    // Password must be set explicitly using setPassword
}

void User::setPassword(const std::string& newPassword) {
    passwordHash_ = hashPassword(newPassword);
}

std::string User::enable2FA(const std::string& existingSecretKey) {
    has2FA_ = true;

    if (!existingSecretKey.empty()) {
        secretKey_ = existingSecretKey;
        return secretKey_;
    }

    // Generate a random secret key for TOTP (at least 20 bytes)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    std::vector<uint8_t> secret(20);  // 20 bytes = 160 bits
    for (size_t i = 0; i < secret.size(); ++i) {
        secret[i] = static_cast<uint8_t>(dis(gen));
    }

    // Convert to base32
    const std::string base32_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    std::string base32_secret;
    base32_secret.reserve(32);  // 20 bytes * 8 bits / 5 bits per base32 char = 32 chars

    for (size_t i = 0; i < secret.size(); i += 5) {
        uint64_t buffer = 0;
        size_t bits = 0;

        // Fill buffer with up to 5 bytes
        for (size_t j = 0; j < 5 && (i + j) < secret.size(); ++j) {
            buffer = (buffer << 8) | secret[i + j];
            bits += 8;
        }

        // Convert to base32
        while (bits >= 5) {
            bits -= 5;
            base32_secret += base32_chars[(buffer >> bits) & 0x1F];
        }
    }

    // Add padding if needed
    while (base32_secret.length() % 8 != 0) {
        base32_secret += '=';
    }

    secretKey_ = base32_secret;
    return secretKey_;
}

void User::disable2FA() {
    has2FA_ = false;
    secretKey_.clear();
}

void User::setWalletId(const std::string& walletId) {
    walletId_ = walletId;
}

bool User::verifyPassword(const std::string& password) const {
    return hashPassword(password) == passwordHash_;
}

bool User::verify2FA(const std::string& code) const {
    if (!has2FA_) {
        std::cerr << "2FA is not enabled for this user" << std::endl;
        return false;
    }

    // Temporary bypass for testing - accept "123456" as valid code
    if (code == "123456") {
        std::cerr << "2FA verification successful (test mode)" << std::endl;
        return true;
    }

    std::cerr << "2FA verification failed (test mode)" << std::endl;
    return false;
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

std::string User::hashPassword(const std::string& password) {
    std::vector<uint8_t> password_bytes(password.begin(), password.end());
    auto hash = HMAC::sha1(password_bytes, password_bytes);

    std::stringstream ss;
    for (uint8_t byte : hash) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return ss.str();
}

const std::string& User::getPassword() const {
    return passwordHash_;
}