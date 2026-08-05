#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "ship.h"
#include "custom_ship.h"
#include "player_manager.h"

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define GRAY "\033[90m"
#define BLACK "\033[30m"

#define SHIP_ROW_LENGTH (5)
#define EXIT_CODE (-1)

std::vector<Ship> classicShips = { Ship(2, 1, 0, 0), Ship(3, 1, 0, 0), Ship(3, 1, 0, 0), Ship(4, 1, 0, 0), Ship(5, 1, 0, 0) };

void trimAndToLower(std::string& s);

int setup(bool, std::vector<Ship>&);
int shipPlacement(int, int, bool, std::vector<Ship>& const, PlayerManager&, PlayerManager&);
void printShip(Ship ship);
void printBoard(std::vector<std::vector<char>>& const);
void printShips(std::vector<Ship>& const);
void printShips(std::vector<Ship>& const, std::vector<bool>& const);
int getIntegerInput(std::string, int, int, bool, std::string);
int getIntegerInput(int, int);
std::pair<int, int> getTwoIntegersInput(std::string, int, int, int, int, bool, std::string);

int main()
{
    std::cout << "Welcome to Battleship!\n";
    int numPlayers = 1;
    int gridSize = 10;
    bool classic = true;
    std::vector<Ship> baseShips = std::vector<Ship>();
    PlayerManager player1, player2;
    std::string mainPrompt = "\n===== Main Menu =====\n[1] Classic Single-player\n[2] Classic Two-player\n[3] Custom Single-player\n[4] Custom Two-player\n[5] Create Custom Ship\n[6] Exit";
    while (true) {
        int mainInput = getIntegerInput(mainPrompt, 1, 5);
        switch (mainInput) {
        case 1:
        case 2:
        case 3:
        case 4:
            numPlayers = (mainInput % 2 == 0) ? 2 : 1;
            classic = mainInput <= 2;
            gridSize = setup(classic, baseShips);
			if (gridSize == EXIT_CODE) {
                continue;
			}
            break;
        case 5:
            // create custom ships
            break;
        case 6:
            return 0;
        }
    }   
}

void trimAndToLower(std::string& s) {
    const std::string whitespace = " \t\n\r\f\v";

    size_t first = s.find_first_not_of(whitespace);
    if (first == std::string::npos) {
        s.clear();
        return;
    }

    size_t last = s.find_last_not_of(whitespace);
    s = s.substr(first, last - first + 1);

    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); });
}

int setup(bool classic, std::vector<Ship>& baseShips) {
    int gridSize = 10;
    if (classic) {
        baseShips = classicShips;
    }
    else {
		gridSize = getIntegerInput("Enter grid size (5-20) or 'exit' to return to Main Menu: ", 5, 20, true);
        if (gridSize == EXIT_CODE) {
            return EXIT_CODE;
        }
        // input ships
    }
	for (int i = 0; i < baseShips.size(); i++) {
		baseShips[i].setShipNum(i);
	}
    return gridSize;
}

