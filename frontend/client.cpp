/**
 * @file client.cpp
 * @brief Client side application that connects to the NLP Banking server.
 * Handles all user interaction and sends/receives messages to/from the server.
 * @author Kaden Oseen
*/

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

using namespace std;

const int PORT = 3001;
const char* SERVER_IP = "INSERT_IP_HERE";

/**
 * @brief Connects to the NLP Banking server using TLS and interacts with the user through the terminal.
 * 
 * @return int Exit code.
 */
int main() {
    // Initialize the OpenSSL library and load the necessary algorithms and error messages
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    // Create a socket for the client to use
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Check if socket was created successfully
    if (client_socket < 0) {
        cerr << "Error: client socket creation failed" << endl;
        return 1;
    }

    // Create a struct for the server address to connect to
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Set the IP address of the server
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Attempt to connect to the server with the given address
    int connect_result = connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address));

    // Check if the connection was successful
    if (connect_result < 0) {
        cerr << "Failed to connect to server\n";
        return 1;
    }

    // Create a context object to hold the TLS configuration
    SSL_CTX* ssl_ctx = SSL_CTX_new(TLS_client_method());


    // Load the server's certificate
    if (SSL_CTX_load_verify_locations(ssl_ctx, "server.crt", nullptr) != 1) {
        cerr << "Failed to load server certificate" << endl;
        return 1;
    }

    // Create a secure socket object for the client to use
    SSL* ssl = SSL_new(ssl_ctx);

    // Set the socket to use TLS by calling the SSL_set_fd() function and passing in the client socket
    SSL_set_fd(ssl, client_socket);

    // Initiate the TLS handshake by calling the SSL_connect() function
    int ssl_connect_result = SSL_connect(ssl);

    // Check if the TLS handshake was successful
    if (ssl_connect_result != 1) {
        cerr << "Failed to establish TLS connection\n";
        cerr << "SSL state: " << SSL_state_string(ssl) << endl;
        return 1;
    }

    // Set finished flag to false to indicate session has started
    bool finished = false;

    // Loop while session is ongoing
    while (!finished){
        // Initialize message buffer
        char message[1024];
        memset(message, 0, sizeof(message));

        // Receive response from server using SSL_read() function
        int bytes_received = SSL_read(ssl, message, 1024);
        message[bytes_received] = '\0';

        // Check if no bytes were received (connection closed)
        if (bytes_received == 0){
            break;
        }

        // Check if message is a special code indicating user is already logged in
        if(strcmp(message, "\n101") == 0){
            cout << "You are already logged in on another instance!" << endl;
            break;
        }
        // Check if message is a special code indicating username already exists
        else if(strcmp(message, "\n104") == 0){
            cout << "Username already exists." << endl;
            break;
        }
        // Check if message is a special code indicating user is logging out
        else if(strcmp(message, "\n105") == 0){
            cout << "Logging out..." << endl;
            break;
        }
        // Check if message is a special code indicating user has been locked out
        else if(strcmp(message, "\n106") == 0){
            cout << "Too many login attempts, exiting..." << endl;
            break;
        }

        // Print server response to console
        cout << message << endl;

        // Get user input from console
        cin.getline(message, 1024);

        // Check if user wants to exit session
        if (strcmp(message, "exit") == 0) {
            cout << "Exiting..." << endl;
            break;
        }

        // Check if message is too long for buffer
        if (strlen(message) >= 1024) {
            cerr << "Error: message too long" << endl;
            continue;
        }

        // Send user input to server using SSL_write() function
        int send_result = SSL_write(ssl, message, strlen(message));

        // Check if message was sent successfully
        if (send_result == -1) {
            cerr << "Failed to send message" << endl;
            return 1;
        }
    }


    // Clean up SSL objects and context
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ssl_ctx);

    // Close the client socket
    close(client_socket);

    // Clean up the OpenSSL library
    ERR_free_strings();
    EVP_cleanup();
    return 0;
}