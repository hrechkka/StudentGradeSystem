#pragma once
#include <string>

class InputValidator {
public:
    static std::string getString(const std::string& prompt, bool allowEmpty = false);
    static int getInt(const std::string& prompt, int min = 0, int max = 10);
    static std::string getDate(const std::string& prompt);
    static bool isValidDate(const std::string& date);
    static std::string getLogin(const std::string& prompt);
    static std::string getPassword(const std::string& prompt);
};
