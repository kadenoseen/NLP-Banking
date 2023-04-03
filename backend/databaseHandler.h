/**
 * @file databaseHandler.h
 * @brief Declaration of the DatabaseHandler class.
 * @author Kaden Oseen
 */

#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include <vector>
#include <string>
#include "user.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "globals.h"

/**
 * @class DatabaseHandler
 * @brief Class for handling the database.
 */
class DatabaseHandler {
public:
    // Constructor
    DatabaseHandler();
    // Methods
    bool updateUser(const std::string& username, double value);
    bool updateUserBalance(User* user);
    User* addUser(const std::string& username, const std::string& password, double balance);
    User* getUser(std::string username, std::string password);
    User* getRecipient(std::string username);
    std::vector<User>& getUsers();
private:
    // Array of Users
    std::vector<User> users;
};

#endif
