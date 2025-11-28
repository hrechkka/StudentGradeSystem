#pragma once
#include "AuthService.h"
#include "Student.h"

class Menu {
public:
    Menu();
    void showLoginScreen();

private:
    AuthService auth;
    Student currentUser;

    void adminMenu();
    void studentMenu();
};
