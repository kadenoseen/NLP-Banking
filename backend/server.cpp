/**
 * @file server.cpp
 * @brief Starts a server to handle NLP Banking client requests.
 * Uses threads to handle many clients at once.
 * @author Kaden Oseen
 */

#include "server.h"

using namespace std;

const int PORT = 3001;

/**
 * @brief Handles a session with a client.
 * Creates a session for the client and starts the session. Shuts down the SSL afterwards and closes socket.
 * @param client_socket The socket to communicate with the client.
 */
void handle_session(int client_socket, SSL* ssl) {
    // Create a new Session object and start the session
    int testSSL = SSL_accept(ssl);
    if(testSSL == 1){
        cout << "SSL connection established" << endl;
    }else{
        cout << "SSL connection failed" << endl;
        return;
    }
    std::cerr << "SSL state: " << SSL_state_string(ssl) << std::endl;

    Session session(client_socket, ssl);
    session.start_session();

    // Clean up the SSL object and close the client socket
    cout << "Closing connection" << endl;
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client_socket);
}


/**
 * @brief Starts the server and listens for incoming client requests.
 * Initializes SSL, socket and address, and starts a new thread for each client.
 * @return int Exit code.
 */
int main() {
    // Initialize OpenSSL library
    SSL_library_init();
    SSL_load_error_strings();

    // Create a new SSL context
    SSL_CTX* ssl_ctx = SSL_CTX_new(TLS_server_method());

    // Load the server's certificate and private key
    SSL_CTX_use_certificate_file(ssl_ctx, "server.crt", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ssl_ctx, "server.key", SSL_FILETYPE_PEM);
    
    // Create a socket for the server to use
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Create a struct for the server address to bind to
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    // Listen for incoming client requests
    listen(server_socket, 5);

    cout << "Listening on port " << PORT << endl;

    // Continuously accept incoming client requests and handle sessions in separate threads
    while (true) {
        // Accept a new client connection and create a new SSL object
        int client_socket = accept(server_socket, nullptr, nullptr);
        SSL* ssl = SSL_new(ssl_ctx);
        SSL_set_fd(ssl, client_socket);

        // Create a new thread to handle the session
        thread t(handle_session, client_socket, ssl);
        // Detach the thread
        t.detach();
    }

    // Clean up the SSL context and close the server socket
    SSL_CTX_free(ssl_ctx);
    close(server_socket);
    return 0;
}
