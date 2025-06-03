#include "../include/database.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <filesystem>

Database* Database::instance = nullptr;

// TODO

Database& Database::getInstance() {
    if (!instance) {
        instance = new Database();
    }
    return *instance;
}

Database::Database() {
    std::cout << "Database constructor started..." << std::endl;
    // TODO
}

Database::~Database() {
    std::cout << "Database destructor called..." << std::endl;
    // TODO
}