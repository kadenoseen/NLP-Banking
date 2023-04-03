/**
 * @file transactionHandler.cpp
 * @brief Implementation of the TransactionHandler class.
 * @author Kaden Oseen
 */

#include "transactionHandler.h"

using namespace std;

/**
 * @brief Handles a transaction.
 * 
 * @param transactionType The type of transaction to handle.
 * @param user The user to handle the transaction for.
 * @param value The value of the transaction.
*/
string TransactionHandler::handleTransaction(TransactionType transactionType, User* user, double value) {
    string bal = "";
    string timestamp = getTimestamp();
    stringstream transactionLog;
    switch (transactionType) {
        // Handle deposit, withdrawal, and balance requests
        case TransactionType::DEPOSIT:
            user->updateBalance(value);
            bal = to_string(user->getBalance());
            bal = bal.substr(0, bal.size() - 4);
            transactionLog << timestamp << " --- Deposit --- $" << value;
            user->addTransaction(transactionLog.str());
            return "Deposit successful. New balance: " + bal;

        case TransactionType::WITHDRAW:
            // Checks if the user has enough funds to withdraw
            if (user->getBalance() < value || value < 0) {
                return "Insufficient funds";
            }
            // Updates the user's balance
            user->updateBalance(-value);
            bal = to_string(user->getBalance());
            bal = bal.substr(0, bal.size() - 4);
            transactionLog << timestamp << " --- Withdrawal --- $" << value;
            user->addTransaction(transactionLog.str());
            return "Withdrawal successful. New balance: " + bal;

        case TransactionType::BALANCE:
            // Returns the user's balance
            return "Current balance: " + to_string(user->getBalance());
        default:
            return "Invalid transaction type";
    }
}

/**
 * @brief Handles a transfer between two users.
 * 
 * @param user The user to transfer from.
 * @param recipient The user to transfer to.
 * @param value The value to transfer.
*/
bool TransactionHandler::handleTransfer(User* user, User* recipient, double value) {
    // Checks if the user has enough funds to transfer
    if (user->getBalance() < value || value < 0) {
        return false;
    }
    string timestamp = getTimestamp();
    stringstream transactionLog;
    
    // Updates the user's and recipient's balances
    user->updateBalance(-value);
    if(recipient != nullptr){
        recipient->updateBalance(value);
        transactionLog << timestamp << " --- Transfer --- $" << value << " --- " << user->getUsername() << " -> " << recipient->getUsername();
        user->addTransaction(transactionLog.str());
    }else{
        transactionLog << timestamp << " --- Transfer --- $" << value << " --- " << user->getUsername() << " -> ExternalRecipient";
        user->addTransaction(transactionLog.str());
    }
    return true;
}
