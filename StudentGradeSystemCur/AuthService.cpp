#include "AuthService.h"
#include "Hash.h"
#include "InputValidator.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

AuthService::AuthService() {
    loadStudents();
}

void AuthService::loadStudents() {
    ifstream file("data/users.txt");
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        Student s;
        string adminStr;

        getline(ss, s.login, ',');
        getline(ss, s.passwordHash, ',');
        getline(ss, adminStr, ',');
        string idStr;
        getline(ss, idStr, ',');
        getline(ss, s.fullName, ',');
        getline(ss, s.group, ',');

        s.id = stoull(idStr);
        s.isAdmin = (adminStr == "1");
        if (s.id >= nextId) nextId = s.id + 1;

        students.push_back(s);
    }
    file.close();
}

void AuthService::saveAllStudents() {
    ofstream file("data/users.txt");
    for (const auto& s : students) {
        file << s.login << ","
             << s.passwordHash << ","
             << (s.isAdmin ? "1" : "0") << ","
             << s.id << ","
             << s.fullName << ","
             << s.group << "\n";
    }
    file.close();
}

Student* AuthService::findUser(const string& login) {
    for (auto& s : students) {
        if (s.login == login) return &s;
    }
    return nullptr;
}

bool AuthService::login(const string& login, const string& password, Student& currentUser) {
    Student* user = findUser(login);
    if (user && user->passwordHash == Hash::simpleHash(password)) {
        currentUser = *user;
        return true;
    }
    return false;
}

bool AuthService::registerUser(const string& login, const string& password,
                               const string& fullName, const string& group, bool isAdmin) {
    if (findUser(login) != nullptr) {
        cout << "Ошибка: пользователь с таким логином уже существует!\n";
        return false;
    }

    Student s;
    s.id = nextId++;
    s.login = login;
    s.passwordHash = Hash::simpleHash(password);
    s.fullName = fullName;
    s.group = group;
    s.isAdmin = isAdmin;

    students.push_back(s);
    saveAllStudents();
    cout << "Пользователь успешно зарегистрирован!\n";
    return true;
}

vector<Student> AuthService::getAllStudents() const {
    return students;
}
