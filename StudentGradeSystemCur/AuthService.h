#pragma once
#include <string>
#include <vector>
#include "Student.h"

class AuthService {
public:
    AuthService();
    bool login(const std::string& login, const std::string& password, Student& currentUser);
    bool registerUser(const std::string& login, const std::string& password,
                      const std::string& fullName, const std::string& group, bool isAdmin);
    
    std::vector<Student> getAllStudents() const;
    std::vector<Student>& getStudentsRef();
    void saveAllStudents();
    void loadGradesAndAttendance();
    static std::string getDataPath(); // Получить базовый путь к папке data

private:
    std::vector<Student> students;
    size_t nextId = 1;

    void loadStudents();
    Student* findUser(const std::string& login);
};
