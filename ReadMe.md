# **NLPBanking**
Created by Kaden Oseen

NLP Banking is a client-server banking system that allows users to perform various banking operations using natural language processing (NLP) capabilities. It provides an interface through the terminal, enabling users to interact with their bank accounts securely and conveniently.

## **Features**
- Secure Connection: The application establishes a secure connection between the client and server using Transport Layer Security (TLS), ensuring the confidentiality and integrity of the communication.
- User Authentication: Users can create accounts, log in securely, and perform banking operations using their unique credentials.
- Account Management: Users can view their account balance, deposit funds, withdraw funds, and transfer money to other users.
- Transaction History: Users can view their transaction history, providing a comprehensive record of their banking activities.
- Natural Language Processing: The application utilizes NLP techniques to process user input and interpret commands, providing an intuitive and conversational user experience.
- Error Handling: The application handles various errors and edge cases, providing informative error messages to users and ensuring the reliability of the system.

### Technologies Used
- C++: The application is developed using C++ programming language, leveraging its efficiency and flexibility.
- OpenSSL: OpenSSL library is used for implementing secure TLS communication between the client and server.
- JSON: JSON format is employed for exchanging data between the client and server, ensuring compatibility and ease of parsing.
- Linux Sockets: The application utilizes Linux sockets to establish network connections between the client and server.

## **Client-Side**
### *Requirements*
1. Linux system
2. Internet connection
3. Server ssl certificate
4. C++ compiler (G++)

### *Building and Running*
1. Get the server.crt file from the server and place it in the same directory as the client application.
2. Build the client-side application using the command `make`.
    - If you experience issues with OpenSSL, it can be installed using the command `sudo apt-get install libssl-dev`. Some devices may need to run this but most do not.
3. Run the client-side application using the command `make run`.

### *Interaction*
1. Client will be asked if they have an existing account.
    - If they do, they will be prompted for a username and password
        - 3 attempts to successfully login
        - For testing purposes, test usernames/passwords are referenced in "passwords.txt", while their SHA256 hashes are stored in "users.txt" (the file that is used in the application)
    - If they do not, they will be prompted to create an account
2. Client will be asked if they would like to use natural language prompts.
    - If they say y/yes, they will be able to make requests in plain English (eg. "I want to take out a hundred dollars" will be understood by the application as a withdrawal of $100)
    - If they say n/no, they will be prompted for each action in list format (eg. Type 1 for withdraw, 2 for deposit, etc.)
3. Clients may request to withdraw money from their account.
    - If this amount exceeds their balance, they will be notified and transaction rejected.
    - If successful, amount will be withdrawn and user notified of new balance
4. Clients may request to deposit money to their account.
    - Account balance will be updated and user notified of new balance
5. Clients may request to transfer money.
    - If transfer amount exceeds balance, transaction will be rejected.
    - If transfer amount less than balance, user will be prompted whether they want to transfer to a NLPBanking client or external banking user.
        - If NLPBanking client, they will be asked for the name.
            - Name will then be verified by server that they are an existing client.
                - If verified, transfer will go through and client notified of new balance.
                - If nonexisting, user will be notified and transfer cancelled
        - If external banking user, clients can enter either an email address or phone number of recipient
            - User will be notified of pending transfer and their new balance
6. Clients may request to view their balance.
    - Balance will be displayed to user
7. Clients may request to view their transaction log.
    - Transaction log will be displayed to user with timestamps
8. Clients may request to change between NLP and non-NLP modes
9. Clients may request to logout.
    - Client will be logged out and SSL connection with server will be closed.


## **Server-Side**
### *Requirements*
1. Linux system
2. Access to port forwarding configuration on network
3. Access to device IP address configuration on network
4. Dependencies installed
5. C++ compiler (G++)
6. PM2 (optional)

### *Building and Deploying on a fresh Raspberry Pi*
1. Port forwarding must be enabled on network which Raspberry Pi is connected to (if you would like to connect from external IPs).
   1. Port 3001 must be forwarded to the Raspberry Pi.
   2. Raspberry Pi's IP address configuration must be set to "static".
   3. Public IPv4 address must be updated on the client-side application in client.cpp line 20: `const char* SERVER_IP = "INSERT_IP_HERE";`
2. Ensure the following external dependencies are installed
   1. libcurl (`sudo apt-get install libcurl4-openssl-dev`)
   2. jsoncpp (`sudo apt-get install libjsoncpp-dev`)
   3. openssl (`sudo apt-get install libssl-dev`)
3. Replace "API_KEY_HERE" with your OpenAI API key in line 44 in request.cpp: ```headers = curl_slist_append(headers, "Authorization: Bearer API_KEY_HERE");```
4. Create server.key and server.crt files using OpenSSL and add them to /backend directory. (share server.crt with client)
    1. To create server.key, run the command `openssl genrsa -out server.key 2048`
    2. To create server.crt, run the command `openssl req -new -x509 -key server.key -out server.crt -days 3650 -subj /CN=server`
5. Build the server-side application using the command `make`.
6. Run the server-side application using the command `make run`.
   1. Alternatively, PM2 can be used with the script "start_server.sh" to run the server in the background and ensure uptime.
      - Note that PM2 requires NodeJS to be installed on the system to install via npm with the command `npm install pm2 -g`
   2. To start this, run the command `pm2 start start_server.sh`
   3. Use command `pm2 logs` to view server logs
7. Server will then be listening for incoming connections on port 3001.
