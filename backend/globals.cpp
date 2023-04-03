/**
 * @file globals.cpp
 * @brief Global variable and function definitions
 * Stores global mutex and mapping for active sessions to ensure one login per user
 * Also contains general use functions such as hashing and timestamping
 * @author Kaden Oseen
*/

#include "globals.h"

using namespace std;

// Global variables for storing active sessions
unordered_map<string, Session*> active_sessions;
mutex active_sessions_mutex;


/**
 * @brief Hashes a string using SHA256
 * Allows for secure password storage and verification through user knowledge only
 * @param str String to be hashed
 * @return Hashed string
 */
string get_hash(const string& str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(str.c_str()), str.size(), hash);
    stringstream ss;
    ss << hex << setfill('0');
    for (unsigned char i : hash) {
        ss << setw(2) << static_cast<unsigned int>(i);
    }
    return ss.str();
}


/**
 * @brief Returns the current timestamp in the format [YYYY-MM-DD HH:MM:SS]
 * Used to get timestamps for log messages
 * @return Current timestamp
 */
string getTimestamp() {
    auto currentTime = chrono::system_clock::now();
    time_t time = chrono::system_clock::to_time_t(currentTime);
    tm localTime = *localtime(&time);
    stringstream timestamp;
    timestamp << put_time(&localTime, "[%Y-%m-%d %H:%M:%S]");
    return timestamp.str();
}

/**
 * @brief Removes all characters except for numbers and decimal points from a string
 * Used to filter out unwanted characters from user input such as dollar signs
 * @param str String to be filtered
 * @return Filtered string
*/
string removeCharacters(string str) {
    string new_string = str;
    regex regex_filter("[^0-9.]");
    string result = regex_replace(new_string, regex_filter, "");
    return result;
}