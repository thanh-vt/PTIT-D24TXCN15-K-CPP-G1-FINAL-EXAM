#pragma once
class Database {
private:
    static Database* instance;

    // Private constructor for singleton
    Database();
    ~Database();

public:
    // Singleton instance
    static Database& getInstance();

};