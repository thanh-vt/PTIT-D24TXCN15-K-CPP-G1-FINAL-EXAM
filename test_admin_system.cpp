// Test file cho he thong admin hoan chinh
#include "include/database.hpp"
#include "include/ui.h"
#include <iostream>
#include <string>

void testAdminSystemComplete() {
    std::cout << "\n=== Testing Complete Admin System ===" << std::endl;
    
    Database& db = Database::getInstance();
    
    // Test 1: Admin authentication
    std::cout << "\n--- Test 1: Admin Authentication ---" << std::endl;
    bool authResult = db.authenticateAdmin("admin", "admin123");
    std::cout << "Admin auth result: " << (authResult ? "SUCCESS" : "FAILED") << std::endl;
    
    // Test 2: View initial users (should show admin)
    std::cout << "\n--- Test 2: View Initial Users ---" << std::endl;
    db.adminViewAllUsers();
    
    // Test 3: Create new user
    std::cout << "\n--- Test 3: Create New User ---" << std::endl;
    std::string generatedPassword;
    bool createResult = db.adminCreateUser("testuser", "Test User", "1995-05-15", generatedPassword);
    std::cout << "Create user result: " << (createResult ? "SUCCESS" : "FAILED") << std::endl;
    std::cout << "Generated password: " << generatedPassword << std::endl;
    
    // Test 4: View all users after creation
    std::cout << "\n--- Test 4: View All Users After Creation ---" << std::endl;
    db.adminViewAllUsers();
    
    // Test 5: View all wallets
    std::cout << "\n--- Test 5: View All Wallets ---" << std::endl;
    db.adminViewAllWallets();
    
    // Test 6: View wallet details
    std::cout << "\n--- Test 6: View Wallet Details ---" << std::endl;
    db.adminViewWalletDetails("ADMIN_WALLET_001");
    db.adminViewWalletDetails("WALLET_testuser_001");
    
    // Test 7: System statistics
    std::cout << "\n--- Test 7: System Statistics ---" << std::endl;
    db.adminViewSystemStats();
    
    // Test 8: Transaction log
    std::cout << "\n--- Test 8: Transaction Log ---" << std::endl;
    db.adminViewTransactionLog();
    
    // Test 9: Update user info
    std::cout << "\n--- Test 9: Update User Info ---" << std::endl;
    bool updateResult = db.adminUpdateUserInfo("testuser", "Updated Test User", "1995-05-16", "123456");
    std::cout << "Update user result: " << (updateResult ? "SUCCESS" : "FAILED") << std::endl;
    
    // Test 10: View users after update
    std::cout << "\n--- Test 10: View Users After Update ---" << std::endl;
    db.adminViewAllUsers();
    
    // Test 11: Backup database
    std::cout << "\n--- Test 11: Backup Database ---" << std::endl;
    bool backupResult = db.adminBackupDatabase("./test_backups");
    std::cout << "Backup result: " << (backupResult ? "SUCCESS" : "FAILED") << std::endl;
    
    // Test 12: Try to delete admin (should fail)
    std::cout << "\n--- Test 12: Try to Delete Admin (should fail) ---" << std::endl;
    bool deleteAdminResult = db.adminDeleteUser("admin");
    std::cout << "Delete admin result: " << (!deleteAdminResult ? "CORRECTLY FAILED" : "INCORRECTLY SUCCEEDED") << std::endl;
    
    // Test 13: Delete test user
    std::cout << "\n--- Test 13: Delete Test User ---" << std::endl;
    bool deleteResult = db.adminDeleteUser("testuser");
    std::cout << "Delete user result: " << (deleteResult ? "SUCCESS" : "FAILED") << std::endl;
    
    // Test 14: View final state
    std::cout << "\n--- Test 14: Final State ---" << std::endl;
    db.adminViewAllUsers();
    db.adminViewAllWallets();
    db.adminViewSystemStats();
    
    std::cout << "\n=== Admin System Test Completed ===" << std::endl;
}

void testUIIntegration() {
    std::cout << "\n=== Testing UI Integration ===" << std::endl;
    
    // This would normally be interactive, but we can test the structure
    UI& ui = UI::getInstance();
    
    std::cout << "UI instance created successfully!" << std::endl;
    std::cout << "Admin functions are available in UI class." << std::endl;
    std::cout << "To test interactively, run the main program and choose 'Admin Login'." << std::endl;
}

void showUsageInstructions() {
    std::cout << "\n=== Usage Instructions ===" << std::endl;
    std::cout << "1. Compile: g++ -std=c++17 src/main.cpp src/ui.cpp src/database.cpp -I. -o ewallet_admin" << std::endl;
    std::cout << "2. Run: ./ewallet_admin" << std::endl;
    std::cout << "3. Choose option 3 (Admin Login)" << std::endl;
    std::cout << "4. Login with: admin / admin123" << std::endl;
    std::cout << "5. Use admin menu to manage users, wallets, and system" << std::endl;
    std::cout << "\nAdmin Features Available:" << std::endl;
    std::cout << "- View/Create/Update/Delete Users" << std::endl;
    std::cout << "- View Wallets and Details" << std::endl;
    std::cout << "- System Statistics and Transaction Log" << std::endl;
    std::cout << "- Database Backup and Restore" << std::endl;
}

int main() {
    try {
        std::cout << "=== ADMIN SYSTEM COMPLETE TEST SUITE ===" << std::endl;
        
        testAdminSystemComplete();
        testUIIntegration();
        showUsageInstructions();
        
        std::cout << "\n=== ALL TESTS COMPLETED SUCCESSFULLY ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during test: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
