/**
 * @file requests.cpp
 * @brief Implementation of the Request class.
 * Makes requests to NLP server API in order to handle natural language input
 * @author Kaden Oseen
 */

#include "request.h"
using namespace std;

/**
 * @name Request
 * @brief Constructor for the Request class

 * @param input The user input to be sent to the NLP API
 */
Request::Request(string input) : input(input) {
    curl = curl_easy_init();
}


/**
 * @name ~Request
 * @brief Destructor for the Request class
 * Cleans up the curl object
 */
Request::~Request() {
    curl_easy_cleanup(curl);
}


/**
 * @name execute
 * @brief Execute the curl request to NLP API
 * Creates a request to the NLP API with the user input and system message
 * @return true If the request was successful
 * @return false If the request failed
 */
bool Request::execute() {
    // Set the headers
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    // Authorization token (should be stored in .env, but leaving hardcoded as it has a request limit and for ease of use by TA)
    headers = curl_slist_append(headers, "Authorization: Bearer API_KEY_HERE");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Set the request URL and body
    Json::Value requestBody;
    requestBody["model"] = "gpt-3.5-turbo";
    // Create the messages array for the model to process
    Json::Value messages(Json::arrayValue);
    // System message lets the model know what it's job is
    Json::Value systemMessage;
    systemMessage["role"] = "system";
    systemMessage["content"] = "You are a banking system where users use natural language to withdraw, deposit, transfer, check balance, view history, go back (backwards), view their options, or logout. \
    After a message from the user, you will respond with a single message. Give ONLY two words for what the user is trying to do in the exact format: (action,amount) \
    where action is a single word string (deposit, transfer, withdraw, balance, history, backwards, options or logout) and amount is a number with maximum 2 decimal places (0 for balance, history, backwards, options or logout). \
    If a user requests to change to regular prompts or or go back in any way, respond with (backwards,0). If a user does not specify an amount with a request such as deposit, withdraw or transfer, return -1 as the value.\
    Remove ALL spaces. If unknown or not 100% sure, return (unknown,0).";
    messages.append(systemMessage);
    // User message is the input from the user
    Json::Value userMessage;
    userMessage["role"] = "user";
    userMessage["content"] = input;
    messages.append(userMessage);
    requestBody["messages"] = messages;
    cout << requestBody << endl;

    // Convert the request body to a string
    std::string requestBodyString = requestBody.toStyledString();

    // Set the request URL and body
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBodyString.c_str());

    // Set the write callback function to receive the response
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &Request::writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Execute the request
    CURLcode res = curl_easy_perform(curl);

    // Check if request failed
    if (res != CURLE_OK) {
        cerr << "curl request failed: " << curl_easy_strerror(res) << endl;
        return false;
    }

    // Parse the JSON response
    Json::Value root;
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();
    std::string errors;

    if (!reader->parse(response.c_str(), response.c_str() + response.size(), &root, &errors)) {
        cerr << "Failed to parse response: " << errors << endl;
        return "";
    }

    // Set response to "assistant" response from model
    response = root["choices"][0]["message"]["content"].asString();

    // Return true if request succeeded
    return true;
}


 
/**
 * @name result
 * @brief Get the result of the curl request
 * 
 * @return string The formatted response from the request
 */
string Request::result() {
    return response;
}


/**
 * @name writeCallback
 * @brief Callback function for the curl request
 * 
 * @param contents The contents of the request
 * @param size The size of each item in the contents
 * @param nmemb The number of members in the contents
 * @param userp Pointer to the json string
 * @return size_t The size of the contents
 */
size_t Request::writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    // Append the contents of the response to jsonData string
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