int shipPlacement(int players, int gridSize, bool classic,std::vector<Ship>& const baseShips, PlayerManager& player1, PlayerManager& player2) {
    int totalShips = baseShips.size();
    for (int i = 1; i <= players; i++) {
        PlayerManager player;
		if (i == 1) {
			player = player1;
		}
		else {
			player = player2;
		}
        std::vector<Ship> ships = baseShips;
		std::vector<bool> placed(totalShips, false);
        std::vector<std::vector<char>> placementBoard = std::vector<std::vector<char>>(
            gridSize, std::vector<char>(gridSize, '.')
        );
        int shipsPlaced = 0;
        while (true) {
            std::cout << "\n===== Player " + std::to_string(i) + " Setup =====\n";
            std::cout << "Board:\n";
            printBoard(placementBoard);
            std::cout << "\nShips:\n";
            printShips(ships, placed);
            int placementInput = 0;
            int transformInput = 0;
            std::pair<int, int> shipPos;
            if (shipsPlaced == totalShips) {
                placementInput = getIntegerInput("Enter the ship number to add/remove (1-" + std::to_string(totalShips) + "), 0 to confirm your setup and continue, or 'exit' to return to Main Menu: ", 0, totalShips, true);
            }
            else {
                placementInput = getIntegerInput("Enter the ship number to add/remove (1-" + std::to_string(totalShips) + ") or 'exit' to return to Main Menu: ", 1, totalShips, true);
            }
            if (placementInput == EXIT_CODE) {
                return EXIT_CODE;
            }
            else if (placementInput == 0) {
                break;
            }
            else if (!placed[placementInput - 1]) {
                // Transform ship
                ships[placementInput - 1] = baseShips[placementInput - 1];
                while (true) {
                    std::cout << "\nShip " + std::to_string(placementInput) << "\n";
					printShip(ships[placementInput - 1]);
                    transformInput = getIntegerInput("Transform the ship:\n[1] Rotate 90° Clockwise\n[2] Rotate 90° Counter-Clockwise\n[3] Flip Horizontally\n[4] Flip Vertically\n[5] Confirm Transformation\n[6] Return to Ship Selection\nEnter your choice: ", 1, 6, true);
                    if (transformInput >= 5) {
                        break;
                    }
                    switch (transformInput) {
                    case 1:
                        ships[placementInput - 1].rotateClockwise();
                        break;
                    case 2:
                        ships[placementInput - 1].rotateCounterClockwise();
                        break;
                    case 3:
                        ships[placementInput - 1].flipHorizontal();
                        break;
                    case 4:
                        ships[placementInput - 1].flipVertical();
                        break;
                    }
                }
                if (transformInput == 6) {
                    continue;
                }
                // Place ship
                while (true) {
                    std::cout << "Board:\n";
                    printBoard(placementBoard);
                    std::cout << "\n";
                    shipPos = getTwoIntegersInput("Enter the position to place the ship (row col) or 'cancel' to return to ship selection: ", 0, gridSize - 1, 0, gridSize - 1, true, "cancel");
					if (shipPos.first == EXIT_CODE) {
						break;
					}
                    if (ships[placementInput - 1].addToBoard(placementBoard)) {
                        placed[placementInput - 1] = true;
						std::cout << "Ship " + std::to_string(placementInput) + " placed successfully!\n";
                        break;
                    }
                    else {
						std::cout << "Invalid placement. Please try again.\n";
                    }
                }
				if (shipPos.first == EXIT_CODE) {
					continue;
				}
            }
            else {
                ships[placementInput - 1].removeFromBoard(placementBoard);
                placed[placementInput - 1] = false;
            }
        }
        player.setPlacementBoard(placementBoard);
        player.setShips(ships);
    }

    if (players == 1) {
        // Bot is player2
        std::vector<Ship> ships = baseShips;
        std::vector<std::vector<char>> placementBoard = std::vector<std::vector<char>>(
            gridSize, std::vector<char>(gridSize, '.')
        );
        for (Ship ship : ships) {
            
        }
		player2.setPlacementBoard(placementBoard);
		player2.setShips(ships);
        // bot setup
    }
    return 0;
}

