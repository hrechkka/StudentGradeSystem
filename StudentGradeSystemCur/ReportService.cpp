#include "ReportService.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;

void ReportService::printStudentTable(const vector<Student>& students) {
    cout << "\n┌────────────────────┬────────────────────┬────────────────────┬─────────┐\n";
    cout << "│ Логин              │ ФИО                │ Группа             │ Роль    │\n";
    cout << "├────────────────────┼────────────────────┼────────────────────┼─────────┤\n";
    
    for (const auto& s : students) {
        cout << "│ " << setw(18) << left << s.login
             << " │ " << setw(18) << left << s.fullName
             << " │ " << setw(18) << left << s.group
             << " │ " << (s.isAdmin ? "Администратор" : "Студент") << "     │\n";
    }
    cout << "└────────────────────┴────────────────────┴────────────────────┴─────────┘\n\n";
}

void ReportService::printOneStudentDetailed(const Student& student) {
    cout << "\n=== ЛИЧНЫЕ ДАННЫЕ ===\n";
    cout << "ФИО: " << student.fullName << "\n";
    cout << "Группа: " << student.group << "\n";
    cout << "Средний балл: " << fixed << setprecision(2) << student.getAverageScore() << "\n\n";

    if (!student.grades.empty()) {
        cout << "ОЦЕНКИ:\n";
        cout << "┌────────────────────┬───────┐\n";
        cout << "│ Предмет            │ Оценка│\n";
        cout << "├────────────────────┼───────┤\n";
        for (const auto& g : student.grades) {
            cout << "│ " << setw(18) << left << g.subject << " │ " << g.score << "     │\n";
        }
        cout << "└────────────────────┴───────┘\n\n";
    }

    if (!student.attendance.empty()) {
        cout << "ПОСЕЩАЕМОСТЬ:\n";
        cout << "┌────────────┬────────────┐\n";
        cout << "│ Дата       │ Присутствие│\n";
        cout << "├────────────┼────────────┤\n";
        for (const auto& a : student.attendance) {
            cout << "│ " << a.date << " │ " << (a.present ? "Да" : "Нет") << "        │\n";
        }
        cout << "└────────────┴────────────┘\n";
    }
}

void ReportService::printRatingTable(const vector<Student>& students) {
    vector<Student> sorted = students;
    sort(sorted.begin(), sorted.end(), [](const Student& a, const Student& b) {
        return a.getAverageScore() > b.getAverageScore();
    });

    cout << "\nРЕЙТИНГ СТУДЕНТОВ ПО СРЕДНЕМУ БАЛЛУ\n";
    cout << "┌─────┬────────────────────┬─────────┬─────────────┐\n";
    cout << "│ №   │ ФИО                │ Группа  │ Ср. балл    │\n";
    cout << "├─────┼────────────────────┼─────────┼─────────────┤\n";
    
    int place = 1;
    for (const auto& s : sorted) {
        if (s.grades.empty()) continue;
        cout << "│ " << setw(3) << right << place++ << " │ "
             << setw(18) << left << s.fullName << " │ "
             << setw(7) << left << s.group << " │ "
             << fixed << setprecision(2) << s.getAverageScore() << "        │\n";
    }
    cout << "└─────┴────────────────────┴─────────┴─────────────┘\n\n";
}

void ReportService::printAttendanceTable(const vector<Student>& students) {
    cout << "\nОБЩАЯ ПОСЕЩАЕМОСТЬ\n";
    cout << "┌────────────────────┬────────────────────┬──────────────┐\n";
    cout << "│ ФИО                │ Группа             │ % посещаемости│\n";
    cout << "├────────────────────┼────────────────────┼──────────────┤\n";
    
    for (const auto& s : students) {
        if (s.attendance.empty()) continue;
        int present = 0;
        for (const auto& a : s.attendance) if (a.present) present++;
        double percent = (double)present / s.attendance.size() * 100;
        
        cout << "│ " << setw(18) << left << s.fullName
             << " │ " << setw(18) << left << s.group
             << " │ " << fixed << setprecision(1) << percent << "%          │\n";
    }
    cout << "└────────────────────┴────────────────────┴──────────────┘\n\n";
}
