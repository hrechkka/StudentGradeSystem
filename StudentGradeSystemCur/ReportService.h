#pragma once
#include <string>
#include <vector>
#include "Student.h"

class ReportService {
public:
    static void printStudentTable(const std::vector<Student>& students);
    static void printOneStudentDetailed(const Student& student);
    static void printRatingTable(const std::vector<Student>& students);
    static void printAttendanceTable(const std::vector<Student>& students);
};