void printShip(Ship ship) {
    for (int i = 0; i < ship.getHeight(); i++) {
        for (int j = 0; j < ship.getWidth(); j++) {
            if (ship.getShipGrid()[i][j]) {
                std::cout << "o";
            }
            else {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
}

void printBoard(std::vector<std::vector<char>>& const board) {
    int n = board.size();
	std::cout << "  ";
    	for (int i = 0; i < n; i++) {
		std::cout << i << " ";
	}
	std::cout << "\n";
	for (int i = 0; i < n; i++) {
		std::cout << i << " ";
		for (int j = 0; j < n; j++) {
			std::cout << board[i][j] << " ";
		}
		std::cout << "\n";
	}
}

void printShips(std::vector<Ship>& const ships) {
	
}

void printShips(std::vector<Ship>& const ships, std::vector<bool>& const placed) {
    int n = ships.size();
    int i = 0;
    while (i < n) {
        int maxHeight = 0;
        // print first row and get max height
        for (int j = i; j < i + SHIP_ROW_LENGTH && j < n; j++) {
            if (placed[j]) {
                std::cout << RED;
            }
            else {
                std::cout << GREEN;
            }
            std::cout << "[" + std::to_string(j + 1) + "] ";
            for (int k = 0; k < ships[j].getWidth(); k++) {
                if (ships[j].getShipGrid()[0][k]) {
                    std::cout << "o";
                }
                else {
                    std::cout << " ";
                }
				maxHeight = std::max(maxHeight, ships[j].getHeight());
            }
            std::cout << "  ";
            std::cout << RESET;
        }
        std::cout << "\n";
        for (int j = 1; j < maxHeight; j++) {
            for (int k = i; k < i + SHIP_ROW_LENGTH && k < n; k++) {
                if (placed[k]) {
                    std::cout << RED;
                }
                else {
                    std::cout << GREEN;
                }
				std::cout << "    ";
                if (j >= ships[k].getHeight()) {
					for (int l = 0; l < ships[k].getWidth(); l++) {
						std::cout << " ";
					}
                }
                else {
                    for (int l = 0; l < ships[k].getWidth(); l++) {
                        if (ships[k].getShipGrid()[j][l]) {
                            std::cout << "o";
                        }
                        else {
                            std::cout << " ";
                        }
                    }
                }
                std::cout << "  ";
                std::cout << RESET;
                
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
}

int getIntegerInput(std::string prompt, int min, int max, bool allowExit = false, std::string exitString = "exit") {
    int input;
    while (true) {
        std::cout << prompt;
        std::string line;

        std::getline(std::cin, line);
        trimAndToLower(line);
		if (allowExit && line == exitString) {
			return EXIT_CODE;
		}
        try {
            std::size_t pos;
            input = std::stoi(line, &pos);
            if (pos != line.size()) {
                std::cout << "Invalid input. Please enter an integer from " << min << " to " << max << (allowExit ? " or " + exitString : "") + ".\n";
                continue;
            }
        }
        catch (...) {
            std::cout << "Invalid input. Please enter an integer from " << min << " to " << max << (allowExit ? " or " + exitString : "") + ".\n";
            continue;
        }
		if (input < min || input > max) {
			std::cout << "Invalid input. Please enter an integer from " << min << " to " << max << (allowExit ? " or " + exitString : "") + ".\n";
			continue;
		}
        return input;
    }
}

int getIntegerInput(int min, int max) {
    int input;
    std::string line;
    std::getline(std::cin, line);
    trimAndToLower(line);
    try {
        std::size_t pos;
        input = std::stoi(line, &pos);
        if (pos != line.size()) {
            std::cout << "Invalid input. Please enter an integer from " << min << " to " << max << ".\n";
            return -1;
        }
    }
    catch (...) {
        std::cout << "Invalid input. Please enter an integer from " << min << " to " << max << ".\n";
        return -1;
    }
    if (input < min || input > max) {
        std::cout << "Invalid input. Please enter an integer from " << min << " to " << max << ".\n";
        return -1;
    }
    return input;
}

std::pair<int, int> getTwoIntegersInput(std::string prompt, int min1, int max1, int min2, int max2, bool allowExit = false, std::string exitString = "exit") {
    std::pair<int, int> input;
    while (true) {
        std::cout << prompt;
        std::string line;

        std::getline(std::cin, line);
        trimAndToLower(line);
		if (allowExit && line == exitString) {
			return { EXIT_CODE, EXIT_CODE };
		}
        try {
            std::size_t pos;
            input.first = std::stoi(line, &pos);
            std::string second = line.substr(pos);
            input.second = std::stoi(second, &pos);
			if (pos != second.size()) {
				std::cout << "Invalid input. Please enter an integer from " << min1 << " to " << max1 << " and an integer from " << min2 << " to " << max2 << ", separated by a space" + (allowExit ? ", or " + exitString : "") + ".\n";
				continue;
			}
        }
        catch (...) {
            std::cout << "Invalid input. Please enter an integer from " << min1 << " to " << max1 << " and an integer from " << min2 << " to " << max2 << ", separated by a space" + (allowExit ? ", or " + exitString : "") + ".\n";
            continue;
        }
        if (input.first < min1 || input.first > max1 || input.second < min2 || input.second > max2) {
            std::cout << "Invalid input. Please enter an integer from " << min1 << " to " << max1 << " and an integer from " << min2 << " to " << max2 << ", separated by a space" + (allowExit ? ", or " + exitString : "") + ".\n";
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
