#include "InputValidator.h"
#include <iostream>
#include <regex>
#include <sstream>
#include <algorithm>
#include <cstdio>

using namespace std;

string InputValidator::getString(const string& prompt, bool allowEmpty) {
    string s;
    while (true) {
        cout << prompt;
        cin >> ws; // Пропускаем пробелы
        getline(cin, s);
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
        cin >> ws; // Пропускаем пробелы
        getline(cin, input);
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
    while (true) {
        string pass = getString(prompt);
        if (pass.length() < 3) {
            cout << "Пароль должен содержать минимум 3 символа!\n";
            continue;
        }
        if (pass.find(',') != string::npos) {
            cout << "Пароль не может содержать запятую!\n";
            continue;
        }
        return pass;
    }
}

string InputValidator::getFullName(const string& prompt) {
    while (true) {
        string name = getString(prompt);
        // Уберём начальные/конечные пробелы
        auto first = name.find_first_not_of(" \t");
        auto last = name.find_last_not_of(" \t");
        if (first == string::npos) {
            cout << "ФИО не может состоять только из пробелов!\n";
            continue;
        }
        name = name.substr(first, last - first + 1);

        if (name.length() < 2) {
            cout << "ФИО должно содержать минимум 2 символа!\n";
            continue;
        }

        bool ok = true;
        for (unsigned char c : name) {
            // Разрешаем буквы любых алфавитов (байты >=128), а также пробел, дефис и апостроф
            if (c >= 128) continue; // часть UTF-8, принимаем
            if (isalpha(c) || c == ' ' || c == '-' || c == '\'') continue;
            // Запрещаем цифры и знаки пунктуации
            ok = false;
            break;
        }

        if (ok) {
            return name;
        }
        cout << "ФИО может содержать буквы, пробелы, дефисы и апострофы, без цифр и лишних символов!\n";
    }
}

string InputValidator::getGroup(const string& prompt) {
    while (true) {
        string group = getString(prompt);
        // Проверка: буквы и цифры, возможно дефисы
        regex r(R"(^[А-Яа-яЁёA-Za-z0-9\-]+$)");
        if (regex_match(group, r) && group.length() >= 1 && group.length() <= 20) {
            return group;
        }
        cout << "Группа должна содержать только буквы, цифры и дефисы (1-20 символов)!\n";
    }
}

string InputValidator::getSubject(const string& prompt) {
    while (true) {
        string subject = getString(prompt);
        if (subject.length() < 2) {
            cout << "Название предмета должно содержать минимум 2 символа!\n";
            continue;
        }
        if (subject.length() > 50) {
            cout << "Название предмета не должно превышать 50 символов!\n";
            continue;
        }
        // Цифры не допускаются в названии предмета
        if (subject.find_first_of("0123456789") != string::npos) {
            cout << "Название предмета не должно содержать цифры!\n";
            continue;
        }
        if (subject.find(',') != string::npos) {
            cout << "Название предмета не может содержать запятую!\n";
            continue;
        }
        return subject;
    }
}

string InputValidator::toLower(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}
