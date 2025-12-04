#include "ReportService.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>

using namespace std;

namespace {
constexpr size_t COL_LOGIN = 20;
constexpr size_t COL_NAME  = 20;
constexpr size_t COL_GROUP = 20;
constexpr size_t COL_ROLE  = 14; // расширено под "Администратор"
constexpr size_t COL_ATTEND_NAME = 20;
constexpr size_t COL_ATTEND_GROUP = 10;
constexpr size_t COL_ATTEND_PERCENT = 16;
constexpr size_t COL_ATT_DATE = 12;
constexpr size_t COL_ATT_PRESENT = 12;

string repeatChar(const string& ch, size_t n) {
    string res;
    res.reserve(ch.size() * n);
    for (size_t i = 0; i < n; ++i) res += ch;
    return res;
}

// Подсчёт количества UTF-8 символов
size_t utf8Length(const string& s) {
    size_t count = 0;
    for (size_t i = 0; i < s.size();) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        size_t charLen = 1;
        if ((c & 0x80) == 0x00) charLen = 1;
        else if ((c & 0xE0) == 0xC0) charLen = 2;
        else if ((c & 0xF0) == 0xE0) charLen = 3;
        else if ((c & 0xF8) == 0xF0) charLen = 4;
        i += charLen;
        ++count;
    }
    return count;
}

// Аккуратно обрезает UTF-8 строки до maxChars с учётом троеточия
string truncateUtf8(const string& s, size_t maxChars) {
    size_t length = utf8Length(s);
    if (length <= maxChars) return s;
    if (maxChars <= 3) return s.substr(0, maxChars); // крайний случай

    size_t target = maxChars - 3; // место под "..."
    size_t count = 0;
    size_t i = 0;
    while (i < s.size() && count < target) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        size_t charLen = 1;
        if ((c & 0x80) == 0x00) charLen = 1;
        else if ((c & 0xE0) == 0xC0) charLen = 2;
        else if ((c & 0xF0) == 0xE0) charLen = 3;
        else if ((c & 0xF8) == 0xF0) charLen = 4;
        if (i + charLen > s.size()) break;
        i += charLen;
        ++count;
    }
    return s.substr(0, i) + "...";
}

// Дополняет строку пробелами до нужной длины в символах
string padUtf8(const string& s, size_t width) {
    size_t len = utf8Length(s);
    if (len >= width) return s;
    return s + string(width - len, ' ');
}

// Готовит ячейку таблицы: обрезка и выравнивание
string formatCell(const string& s, size_t width) {
    return padUtf8(truncateUtf8(s, width), width);
}
}

void ReportService::printStudentTable(const vector<Student>& students) {
    cout << "\n┌" << repeatChar("─", COL_LOGIN + 2)
         << "┬" << repeatChar("─", COL_NAME + 2)
         << "┬" << repeatChar("─", COL_GROUP + 2)
         << "┬" << repeatChar("─", COL_ROLE + 2) << "┐\n";

    cout << "│ " << formatCell("Логин", COL_LOGIN)
         << " │ " << formatCell("ФИО", COL_NAME)
         << " │ " << formatCell("Группа", COL_GROUP)
         << " │ " << formatCell("Роль", COL_ROLE) << " │\n";

    cout << "├" << repeatChar("─", COL_LOGIN + 2)
         << "┼" << repeatChar("─", COL_NAME + 2)
         << "┼" << repeatChar("─", COL_GROUP + 2)
         << "┼" << repeatChar("─", COL_ROLE + 2) << "┤\n";
    
    if (students.empty()) {
        cout << "│ " << formatCell("", COL_LOGIN)
             << " │ " << formatCell("Нет данных", COL_NAME)
             << " │ " << formatCell("", COL_GROUP)
             << " │ " << formatCell("", COL_ROLE) << " │\n";
    } else {
        for (const auto& s : students) {
            string role = s.isAdmin ? "Администратор" : "Студент";
            string login = formatCell(s.login, COL_LOGIN);
            string name = formatCell(s.fullName, COL_NAME);
            string group = formatCell(s.group, COL_GROUP);
            string roleCell = formatCell(role, COL_ROLE);
            
            cout << "│ " << login
                 << " │ " << name
                 << " │ " << group
                 << " │ " << roleCell << " │\n";
        }
    }
    cout << "└" << repeatChar("─", COL_LOGIN + 2)
         << "┴" << repeatChar("─", COL_NAME + 2)
         << "┴" << repeatChar("─", COL_GROUP + 2)
         << "┴" << repeatChar("─", COL_ROLE + 2) << "┘\n\n";
}

