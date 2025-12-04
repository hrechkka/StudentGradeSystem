#include "Menu.h"
#include "InputValidator.h"
#include "StudentService.h"
#include "ReportService.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <unistd.h>

using namespace std;

namespace {
// Аккуратно обрезает UTF-8 строки, не разрывая символы
string truncateUtf8(const string& s, size_t maxChars) {
    size_t count = 0;
    size_t i = 0;
    while (i < s.size() && count < maxChars) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        size_t charLen = 1;
        if ((c & 0x80) == 0x00) {
            charLen = 1;
        } else if ((c & 0xE0) == 0xC0) {
            charLen = 2;
        } else if ((c & 0xF0) == 0xE0) {
            charLen = 3;
        } else if ((c & 0xF8) == 0xF0) {
            charLen = 4;
        }
        if (i + charLen > s.size()) break;
        i += charLen;
        ++count;
    }
    if (i < s.size()) {
        return s.substr(0, i) + "...";
    }
    return s;
}

vector<Student> nonAdminStudents(const vector<Student>& students) {
    vector<Student> result;
    result.reserve(students.size());
    for (const auto& s : students) {
        if (!s.isAdmin) result.push_back(s);
    }
    return result;
}

string repeatChar(const string& ch, size_t n) {
    string res;
    res.reserve(ch.size() * n);
    for (size_t i = 0; i < n; ++i) res += ch;
    return res;
}

void waitForEnter() {
    cout << "\nНажмите Enter, чтобы продолжить...";
    cout.flush();
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
}

Menu::Menu() {
    // Загружаем оценки и посещаемость при старте
    auth.loadGradesAndAttendance();
}

void Menu::showLoginScreen() {
    while (true) {
        system("clear"); // cls для Windows
        cout << "╔══════════════════════════════════════════════════════╗\n";
        cout << "║          СИСТЕМА УЧЁТА УСПЕВАЕМОСТИ                  ║\n";
        cout << "╚══════════════════════════════════════════════════════╝\n\n";
        cout << "Для входа введите логин и пароль.\n";
        cout << "Пример: логин 'admin', пароль 'admin'\n\n";
        string login = InputValidator::getLogin("Логин: ");
        string pass = InputValidator::getPassword("Пароль: ");

        if (auth.login(login, pass, currentUser)) {
            // Загружаем актуальные данные студента с оценками и посещаемостью
            auth.loadGradesAndAttendance();
            for (const auto& s : auth.getAllStudents()) {
                if (s.id == currentUser.id) {
                    currentUser = s;
                    break;
                }
            }
            cout << "Добро пожаловать, " << currentUser.fullName << "!\n";
            // Небольшая пауза для отображения сообщения
            usleep(800000); // 0.8 секунды
            if (currentUser.isAdmin) adminMenu();
            else studentMenu();
        } else {
            cout << "Неверный логин или пароль!\n";
            usleep(1500000); // 1.5 секунды
        }
    }
}

void Menu::adminMenu() {
    while (true) {
        system("clear");
        cout << "╔════════════════════════════════════════════════════════════╗\n";
        cout << "║           ПАНЕЛЬ АДМИНИСТРАТОРА                            ║\n";
        cout << "╚════════════════════════════════════════════════════════════╝\n\n";
        cout << "Пользователь: " << currentUser.fullName << "\n";
        cout << "Группа: " << currentUser.group << "\n\n";
        cout << "┌────────────────────────────────────────────────────────────┐\n";
        cout << "│  ДОСТУПНЫЕ ФУНКЦИИ:                                        │\n";
        cout << "├────────────────────────────────────────────────────────────┤\n";
        cout << "│  1. Добавить пользователя                                  │\n";
        cout << "│     → Регистрация нового студента или администратора       │\n";
        cout << "│                                                            │\n";
        cout << "│  2. Просмотреть всех студентов                             │\n";
        cout << "│     → Список всех зарегистрированных пользователей         │\n";
        cout << "│                                                            │\n";
        cout << "│  3. Добавить/изменить оценку                               │\n";
        cout << "│     → Выставление или изменение оценки по предмету         │\n";
        cout << "│                                                            │\n";
        cout << "│  4. Отметить посещаемость                                  │\n";
        cout << "│     → Отметка присутствия/отсутствия на занятии            │\n";
        cout << "│                                                            │\n";
        cout << "│  5. Рейтинг студентов                                      │\n";
        cout << "│     → Список студентов, отсортированный по среднему баллу  │\n";
        cout << "│                                                            │\n";
        cout << "│  6. Общая посещаемость                                     │\n";
        cout << "│     → Статистика посещаемости всех студентов               │\n";
        cout << "│                                                            │\n";
        cout << "│  7. Выйти                                                  │\n";
        cout << "│     → Возврат к экрану входа                               │\n";
        cout << "└────────────────────────────────────────────────────────────┘\n\n";
        int choice = InputValidator::getInt("Выберите действие (1-7): ", 1, 7);

        if (choice == 7) return;

        switch (choice) {
            case 1: {
                string login = InputValidator::getLogin("Логин: ");
                string pass = InputValidator::getPassword("Пароль: ");
                string name = InputValidator::getFullName("ФИО: ");
                string group = InputValidator::getGroup("Группа: ");
                int isAdmin = InputValidator::getInt("Администратор? (1=да, 0=нет): ", 0, 1);
                auth.registerUser(login, pass, name, group, isAdmin);
                usleep(1500000); // 1.5 секунды
                break;
            }
            case 2:
                ReportService::printStudentTable(nonAdminStudents(auth.getAllStudents()));
                usleep(2000000); // 2 секунды
                break;
            case 3: {
                cout << "\nТекущие студенты:\n";
                auto studentsNoAdmins = nonAdminStudents(auth.getAllStudents());
                ReportService::printStudentTable(studentsNoAdmins);
                string login = InputValidator::getLogin("Логин студента: ");
                Student* st = nullptr;
                string lowerLogin = InputValidator::toLower(login);
                for (auto& s : auth.getStudentsRef()) {
                    if (s.isAdmin) continue;
                    string lowerStudentLogin = InputValidator::toLower(s.login);
                    if (lowerStudentLogin == lowerLogin) {
                        st = &s;
                        break;
                    }
                }
                if (!st) { 
                    cout << "Студент не найден!\n"; 
                    usleep(1500000);
                    break; 
                }
                string subject = InputValidator::getSubject("Предмет: ");
                int score = InputValidator::getInt("Оценка (0-10): ", 0, 10);
                StudentService::addGrade(*st, subject, score);
                StudentService::saveGradesAndAttendance(auth.getAllStudents());
                usleep(1500000);
                break;
            }
            case 4: {
                string login = InputValidator::getLogin("Логин студента: ");
                Student* st = nullptr;
                string lowerLogin = InputValidator::toLower(login);
                for (auto& s : auth.getStudentsRef()) {
                    if (s.isAdmin) continue;
                    string lowerStudentLogin = InputValidator::toLower(s.login);
                    if (lowerStudentLogin == lowerLogin) {
                        st = &s;
                        break;
                    }
                }
                if (!st) { 
                    cout << "Студент не найден!\n"; 
                    usleep(1500000);
                    break; 
                }
                string date = InputValidator::getDate("Дата (YYYY-MM-DD): ");
                int present = InputValidator::getInt("Присутствовал? (1=да, 0=нет): ", 0, 1);
                StudentService::addAttendance(*st, date, present);
                StudentService::saveGradesAndAttendance(auth.getAllStudents());
                usleep(1500000);
                break;
            }
            case 5:
                ReportService::printRatingTable(nonAdminStudents(auth.getAllStudents()));
                usleep(2000000);
                break;
            case 6:
                ReportService::printAttendanceTable(nonAdminStudents(auth.getAllStudents()));
                usleep(2000000);
                break;
        }
    }
}

