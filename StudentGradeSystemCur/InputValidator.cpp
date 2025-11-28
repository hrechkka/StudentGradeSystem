#include "InputValidator.h"
#include <iostream>
#include <regex>
#include <sstream>
#include <cstdio>

using namespace std;

string InputValidator::getString(const string& prompt, bool allowEmpty) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin >> ws, s);
        if (!allowEmpty && s.empty()) {
            cout << "Ошибка: поле не может быть пустым!\n";
        } else {
            return s;
        }
    }
}

int InputValidator::getInt(const string& prompt, int min, int max) {
    string input;
    int value;
    while (true) {
        cout << prompt;
        getline(cin >> ws, input);
        stringstream ss(input);
        if (ss >> value && ss.eof() && value >= min && value <= max) {
            return value;
        }
        cout << "Введите число от " << min << " до " << max << "!\n";
    }
}

string InputValidator::getDate(const string& prompt) {
    while (true) {
        string date = getString(prompt);
        if (isValidDate(date)) return date;
        cout << "Неверный формат! Используйте YYYY-MM-DD (например: 2025-04-05)\n";
    }
}

bool InputValidator::isValidDate(const string& date) {
    regex r(R"(^\d{4}-\d{2}-\d{2}$)");
    if (!regex_match(date, r)) return false;
    
    int y, m, d;
    if (sscanf(date.c_str(), "%d-%d-%d", &y, &m, &d) != 3) return false;
    return (y >= 2000 && y <= 2100 && m >= 1 && m <= 12 && d >= 1 && d <= 31);
}

string InputValidator::getLogin(const string& prompt) {
    while (true) {
        string login = getString(prompt);
        if (login.find(',') == string::npos && !login.empty()) {
            return login;
        }
        cout << "Логин не может содержать запятую!\n";
    }
}

string InputValidator::getPassword(const string& prompt) {
    return getString(prompt);
}