void ReportService::printOneStudentDetailed(const Student& student) {
    cout << "\n=== ЛИЧНЫЕ ДАННЫЕ ===\n";
    cout << "ФИО: " << student.fullName << "\n";
    cout << "Группа: " << student.group << "\n";
    cout << "Средний балл: " << fixed << setprecision(2) << student.getAverageScore() << "\n\n";

    if (!student.grades.empty()) {
        cout << "ОЦЕНКИ:\n";
        cout << "┌──────────────────────┬────────┐\n";
        cout << "│ Предмет              │ Оценка │\n";
        cout << "├──────────────────────┼────────┤\n";
        for (const auto& g : student.grades) {
            string subject = formatCell(g.subject, 20);
            cout << "│ " << subject
                 << " │ " << setw(6) << right << g.score << " │\n";
        }
        cout << "└──────────────────────┴────────┘\n\n";
    }

    if (!student.attendance.empty()) {
        cout << "ПОСЕЩАЕМОСТЬ:\n";
        cout << "┌" << repeatChar("─", COL_ATT_DATE + 2)
             << "┬" << repeatChar("─", COL_ATT_PRESENT + 2) << "┐\n";
        cout << "│ " << formatCell("Дата", COL_ATT_DATE)
             << " │ " << formatCell("Присутствие", COL_ATT_PRESENT) << " │\n";
        cout << "├" << repeatChar("─", COL_ATT_DATE + 2)
             << "┼" << repeatChar("─", COL_ATT_PRESENT + 2) << "┤\n";
        for (const auto& a : student.attendance) {
            string present = a.present ? "Да" : "Нет";
            cout << "│ " << formatCell(a.date, COL_ATT_DATE)
                 << " │ " << formatCell(present, COL_ATT_PRESENT) << " │\n";
        }
        cout << "└" << repeatChar("─", COL_ATT_DATE + 2)
             << "┴" << repeatChar("─", COL_ATT_PRESENT + 2) << "┘\n";
    }
}

void ReportService::printRatingTable(const vector<Student>& students) {
    vector<Student> sorted = students;
    sort(sorted.begin(), sorted.end(), [](const Student& a, const Student& b) {
        return a.getAverageScore() > b.getAverageScore();
    });

    cout << "\nРЕЙТИНГ СТУДЕНТОВ ПО СРЕДНЕМУ БАЛЛУ\n";
    cout << "┌─────┬──────────────────────┬────────────┬──────────────┐\n";
    cout << "│ №   │ ФИО                  │ Группа     │ Ср. балл     │\n";
    cout << "├─────┼──────────────────────┼────────────┼──────────────┤\n";
    
    int place = 1;
    bool hasData = false;
    for (const auto& s : sorted) {
        if (s.grades.empty()) continue;
        hasData = true;
        string name = formatCell(s.fullName, 20);
        string group = formatCell(s.group, 10);
        cout << "│ " << setw(3) << right << place++ << " │ "
             << name << " │ "
             << group << " │ "
             << setw(12) << right << fixed << setprecision(2) << s.getAverageScore() << " │\n";
    }
    if (!hasData) {
        cout << "│     │ Нет данных           │            │              │\n";
    }
    cout << "└─────┴──────────────────────┴────────────┴──────────────┘\n\n";
}

void ReportService::printAttendanceTable(const vector<Student>& students) {
    cout << "\nОБЩАЯ ПОСЕЩАЕМОСТЬ\n";
    cout << "┌" << repeatChar("─", COL_ATTEND_NAME + 2)
         << "┬" << repeatChar("─", COL_ATTEND_GROUP + 2)
         << "┬" << repeatChar("─", COL_ATTEND_PERCENT + 2) << "┐\n";
    cout << "│ " << formatCell("ФИО", COL_ATTEND_NAME)
         << " │ " << formatCell("Группа", COL_ATTEND_GROUP)
         << " │ " << formatCell("% посещаемости", COL_ATTEND_PERCENT) << " │\n";
    cout << "├" << repeatChar("─", COL_ATTEND_NAME + 2)
         << "┼" << repeatChar("─", COL_ATTEND_GROUP + 2)
         << "┼" << repeatChar("─", COL_ATTEND_PERCENT + 2) << "┤\n";
    
    bool hasData = false;
    for (const auto& s : students) {
        if (s.attendance.empty()) continue;
        hasData = true;
        int present = 0;
        for (const auto& a : s.attendance) if (a.present) present++;
        double percent = (double)present / s.attendance.size() * 100;
        string percentStr = (ostringstream{} << fixed << setprecision(1) << percent << "%").str();
        
        string name = formatCell(s.fullName, COL_ATTEND_NAME);
        string group = formatCell(s.group, COL_ATTEND_GROUP);
        string percentCell = formatCell(percentStr, COL_ATTEND_PERCENT);
        
        cout << "│ " << name
             << " │ " << group
             << " │ " << percentCell << " │\n";
    }
    if (!hasData) {
        cout << "│ " << formatCell("Нет данных", COL_ATTEND_NAME)
             << " │ " << formatCell("", COL_ATTEND_GROUP)
             << " │ " << formatCell("", COL_ATTEND_PERCENT) << " │\n";
    }
    cout << "└" << repeatChar("─", COL_ATTEND_NAME + 2)
         << "┴" << repeatChar("─", COL_ATTEND_GROUP + 2)
         << "┴" << repeatChar("─", COL_ATTEND_PERCENT + 2) << "┘\n\n";
}
