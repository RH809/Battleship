#include <iostream>
#include <string>
#include <vector>

#include "ship.h"
#include "custom_ship.h"

std::vector<Ship> classicShips = { Ship(2, 1, 0, 0), Ship(3, 1, 0, 0), Ship(3, 1, 0, 0), Ship(4, 1, 0, 0), Ship(5, 1, 0, 0) };

int main()
{
    std::cout << "Welcome to Battleship!\n";
    std::string mainPrompt = "===== Main Menu =====\n[1] Classic Single-player\n[2] Classic Two-player\n[3] Custom Single-player\n[4] Custom Two-player\n[5] Exit";
    while (true) {
        switch (getIntegerInput(mainPrompt, 1, 5)) {
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            return 0;
        }
    }   
}

void setup(int players, bool classic) {
    std::vector<Ship> baseShips;
    if (classic) {
        baseShips = classicShips;
    }
    else {
        // input ships
    }
    if (players == 1) {
        // bot setup
    }
    for (int i = 1; i <= players; i++) {
        
    }
}

int getIntegerInput(std::string prompt, int min, int max) {
    int input;
    while (true) {
        std::cout << prompt;
        std::string line;

        std::getline(std::cin, line);
        try {
            std::size_t pos;
            input = std::stoi(line, &pos);
            if (pos != line.size()) {
                std::cout << "Invalid input. Please enter an integer from " << min << " to " << max << ".\n";
                continue;
            }
        }
        catch (...) {
            std::cout << "Invalid input. Please enter an integer from " << min << " to " << max << ".\n";
            continue;
        }
		if (input < min || input > max) {
			std::cout << "Invalid input. Please enter an integer from " << min << " to " << max << ".\n";
			continue;
		}
        return input;
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
