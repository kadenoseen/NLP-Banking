/**
 * @file requests.h
 * @brief Declaration of the Request class.
 * @author Kaden Oseen
 */

#ifndef REQUEST_H
#define REQUEST_H

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>

/**
 * @class Request
 * @brief Class for making curl requests
 * 
 * @param input The user input to send to the server
 */
class Request {
    private:
        // Curl object and url
        CURL* curl;
        std::string input;
        std::string response;
    public:
        // Constructor and destructor
        Request(std::string input);
        ~Request();
        // Methods
        bool execute();
        std::string result();
    private:
        // Callback function for writing the response
        static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);
};

#endif
