#include "StudentService.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

void StudentService::addGrade(Student& student, const string& subject, int score) {
    for (auto& g : student.grades) {
        if (g.subject == subject) {
            g.score = score;
            cout << "Оценка по предмету '" << subject << "' обновлена!\n";
            return;
        }
    }
    student.grades.push_back({subject, score});
    cout << "Оценка по предмету '" << subject << "' добавлена!\n";
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
    ofstream gradesFile("data/grades.txt");
    ofstream attFile("data/attendance.txt");

    for (const auto& s : students) {
        for (const auto& g : s.grades) {
            gradesFile << s.id << "," << g.subject << "," << g.score << "\n";
        }
        for (const auto& a : s.attendance) {
            attFile << s.id << "," << a.date << "," << (a.present ? "1" : "0") << "\n";
        }
    }
}

void StudentService::loadGradesAndAttendance(vector<Student>& students) {
    ifstream gradesFile("data/grades.txt");
    if (gradesFile.is_open()) {
        string line;
        while (getline(gradesFile, line)) {
            stringstream ss(line);
            string idStr, subject, scoreStr;
            getline(ss, idStr, ',');
            getline(ss, subject, ',');
            getline(ss, scoreStr, ',');

            size_t id = stoull(idStr);
            int score = stoi(scoreStr);

            for (auto& s : students) {
                if (s.id == id) {
                    s.grades.push_back({subject, score});
                }
            }
        }
    }

    ifstream attFile("data/attendance.txt");
    if (attFile.is_open()) {
        string line;
        while (getline(attFile, line)) {
            stringstream ss(line);
            string idStr, date, presentStr;
            getline(ss, idStr, ',');
            getline(ss, date, ',');
            getline(ss, presentStr, ',');

            size_t id = stoull(idStr);
            bool present = (presentStr == "1");

            for (auto& s : students) {
                if (s.id == id) {
                    s.attendance.push_back({date, present});
                }
            }
        }
    }
}
