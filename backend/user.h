/**
 * @file user.h
 * @brief Declaration of the User class.
 * @author Kaden Oseen
 */

#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <iomanip>

/**
 * @class User
 * @brief Class for storing user data.
 * 
 * @param username The username of the user.
 * @param password The password of the user.
 * @param balance The balance of the user.
 */
class User {
public:
    // Constructors
    User() : username(""), password(""), balance(0.0) {}
    User(const std::string& username, const std::string& password, double balance);
    
    // Getters
    std::string getUsername() const;
    std::string getPassword() const;
    double getBalance() const;
    std::string getTransactionLog() const;

    // Setters
    void updateBalance(double amount);
    void addTransaction(const std::string& transaction);

private:
    // Private variables
    // add recipient list
    std::string username;
    std::string password;
    double balance;
    std::vector<std::string> transactionLog;
};


#endif
