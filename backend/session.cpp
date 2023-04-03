/**
 * @file session.cpp
 * @brief Implementation of the Session class.
 * Class handles individual client sessions on the NLP Banking server.
 * Deals with all communication/interaction from server side.
 * @author Kaden Oseen
 */

#include "session.h"

using namespace std;

/**
 * @brief Constructor for a new Session object
 * 
 * @param socket The socket to communicate with the client.
 * @param new_ssl The SSL object to use for encryption.
 */
Session::Session(int socket, SSL* new_ssl) : m_socket(socket), ssl(new_ssl) {}


/**
 * @brief Starts a new session for a client connecting to the server
 * Loops until the client disconnects or logs out, handling requests as sent by client.
 */
void Session::start_session() {

    cout << "Starting new session..." << endl;
    // Send welcome message to client and ask if they have an existing account
    send_message("Welcome to NLP banking!\n1. Login to existing account\n2. Create Account");
    string response = receive_message();
    if(response == "exit"){
        disconnect();
        return;
    }
    // Call login function if user has an existing account
    if(response == "1"){
        bool success = login();
        if(!success){
            disconnect();
            return;
        }
    }
    // Call create account function if user does not have an existing account
    else if(response == "2"){
        bool success = createAccount();
        if(!success){
            disconnect();
            return;
        }
    }else{
        return;
    }

    // Loop until user exits session
    while (true) {
        // Requests messages from users and processes them until exit message is received.
        string request = receive_message();
        if(request == "exit"){
            disconnect();
            return;
        }
        try {
            process_request(request);
        }
        catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            break;
        }
    }
    // Remove user from active_sessions map
    
    lock_guard<mutex> guard(active_sessions_mutex);
    active_sessions.erase(user->getUsername());
    
}

/**
 * @brief Login method for existing users
 * Asks client for username and password, checks if user exists in database and if password is correct.
 * Gives 3 attempts for successful login before disconnecting.
 * @return true if login is successful, false otherwise.
 */
bool Session::login(){
    bool success = false;
    string username = "";
    int tries = 0;
    send_message("Username:");
    // Loop for up to 3 login attempts
    while(strcmp(username.c_str(), "") == 0){
        string received = receive_message();
        if(received == "exit"){
            disconnect();
            return false;
        }
        // checks if username exists
        if(dbHandler.getRecipient(received) != nullptr){
            username = received;
        }else {
            cout << "User attempted to login with invalid username " << username << endl;
            send_message("Invalid username, please try again.\nUsername: ");
        }
    }
    // Asks for password, gives 3 attempts for success
    while (!success && tries < 3) {
        if(tries == 0){
            send_message("Password:");
        }else{
            send_message("Incorrect password, please try again.\nPassword:");
        }
        string password = receive_message();
        if(password == "exit"){
            disconnect();
            return false;
        }
        // Hashes password and checks if the user exists in the database
        string new_password = get_hash(password);
        User* newUser = dbHandler.getUser(username, new_password);
        if (newUser != nullptr) {
            // Checks if user is already logged in with mutex lock
            lock_guard<mutex> guard(active_sessions_mutex);
            
            auto it = active_sessions.find(username);
            
            if (it != active_sessions.end() && it->second) {
                send_message("101");
                return false;
            }
            active_sessions[username] = this;

            cout << "User " << newUser->getUsername() << " successfully logged in " << "with password: " << newUser->getPassword() << endl;

            // Ask if user wants to use natural language prompts
            send_message("Successfully logged in!\nWould you like to use natural language prompts today? (y/n)");
            string response = receive_message();
            if(response == "exit"){
                disconnect();
                return false;
            }
            if(response == "y"){
                nlp = true;
                send_message("Welcome " + username + "!\nWhat would you like to do today?");
            }else{
                nlp = false;
                send_message("Welcome " + username + "!\nWhat would you like to do today?" + OPTIONS_MESSAGE);
            }
            // Send welcome message and set class user object
            user = newUser;
            success = true;

        } else {
            cout << "User " << username << " failed to logged in " << "with password: " << password << endl;
            ++tries;
        }
    }
    // Handle case when user exceeds login attempts
    if (!success) {
        send_message("106");
        return false;
    }
    return true;
}

