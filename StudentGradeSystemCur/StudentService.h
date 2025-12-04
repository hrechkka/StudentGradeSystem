#pragma once
#include <string>
#include <vector>
#include "Student.h"

struct GradeHistoryEntry {
    size_t studentId = 0;
    std::string timestamp;
    std::string subject;
    int score = 0;
};

class StudentService {
public:
    static void addGrade(Student& student, const std::string& subject, int score);
    static void addAttendance(Student& student, const std::string& date, bool present);
    static void saveGradesAndAttendance(const std::vector<Student>& students);
    static void loadGradesAndAttendance(std::vector<Student>& students);
    static void logGradeHistory(size_t studentId, const std::string& subject, int score);
    static std::vector<GradeHistoryEntry> loadGradeHistory(size_t studentId);
};
