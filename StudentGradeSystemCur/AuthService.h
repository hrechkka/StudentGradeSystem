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
    void saveAllStudents();

private:
    std::vector<Student> students;
    size_t nextId = 1;

    void loadStudents();
    Student* findUser(const std::string& login);
};