/**
 * @brief Create account method for new users
 * Asks client for username and password, checks if username is existing.
 * Hashes password and stores new user in database.
 * @return true if acccount creation is successful, false otherwise.
 */
bool Session::createAccount(){
    send_message("Please create a username: ");
    string username = receive_message();
    if(username == "exit"){
        disconnect();
        return false;
    }
    if(dbHandler.getRecipient(username) != nullptr){
        send_message("104");
        cout << "User failed to create account (existing username: " << username << ")" << endl;
        return false;
    }
    send_message("Please enter a password:");
    string password = receive_message();
    if(password == "exit"){
        disconnect();
        return false;
    }
    string new_password = get_hash(password);
    user = dbHandler.addUser(username, new_password, 0);
    cout << "User " << user->getUsername() << " successfully created account " << "with password: " << user->getPassword() << endl;
    // Ask if user wants to use natural language prompts
    send_message("Successfully logged in!\nWould you like to use natural language prompts today? (y/n)");
    string response = receive_message();
    if(response == "exit"){
        disconnect();
        return false;
    }
    // Sets NLP flag and sends welcome message
    if(response == "y"){
        nlp = true;
        send_message("Welcome " + username + "!\nWhat would you like to do today?");
    }else{
        nlp = false;
        send_message("Welcome " + username + "!\nWhat would you like to do today?" + OPTIONS_MESSAGE);
    }
    return true;
}


/**
 * @brief Receives a message from the client over a TLS-encrypted connection.
 * Ensures client did not unexpectedly disconnect by checking if bytes received.
 * Handles unexpected disconnects by returning the "exit" message.
 * 
 * @param ssl The SSL object representing the TLS connection.
 * 
 * @return string The message received from the client.
 */
string Session::receive_message() {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int bytes_received = SSL_read(ssl, buffer, 1024);
    cout << "Received message: " << buffer << endl;
    // If no bytes were received or error occurred, close socket
    if (bytes_received <= 0) {
        return "exit";
    }else{
        return string(buffer, bytes_received);
    }
    
}

/**
 * @brief Sends a message to the client over a TLS-encrypted connection.
 * Converts message to a c_str and sends it to the client via SSL_write.
 * 
 * @param ssl The SSL object representing the TLS connection.
 * @param message The message to send to the client.
 */
void Session::send_message(const string& message) {
    string newMessage = "\n" + message;
    const char* message_cstr = newMessage.c_str();
    cout << "Sending message: " << message_cstr << endl;
    int message_length = strlen(message_cstr);
    // Send message and check if all bytes were sent
    int bytes_sent = SSL_write(ssl, message_cstr, message_length);
    if (bytes_sent == -1) {
        // Print the error message to stderr
        std::cerr << "Error sending message: " << ERR_error_string(ERR_get_error(), NULL) << std::endl;
        // Throw a runtime error
        throw std::runtime_error("Error sending message");
    }
}




/**
 * @brief Processes a request received from the client.
 * Determines whether or not to interpret request with NLP.
 * Message interpreted and action/value sent to handle_request.
 * 
 * @param request The request to process.
 */
void Session::process_request(const string& request) {
    // If NLP is enabled, create a Request object and execute the request with NLP server
    if(nlp){
        Request req(request);
        if (req.execute()) {
            // If request was executed successfully, parse the response and handle the request
            string response = req.result();
            string action = response.substr(1, response.find(",") - 1);
            string value = response.substr(response.find(",") + 1, response.size() - response.find(",") - 2);
            handle_request(action, value);
        }
    }else{
        // If NLP is disabled, switch statement used to check action chosen
        string value = "";
        switch (request[0]) {
            case '1':
                handle_request("balance", "");
                break;
            case '2':
                send_message("How much would you like to deposit?");
                value = receive_message();
                value = removeCharacters(value);
                handle_request("deposit", value);
                break;
            case '3':
                send_message("How much would you like to withdraw?");
                value = receive_message();
                value = removeCharacters(value);
                handle_request("withdraw", value);
                break;
            case '4':
                send_message("How much would you like to transfer?");
                value = receive_message();
                value = removeCharacters(value);
                handle_request("transfer", value);
                break;
            case '5':
                handle_request("history", "");
                break;
            case '6':
                handle_request("backwards", value);
                break;
            case '7':
                handle_request("logout", value);
                break;
            default:
                send_message("Invalid option, please try again.");
                break;
        }
    }
}

