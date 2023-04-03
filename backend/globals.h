/**
 * @file globals.h
 * @brief Global variable declarations
 * @author Kaden Oseen
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <chrono>
#include <regex>

// Forward declaration of Session class
class Session;

// Global variables
extern std::unordered_map<std::string, Session*> active_sessions;
extern std::mutex active_sessions_mutex;

// Global general use functions
std::string get_hash(const std::string& str);
std::string getTimestamp();
std::string removeCharacters(std::string str);

#endif
