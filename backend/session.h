/**
 * @file session.h
 * @brief Declaration of the Session class.
 * @author Kaden Oseen
 */

#ifndef SESSION_H
#define SESSION_H

#include <iostream>
#include <string>
#include <cstring>
#include <mutex>
#include <unistd.h>
#include "request.h"
#include "databaseHandler.h"
#include "user.h"
#include "globals.h"
#include "transactionHandler.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <iomanip>


/**
 * @class Session
 * @brief Class for handling a session with a client.
 * Class handles individual client sessions on the NLP Banking server.
 * Deals with all communication/interaction from server side.
 */
class Session {
public:
    // Constructor and destructor
    Session(int socket, SSL* new_ssl);
    void start_session();
    void disconnect();
private:
    // Variables
    int m_socket;
    SSL* ssl;
    bool nlp;
    User* user;
    DatabaseHandler dbHandler;
    const std::string OPTIONS_MESSAGE = "\n\
    1. View Balance\n\
    2. Deposit\n\
    3. Withdraw\n\
    4. Transfer Funds\n\
    5. View Transaction History\n\
    6. Change to NLP\n\
    7. LogOut\n";

    // Methods
    std::string receive_message();
    void send_message(const std::string& message);
    void process_request(const std::string& request);
    void handle_request(std::string action, std::string value);
    bool login();
    bool createAccount();
};

#endif