/**
 * @brief Handles a banking request received from the client.
 * Handles specific requests, delegating to the TransactionHandler class to perform transactions.
 * Responsible for confirming user actions and sending appropriate messages to the client.
 * 
 * @param action The action to perform.
 * @param value The value associated with the action.
 */
void Session::handle_request(string action, string value){
    // Create a TransactionHandler object to handle transactions
    TransactionHandler transaction_handler;
    string options = "";
    if(!nlp){
        options = OPTIONS_MESSAGE;
    }
    double amount;

    // Verify the user's requested action and execute the appropriate transaction
    try {
        if (action == "deposit") {
            try{
                amount = stod(value);
            }catch(const exception& e){
                send_message("Invalid value.\nWhat else can I help you with today?" + OPTIONS_MESSAGE);
                return;
            }
            if(amount < 0){
                send_message("How much would you like to deposit?");
                value = receive_message();
                value = removeCharacters(value);
                try{
                    amount = stod(value);
                }catch(const exception& e){
                    send_message("Invalid value.\nWhat else can I help you with today?" + OPTIONS_MESSAGE);
                    return;
                }
            }
            // Ask the user if they are sure they want to deposit the specified value
            send_message("Are you sure you want to deposit " + value + "? (y/n)");
            string response = receive_message();
            
            // If the user confirms, execute the deposit transaction
            if (response == "y" || response == "yes") {
                transaction_handler.handleTransaction(TransactionHandler::TransactionType::DEPOSIT, user, amount);
                string bal = to_string(user->getBalance());
                bal = bal.substr(0, bal.size() - 4);

                send_message("Deposit successful. New balance: " + bal + "\nWhat else can I help you with today?" + options);
                
                // Update the user's balance in the database
                dbHandler.updateUserBalance(user);
            } else {
                // If the user cancels the deposit, inform them and ask for further requests

                send_message("Deposit cancelled.\nWhat else can I help you with today?");
            }
        } else if (action == "withdraw") {
            try{
                amount = stod(value);
            }catch(const exception& e){
                send_message("Invalid value.\nWhat else can I help you with today?" + OPTIONS_MESSAGE);
                return;
            }
            if(amount < 0){
                send_message("How much would you like to withdraw?");
                value = receive_message();
                value = removeCharacters(value);
                try{
                    amount = stod(value);
                }catch(const exception& e){
                    send_message("Invalid value.\nWhat else can I help you with today?" + OPTIONS_MESSAGE);
                    return;
                }
            }
            // Ask the user if they are sure they want to withdraw the specified value
            send_message("Are you sure you want to withdraw " + value + "? (y/n)");
            string response = receive_message();
            
            // If the user confirms, execute the withdrawal transaction
            if (response == "y" || response == "yes") {
                
                string reply = transaction_handler.handleTransaction(TransactionHandler::TransactionType::WITHDRAW, user, amount);
                send_message(reply + "\nWhat else can I help you with today?" + options);
                
                // Update the user's balance in the database
                dbHandler.updateUserBalance(user);
            } else {
                // If the user cancels the withdrawal, inform them and ask for further requests
                send_message("Withdrawal cancelled.\nWhat else can I help you with today?" + options);
            }
        } else if (action == "transfer") {
            // Ask the user who they want to transfer funds to
            send_message("Who would you like to transfer to?\n\
            1. Existing user\n\
            2. External user (by email)");
            string choice = receive_message();
            if(choice == "1"){
                // If the user wants to transfer to an existing user, ask for the recipient's username
                send_message("Please enter the recipient's username:");
            }else if(choice == "2"){
                send_message("Please enter the recipient's email:");
            }else{
                send_message("Transfer cancelled.\nWhat else can I help you with today?" + options);
            }
            string recipient = receive_message();
            // Check if the recipient exists in the database
            User* recipient_user = dbHandler.getRecipient(recipient);
            if (recipient_user == nullptr && choice == "1") {
                // If the recipient does not exist, inform the user and ask for further requests
                send_message("Recipient does not exist.\nWhat else can I help you with today?" + options);
                return;
            } else if(recipient == user->getUsername()){
                send_message("You cannot transfer to yourself.\nWhat else can I help you with today?" + options);
                return;
            } else {
                try{
                    amount = stod(value);
                }catch(const exception& e){
                    send_message("Invalid value.\nWhat else can I help you with today?" + OPTIONS_MESSAGE);
                    return;
                }
                if(amount < 0){
                        send_message("How much would you like to transfer to " + recipient + "?");
                        value = receive_message();
                        value = removeCharacters(value);
                        try{
                            amount = stod(value);
                        }catch(const exception& e){
                            send_message("Invalid value.\nWhat else can I help you with today?" + OPTIONS_MESSAGE);
                            return;
                        }
                        
                }
                // If the recipient exists, ask the user if they are sure they want to transfer the specified value
                send_message("Are you sure you want to transfer " + value + " to " + recipient + "? (y/n)");
                string response = receive_message();
                
                // If the user confirms, execute the transfer transaction
                if (response == "y" || response == "yes") {
                    
                    bool reply = transaction_handler.handleTransfer(user, recipient_user, amount);
                    
                    if(reply){
                        string bal = to_string(user->getBalance());
                        bal = bal.substr(0, bal.size() - 4);
                        // If the transfer is successful, inform the user and update both users' balances in the database
                        send_message("Transfer to " + recipient + " successful. New balance: " + bal + "\nWhat else can I help you with today?" + options);
                        dbHandler.updateUserBalance(user);
                        if(recipient_user != nullptr){
                            dbHandler.updateUserBalance(recipient_user);
                        }
                    } else {
                        // If the transfer fails due to insufficient funds, inform the user and ask for further requests
                        send_message("Transfer to " + recipient + " failed. Insufficient funds!\nWhat else can I help you with today?" + options);
                    }
                // If the user cancels the transfer, inform them and ask for further requests
                } else {
                    send_message("Transfer cancelled.\nWhat else can I help you with today?" + options);
                }
            }
        } else if (action == "balance") {
            // If the user requests their balance, inform them and ask for further requests
            string bal = to_string(user->getBalance());
            bal = bal.substr(0, bal.size() - 4);
            send_message("Your balance is: " + bal + "\nWhat else can I help you with today?" + options);
        }
        else if(action == "history"){
            // If the user requests their transaction history, send transaction log.
            string transactions = user->getTransactionLog();
            if(transactions == ""){
                send_message("You have no transactions.\nWhat else can I help you with today?" + options);
            }else{
                send_message(user->getUsername() + "'s Transaction Log:\n" + user->getTransactionLog() + "\nWhat else can I help you with today?" + options);
            }
        }
        else if (action == "backwards"){
            if(nlp){
                send_message("Are you sure you would like to switch to regular prompts? (y/n)");
                string response = receive_message();
                if(response == "exit"){
                    disconnect();
                    return;
                }
                // Sets NLP flag and sends welcome message
                if(response == "y"){
                    nlp = false;
                    send_message("What would you like to do today?" + OPTIONS_MESSAGE);
                }else {
                    send_message("What else can I help you with today?");
                }
            }else{
                nlp = true;
                send_message("What can I help you with today?");
            }
            
        }
        else if (action == "options"){
            send_message("You can withdraw, deposit, transfer, check your balance, change back to normal inputs, or view your transaction history.\nWhat would you like to do today?");
        }
        else if (action == "logout"){
            // Send logout code to client.
            send_message("105");
            disconnect();
        } else {
            if(nlp){
                send_message("Sorry I didn't get that. Please try again.\nWhat can I help you with?");
            }else{
                send_message("Invalid action.\nWhat else can I help you with today?" + OPTIONS_MESSAGE);
            }
        }
    } catch (const exception& e) {
        send_message("Error: " + string(e.what()));
    }
}


/**
 * @brief Disconnects the client from the server.
 * Closes socket and ends connection.
 */
void Session::disconnect() {
    close(m_socket);
    try{
        lock_guard<mutex> guard(active_sessions_mutex);
        active_sessions.erase(user->getUsername());
    }
    catch (const exception& e){
        cout << "Error: " << e.what() << endl;
    }
}