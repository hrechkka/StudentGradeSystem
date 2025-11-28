#include <iostream>
#include <filesystem>
#include "Menu.h"

using namespace std;

int main() {
    if (!filesystem::exists("data")) {
        filesystem::create_directory("data");
        cout << "Создана папка data/ для хранения данных\n";
    }

    Menu menu;
    menu.showLoginScreen();
    return 0;
}
