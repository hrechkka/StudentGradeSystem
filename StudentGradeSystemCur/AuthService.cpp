#include "AuthService.h"
#include "Hash.h"
#include "InputValidator.h"
#include "StudentService.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <cstdlib>
#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

using namespace std;

// Функция для определения правильного пути к папке data
static string findDataPath(const string& filename) {
    // Список путей для поиска файла
    vector<string> paths = {
        "data/" + filename,                                    // Текущая директория
        "StudentGradeSystemCur/data/" + filename,               // Относительно проекта
        "../data/" + filename,                                  // На уровень выше
        "../../data/" + filename,                               // На два уровня выше
        "../../../data/" + filename,                            // На три уровня выше
        "./data/" + filename,                                   // Текущая директория (явно)
        "/Users/Chamomile/Desktop/BSUIR/Курсовая/StudentGradeSystemCur/data/" + filename  // Абсолютный путь к исходникам
    };
    
    // Пробуем найти файл по каждому пути
    for (const auto& path : paths) {
        ifstream testFile(path);
        if (testFile.is_open()) {
            testFile.close();
            // Возвращаем базовый путь (без имени файла)
            size_t pos = path.find_last_of('/');
            if (pos != string::npos) {
                return path.substr(0, pos + 1);
            }
            return "data/";
        }
    }
    
    // Если файл не найден, пробуем создать папку data в текущей директории
    namespace fs = std::filesystem;
    if (!fs::exists("data")) {
        fs::create_directory("data");
    }
    return "data/";
}

// Статическая переменная для хранения базового пути
static string dataBasePath = "";

AuthService::AuthService() {
    if (dataBasePath.empty()) {
        dataBasePath = findDataPath("users.txt");
    }
    loadStudents();
}

void AuthService::loadStudents() {
    string filePath = dataBasePath + "users.txt";
    ifstream file(filePath);
    if (!file.is_open()) {
        return;
    }
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
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

        // Убираем возможные пробелы
        s.login.erase(0, s.login.find_first_not_of(" \t"));
        s.login.erase(s.login.find_last_not_of(" \t") + 1);
        s.passwordHash.erase(0, s.passwordHash.find_first_not_of(" \t"));
        s.passwordHash.erase(s.passwordHash.find_last_not_of(" \t") + 1);

        s.id = stoull(idStr);
        s.isAdmin = (adminStr == "1");
        if (s.id >= nextId) nextId = s.id + 1;

        students.push_back(s);
    }
    file.close();
}

void AuthService::saveAllStudents() {
    string filePath = dataBasePath + "users.txt";
    ofstream file(filePath);
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
    string lowerLogin = login;
    transform(lowerLogin.begin(), lowerLogin.end(), lowerLogin.begin(), ::tolower);
    for (auto& s : students) {
        string lowerStudentLogin = s.login;
        transform(lowerStudentLogin.begin(), lowerStudentLogin.end(), lowerStudentLogin.begin(), ::tolower);
        if (lowerStudentLogin == lowerLogin) return &s;
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

vector<Student>& AuthService::getStudentsRef() {
    return students;
}

void AuthService::loadGradesAndAttendance() {
    StudentService::loadGradesAndAttendance(students);
}

string AuthService::getDataPath() {
    return dataBasePath.empty() ? "data/" : dataBasePath;
}
