#pragma once
#include <string>
#include <vector>
#include "Student.h"

class StudentService {
public:
    static void addGrade(Student& student, const std::string& subject, int score);
    static void addAttendance(Student& student, const std::string& date, bool present);
    static void saveGradesAndAttendance(const std::vector<Student>& students);
    static void loadGradesAndAttendance(std::vector<Student>& students);
};
