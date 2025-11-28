#pragma once
#include <string>
#include <vector>

struct Grade {
    std::string subject;
    int score = 0;
};

struct Attendance {
    std::string date;
    bool present = false;
};

struct Student {
    size_t id = 0;
    std::string login;
    std::string fullName;
    std::string group;
    std::string passwordHash;
    bool isAdmin = false;

    std::vector<Grade> grades;
    std::vector<Attendance> attendance;

    double getAverageScore() const {
        if (grades.empty()) return 0.0;
        double sum = 0.0;
        for (const auto& g : grades) sum += g.score;
        return sum / grades.size();
    }
};
