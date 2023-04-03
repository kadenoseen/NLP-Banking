/**
 * @file user.cpp
 * @brief Implementation of the User class.
 * @author Kaden Oseen
 */

#include "user.h"

using namespace std;

/**
 * @name User
 * @brief Constructor for the User class.
 * 
 * @param username The username of the user.
 * @param password The password of the user.
 * @param balance The balance of the user.
 */
User::User(const string& username, const string& password, double balance)
    : username(username), password(password), balance(balance) {}

/**
 * @name getUsername
 * @brief Returns the username of the user.
 * 
 * @return The username of the user.
 */
string User::getUsername() const {
    return username;
}

/**
 * @name getPassword
 * @brief Returns the password of the user.
 * 
 * @return The password of the user.
 */
string User::getPassword() const {
    return password;
}

/**
 * @name getBalance
 * @brief Returns the balance of the user.
 * 
 * @return The balance of the user.
 */
double User::getBalance() const {
    return balance;
}

/**
 * @name updateBalance
 * @brief Updates the balance of the user.
 * 
 * @param amount The amount to update the balance with.
 */
void User::updateBalance(double amount) {
    balance += amount;
}

/**
 * @name getTransactionLog
 * @brief Returns the transaction log of the user.
 * 
 * @return The transaction log of the user.
 */
string User::getTransactionLog() const {
    std::string result;

    for (const auto& entry : transactionLog) {
        result += entry + "\n";
    }

    return result;
}

/**
 * @name addTransaction
 * @brief Adds a transaction to the transaction log of the user.
 * 
 * @param transaction The transaction to add to the transaction log.
 */
void User::addTransaction(const string& transaction) {
    transactionLog.push_back(transaction);
}
