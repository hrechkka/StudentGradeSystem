#include "Menu.h"
#include "InputValidator.h"
#include "StudentService.h"
#include "ReportService.h"
#include <iostream>
#include <cstdlib>

using namespace std;

Menu::Menu() {
    // Загружаем оценки и посещаемость при старте
    auto all = auth.getAllStudents();
    StudentService::loadGradesAndAttendance(all);
    for (auto& s : all) {
        for (auto& existing : auth.getAllStudents()) {
            if (existing.id == s.id) {
                // Копируем оценки и посещаемость обратно
                auto it = find_if(auth.getAllStudents().begin(), auth.getAllStudents().end(),
                    [&s](const Student& st) { return st.id == s.id; });
                if (it != auth.getAllStudents().end()) {
                    it->grades = s.grades;
                    it->attendance = s.attendance;
                }
            }
        }
    }
}

void Menu::showLoginScreen() {
    while (true) {
        system("clear"); // cls для Windows
        cout << "=== СИСТЕМА УЧЁТА УСПЕВАЕМОСТИ ===\n\n";
        string login = InputValidator::getLogin("Введите логин: ");
        string pass = InputValidator::getPassword("Введите пароль: ");

        if (auth.login(login, pass, currentUser)) {
            cout << "Добро пожаловать, " << currentUser.fullName << "!\n";
            cin.get(); // пауза
            if (currentUser.isAdmin) adminMenu();
            else studentMenu();
        } else {
            cout << "Неверный логин или пароль!\n";
            cin.get();
        }
    }
}

void Menu::adminMenu() {
    while (true) {
        system("clear");
        cout << "АДМИНИСТРАТОР: " << currentUser.fullName << "\n\n";
        cout << "1. Добавить пользователя\n";
        cout << "2. Просмотреть всех студентов\n";
        cout << "3. Добавить/изменить оценку\n";
        cout << "4. Отметить посещаемость\n";
        cout << "5. Рейтинг студентов\n";
        cout << "6. Общая посещаемость\n";
        cout << "7. Выйти\n\n";
        int choice = InputValidator::getInt("Выбор: ", 1, 7);

        if (choice == 7) return;

        switch (choice) {
            case 1: {
                string login = InputValidator::getLogin("Логин: ");
                string pass = InputValidator::getPassword("Пароль: ");
                string name = InputValidator::getString("ФИО: ");
                string group = InputValidator::getString("Группа: ");
                int isAdmin = InputValidator::getInt("Администратор? (1=да, 0=нет): ", 0, 1);
                auth.registerUser(login, pass, name, group, isAdmin);
                cin.get();
                break;
            }
            case 2:
                ReportService::printStudentTable(auth.getAllStudents());
                cin.get(); cin.get();
                break;
            case 3: {
                string login = InputValidator::getLogin("Логин студента: ");
                Student* st = nullptr;
                for (auto& s : auth.getAllStudents()) if (s.login == login) st = &s;
                if (!st) { cout << "Студент не найден!\n"; cin.get(); break; }
                string subject = InputValidator::getString("Предмет: ");
                int score = InputValidator::getInt("Оценка (0-10): ", 0, 10);
                StudentService::addGrade(*st, subject, score);
                StudentService::saveGradesAndAttendance(auth.getAllStudents());
                cin.get();
                break;
            }
            case 4: {
                string login = InputValidator::getLogin("Логин студента: ");
                Student* st = nullptr;
                for (auto& s : auth.getAllStudents()) if (s.login == login) st = &s;
                if (!st) { cout << "Студент не найден!\n"; cin.get(); break; }
                string date = InputValidator::getDate("Дата (YYYY-MM-DD): ");
                int present = InputValidator::getInt("Присутствовал? (1=да, 0=нет): ", 0, 1);
                StudentService::addAttendance(*st, date, present);
                StudentService::saveGradesAndAttendance(auth.getAllStudents());
                cin.get();
                break;
            }
            case 5:
                ReportService::printRatingTable(auth.getAllStudents());
                cin.get(); cin.get();
                break;
            case 6:
                ReportService::printAttendanceTable(auth.getAllStudents());
                cin.get(); cin.get();
                break;
        }
    }
}

void Menu::studentMenu() {
    while (true) {
        system("clear");
        cout << "СТУДЕНТ: " << currentUser.fullName << "\n\n";
        ReportService::printOneStudentDetailed(currentUser);
        cout << "\nНажмите Enter для выхода...";
        cin.get();
        return;
    }
}
