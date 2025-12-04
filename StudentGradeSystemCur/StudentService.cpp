#include "StudentService.h"
#include "AuthService.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <algorithm>

using namespace std;

void StudentService::addGrade(Student& student, const string& subject, int score) {
    for (auto& g : student.grades) {
        if (g.subject == subject) {
            g.score = score;
            cout << "Оценка по предмету '" << subject << "' обновлена!\n";
            logGradeHistory(student.id, subject, score);
            return;
        }
    }
    student.grades.push_back({subject, score});
    cout << "Оценка по предмету '" << subject << "' добавлена!\n";
    logGradeHistory(student.id, subject, score);
}

void StudentService::addAttendance(Student& student, const string& date, bool present) {
    for (auto& a : student.attendance) {
        if (a.date == date) {
            a.present = present;
            cout << "Посещаемость за " << date << " обновлена!\n";
            return;
        }
    }
    student.attendance.push_back({date, present});
    cout << "Посещаемость за " << date << " добавлена!\n";
}

void StudentService::saveGradesAndAttendance(const vector<Student>& students) {
    string dataPath = AuthService::getDataPath();
    ofstream gradesFile(dataPath + "grades.txt");
    ofstream attFile(dataPath + "attendance.txt");

    for (const auto& s : students) {
        for (const auto& g : s.grades) {
            gradesFile << s.id << "," << g.subject << "," << g.score << "\n";
        }
        for (const auto& a : s.attendance) {
            attFile << s.id << "," << a.date << "," << (a.present ? "1" : "0") << "\n";
        }
    }
    gradesFile.close();
    attFile.close();
}

void StudentService::loadGradesAndAttendance(vector<Student>& students) {
    // Сначала очистим текущие данные, чтобы избежать дублирования при повторных загрузках
    for (auto& s : students) {
        s.grades.clear();
        s.attendance.clear();
    }

    string dataPath = AuthService::getDataPath();
    ifstream gradesFile(dataPath + "grades.txt");
    if (gradesFile.is_open()) {
        string line;
        while (getline(gradesFile, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string idStr, subject, scoreStr;
            getline(ss, idStr, ',');
            getline(ss, subject, ',');
            getline(ss, scoreStr, ',');

            if (idStr.empty() || subject.empty() || scoreStr.empty()) continue;
            
            try {
                size_t id = stoull(idStr);
                int score = stoi(scoreStr);

                for (auto& s : students) {
                    if (s.id == id) {
                        auto it = std::find_if(s.grades.begin(), s.grades.end(), [&](const Grade& g){
                            return g.subject == subject;
                        });
                        if (it != s.grades.end()) {
                            it->score = score; // заменяем последним значением
                        } else {
                            s.grades.push_back({subject, score});
                        }
                    }
                }
            } catch (...) {
                // Пропускаем некорректные строки
                continue;
            }
        }
        gradesFile.close();
    }

    ifstream attFile(dataPath + "attendance.txt");
    if (attFile.is_open()) {
        string line;
        while (getline(attFile, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string idStr, date, presentStr;
            getline(ss, idStr, ',');
            getline(ss, date, ',');
            getline(ss, presentStr, ',');

            if (idStr.empty() || date.empty() || presentStr.empty()) continue;
            
            try {
                size_t id = stoull(idStr);
                bool present = (presentStr == "1");

                for (auto& s : students) {
                    if (s.id == id) {
                        auto it = std::find_if(s.attendance.begin(), s.attendance.end(), [&](const Attendance& a){
                            return a.date == date;
                        });
                        if (it != s.attendance.end()) {
                            it->present = present; // заменяем последним значением
                        } else {
                            s.attendance.push_back({date, present});
                        }
                    }
                }
            } catch (...) {
                // Пропускаем некорректные строки
                continue;
            }
        }
        attFile.close();
    }
}

void StudentService::logGradeHistory(size_t studentId, const string& subject, int score) {
    string dataPath = AuthService::getDataPath();
    ofstream logFile(dataPath + "grade_history.txt", ios::app);
    if (!logFile.is_open()) {
        cerr << "Не удалось открыть файл истории оценок для записи!\n";
        return;
    }

    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    tm* tmPtr = localtime(&t);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tmPtr);

    logFile << studentId << "," << buf << "," << subject << "," << score << "\n";
    logFile.close();
}

vector<GradeHistoryEntry> StudentService::loadGradeHistory(size_t studentId) {
    vector<GradeHistoryEntry> history;
    string dataPath = AuthService::getDataPath();
    ifstream logFile(dataPath + "grade_history.txt");
    if (!logFile.is_open()) {
        return history;
    }

    string line;
    while (getline(logFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string idStr, timestamp, subject, scoreStr;
        getline(ss, idStr, ',');
        getline(ss, timestamp, ',');
        getline(ss, subject, ',');
        getline(ss, scoreStr, ',');

        if (idStr.empty() || subject.empty() || scoreStr.empty()) continue;

        try {
            size_t id = stoull(idStr);
            if (id != studentId) continue;
            int score = stoi(scoreStr);
            GradeHistoryEntry entry;
            entry.studentId = id;
            entry.timestamp = timestamp;
            entry.subject = subject;
            entry.score = score;
            history.push_back(entry);
        } catch (...) {
            continue;
        }
    }
    return history;
}
