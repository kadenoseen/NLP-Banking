/**
 * @file databaseHandler.cpp
 * @brief Implementation of the DatabaseHandler class.
 * @author Kaden Oseen
 */

#include "databaseHandler.h"

using namespace std;

/**
 * @name DatabaseHandler
 * @brief Constructor for the DatabaseHandler class.
 */
DatabaseHandler::DatabaseHandler() {
    // Open the users file
    ifstream file("users.txt");
    if (file.is_open()) {
        string line;
        // Create a User object for each line in the file
        while (getline(file, line)) {
            istringstream iss(line);
            string username, password;
            double balance;
            if (getline(iss, username, ':') && getline(iss, password, ':') && iss >> balance) {
                // Add the User object to the users vector
                users.push_back(User(username, password, balance));
            } else {
                cerr << "Error parsing line: " << line << endl;
            }
        }
    } else {
        cerr << "Could not open users.txt" << endl;
    }
}

/**
 * @name updateUser
 * @brief Update the balance of a user.
 * 
 * @param username The username of the user to update the balance of
 * @param value The value to update the balance with
 */
bool DatabaseHandler::updateUser(const string& username, double value) {
    for (auto& user : users) {
        if (user.getUsername() == username) {
            user.updateBalance(value);
            return true;
        }
    }
    return false;
}

/**
 * @name updateUserBalance
 * @brief Update the balance of a user in users.txt file.
 * 
 * @param user The user object to update the balance of
 * @return true if the balance was updated successfully
 */
bool DatabaseHandler::updateUserBalance(User* user) {
    // Open the users file
    ifstream infile("users.txt");
    string line;
    ostringstream updated_file;
    while (getline(infile, line)) {
        istringstream iss(line);
        string username, password, balance_str;
        double balance;
        if (getline(iss, username, ':') && getline(iss, password, ':') && getline(iss, balance_str)) {
            // If the username matches the username of the user object, update the balance
            if (username == user->getUsername()) {
                try {
                    // Gets user balance and updates the file
                    balance = user->getBalance();
                    updated_file << username << ":" << password << ":" << balance << "\n";
                }
                catch (const exception& e) {
                    cerr << "Error: " << e.what() << endl;
                    return false;
                }
            }
            else {
                updated_file << line << "\n";
            }
        }
    }
    infile.close();

    // Write the updated file to users.txt
    ofstream outfile("users.txt");
    if (outfile.is_open()) {
        outfile << updated_file.str();
        outfile.close();
        return true;
    }
    else {
        cerr << "Error: could not open file" << endl;
        return false;
    }
}

/**
 * @name getUser
 * @brief Get a User object from the users vector.
 * 
 * @param username The username of the user to get
 * @param password The password of the user to get
 * @return User* The User object
 */
User* DatabaseHandler::getUser(string username, string password) {
    for (auto &user : users) {
        if (user.getUsername() == username && user.getPassword() == password) {
            return &user;
        }
    }
    return nullptr;
}

/**
 * @name getRecipient
 * @brief Get a User object from the users vector.
 * 
 * @param username The username of the user to get
 * @return User* The User object
 */
User* DatabaseHandler::getRecipient(string username) {
    for (auto &user : users) {
        if (user.getUsername() == username) {
            return &user;
        }
    }
    return nullptr;
}

/**
 * @name addUser
 * @brief Add a User object to the users vector.
 * 
 * @param username The username of the user to add
 * @param password The password of the user to add
 * @param balance The balance of the user to add
 * @return true if the user was added successfully
 */
User* DatabaseHandler::addUser(const string& username, const string& password, double balance) {
    for (auto& user : users) {
        if (user.getUsername() == username) {
            return nullptr;
        }
    }
    users.push_back(User(username, password, balance));
    
    // add user in format username:password:balance to new line in users.txt file
    ofstream outfile("users.txt", ios_base::app);
    if (outfile.is_open()) {
        outfile << username << ":" << password << ":" << balance << "\n";
        outfile.close();
        cout << "User " << username << " added successfully" << endl;
        return getUser(username, password);
    }
    else {
        cerr << "Error: could not open file" << endl;
        return nullptr;
    }
}


/**
 * @name getUsers
 * @brief Get the users vector.
 * 
 * @return vector<User>& The users vector
 */
vector<User>& DatabaseHandler::getUsers() {
    return users;
}