void Menu::studentMenu() {
    while (true) {
        system("clear");
        // Обновляем данные студента из базы
        for (const auto& s : auth.getAllStudents()) {
            if (s.id == currentUser.id) {
                currentUser = s;
                break;
            }
        }

        cout << "СТУДЕНТ: " << currentUser.fullName << "\n\n";
        cout << "1. Просмотреть мои данные\n";
        cout << "2. Оценки по предмету (фильтр)\n";
        cout << "3. Прогноз итоговой оценки\n";
        cout << "4. История изменений оценок\n";
        cout << "5. Выйти\n\n";

        int choice = InputValidator::getInt("Выберите действие (1-5): ", 1, 5);
        if (choice == 5) return;

        system("clear");
        cout << "СТУДЕНТ: " << currentUser.fullName << "\n\n";

        switch (choice) {
            case 1: {
                ReportService::printOneStudentDetailed(currentUser);
                break;
            }
            case 2: {
                string filter = InputValidator::getString("Введите предмет или его часть (пусто = все): ", true);
                string filterLower = InputValidator::toLower(filter);

                cout << "\nОЦЕНКИ (фильтр: " << (filter.empty() ? "все" : filter) << ")\n";
                cout << "┌──────────────────────┬────────┐\n";
                cout << "│ Предмет              │ Оценка │\n";
                cout << "├──────────────────────┼────────┤\n";
                bool hasAny = false;
                for (const auto& g : currentUser.grades) {
                    string subjectLower = InputValidator::toLower(g.subject);
                    if (!filterLower.empty() && subjectLower.find(filterLower) == string::npos) continue;
                    hasAny = true;
                    cout << "│ " << setw(20) << left << truncateUtf8(g.subject, 20)
                         << " │ " << setw(6) << right << g.score << " │\n";
                }
                if (!hasAny) {
                    cout << "│ " << setw(20) << left << "Нет оценок по фильтру"
                         << " │ " << setw(6) << right << "" << " │\n";
                }
                cout << "└──────────────────────┴────────┘\n";
                break;
            }
            case 3: {
                double avg = currentUser.getAverageScore();
                cout << "Текущий средний балл: " << fixed << setprecision(2) << avg << "\n";
                cout << "Прогноз итоговой оценки: " << fixed << setprecision(2) << avg << "\n";
                cout << "(Прогноз основан на текущем среднем балле)\n";
                break;
            }
            case 4: {
                auto history = StudentService::loadGradeHistory(currentUser.id);
                cout << "История изменений оценок:\n";
                cout << "┌──────────────────────┬──────────────────────┬────────┐\n";
                cout << "│ Время                │ Предмет              │ Оценка │\n";
                cout << "├──────────────────────┼──────────────────────┼────────┤\n";
                if (history.empty()) {
                    cout << "│ Нет записей истории                                  │\n";
                } else {
                    for (const auto& h : history) {
                        cout << "│ " << setw(20) << left << h.timestamp
                             << " │ " << setw(20) << left << truncateUtf8(h.subject, 20)
                             << " │ " << setw(6) << right << h.score << " │\n";
                    }
                }
                cout << "└──────────────────────┴──────────────────────┴────────┘\n";
                break;
            }
        }

        waitForEnter();
    }
}
