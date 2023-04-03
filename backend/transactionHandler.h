/**
 * @file transactionHandler.h
 * @brief Declaration of the TransactionHandler class.
 * @author Kaden Oseen
 */

#ifndef TRANSACTION_HANDLER_H
#define TRANSACTION_HANDLER_H

#include <string>
#include "user.h"
#include "globals.h"

/**
 * @class TransactionHandler
 * @brief Class for handling transactions.
 */
class TransactionHandler {
public:
    // Constants for transaction types
    enum class TransactionType {
        DEPOSIT,
        WITHDRAW,
        BALANCE
    };
    // Methods
    static std::string handleTransaction(TransactionType transactionType, User* user, double value);
    bool handleTransfer(User* user, User* recipient, double value);
};

#endif
