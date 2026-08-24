#include <algorithm>
#include <deque>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <string>
#include <vector>

#include "ship.h"
#include "custom_ship.h"
#include "player_manager.h"

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"

#define SHIP_ROW_LENGTH (5)
#define EXIT_CODE (-1)

#define DEFAULT_GRID_SIZE (10)
#define MIN_GRID_SIZE (3)
#define MAX_GRID_SIZE (20)
#define MIN_SHIP_SIZE (1)
#define MAX_SHIP_SIZE (10)

int randomInt(int, int);
void trimAndToLower(std::string& s);
void clearOutput(void);
int setup(bool, std::vector<std::unique_ptr<Ship>>&);
void shipCreation(void);
void printShipCreation(const std::vector<std::vector<char>>&);
bool isValidShip(std::vector<std::vector<char>>);
void traverse(std::vector<std::vector<char>>&, int, int);
CustomShip buildShip(const std::vector<std::vector<char>>);
bool importShip(std::string path, CustomShip&, int);
int shipPlacement(int, int, bool, const std::vector<std::unique_ptr<Ship>>&, PlayerManager&, PlayerManager&);
bool placeShips(std::vector<std::vector<char>>&, int, std::vector<std::unique_ptr<Ship>>&, int);
int playSingleplayer(PlayerManager&, PlayerManager&, const std::vector<std::unique_ptr<Ship>>&, int);
int playMultiplayer(PlayerManager&, PlayerManager&, const std::vector<std::unique_ptr<Ship>>&, int);
void printEndBoards(int, bool, PlayerManager&, PlayerManager&, const std::vector<std::unique_ptr<Ship>>&);
void printEndBoard(const std::vector<std::vector<char>>&, const std::vector<std::vector<char>>&, int);
void printShip(const std::unique_ptr<Ship>& ship);
void printGameBoard(const std::vector<std::vector<char>>&);
void printPlacementBoard(const std::vector<std::vector<char>>&, int);
void printShips(const std::vector<std::unique_ptr<Ship>>&);
void printShips(const std::vector<std::unique_ptr<Ship>>&, const std::vector<bool>&);
int getIntegerInput(std::string, int, int, bool allowExit = false, std::string exitString = "exit");
std::pair<int, int> getTwoIntegersInput(std::string, int, int, int, int, bool allowExit = false, std::string exitString = "exit");


std::mt19937 gen(std::random_device{}());
std::vector<std::unique_ptr<Ship>> classicShips;
std::string const shipPath = "Ships/";

int main()
{
    // Classic ships setup
    classicShips.push_back(std::make_unique<Ship>(2, 1, 0, 0));
    classicShips.push_back(std::make_unique<Ship>(3, 1, 0, 0));
    classicShips.push_back(std::make_unique<Ship>(3, 1, 0, 0));
    classicShips.push_back(std::make_unique<Ship>(4, 1, 0, 0));
    classicShips.push_back(std::make_unique<Ship>(5, 1, 0, 0));

    clearOutput();
    std::cout << RESET << "Welcome to Battleship!\n";
    int numPlayers = 1;
    int gridSize = DEFAULT_GRID_SIZE;
    bool classic = true;
    std::vector<std::unique_ptr<Ship>> baseShips = std::vector<std::unique_ptr<Ship>>();
    PlayerManager player1, player2;
    std::string mainPrompt = "===== Main Menu =====\n[1] Classic Single-player\n[2] Classic Two-player\n[3] Custom Single-player\n[4] Custom Two-player\n[5] Create Custom Ship\n[6] Exit\nEnter your choice: ";
    while (true) {
        int mainInput = getIntegerInput(mainPrompt, 1, 6);
        switch (mainInput) {
        case 1:
        case 2:
        case 3:
        case 4:
            numPlayers = (mainInput % 2 == 0) ? 2 : 1;
            classic = mainInput <= 2;
            clearOutput();
            if (!classic) {
                std::cout << "==== Custom" << (numPlayers == 1 ? " Single-player" : " Two-player") << " Setup Stage ==== \n";
            }
            gridSize = setup(classic, baseShips);
            clearOutput();
			if (gridSize == EXIT_CODE) {
                continue;
			}
            std::cout << "==== " << (classic ? "Classic " : "Custom ") << (numPlayers == 1 ? " Single-player" : " Two-player") << " Ship Placement Stage ==== \n";
            if (shipPlacement(numPlayers, gridSize, classic, baseShips, player1, player2) == EXIT_CODE) {
                clearOutput();
                continue;
            }
            std::cout << "==== " << (classic ? "Classic " : "Custom ") << (numPlayers == 1 ? " Single-player" : " Two-player") << " ==== \n";
            if (numPlayers == 1) {
                playSingleplayer(player1, player2, baseShips, gridSize);
            }
            else {
                playMultiplayer(player1, player2, baseShips, gridSize);
            }
            break;
        case 5:
            // create custom ships
            clearOutput();
            std::cout << "===== Custom Ship Creation ===== \n";
            shipCreation();
            break;
        case 6:
            clearOutput();
            return 0;
        }
    }   
}

int randomInt(int min, int max) {
    std::uniform_int_distribution<> dist(min, max);
    return dist(gen);
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

void clearOutput(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int setup(bool classic, std::vector<std::unique_ptr<Ship>>& baseShips) {
    baseShips = std::vector<std::unique_ptr<Ship>>();
    int gridSize = DEFAULT_GRID_SIZE;
    if (classic) {
        for (const auto& ship : classicShips)
        {
            baseShips.push_back(ship->clone());
        }
    }
    else {
		gridSize = getIntegerInput("Enter grid size (" + std::to_string(MIN_GRID_SIZE) + "-" + std::to_string(MAX_GRID_SIZE) + ") or 'exit' to return to Main Menu: ", MIN_GRID_SIZE, MAX_GRID_SIZE, true);
        if (gridSize == EXIT_CODE) {
            return EXIT_CODE;
        }
        // input ships
        while (true) {
            int setupInput = getIntegerInput("\n===== Setup Menu =====\n[1] View/Remove Ships\n[2] Add Ship\n[3] Confirm Ship List\n[4] Return to Main Menu\nEnter your choice: ", 1, 4);
            if (setupInput == 4) {
                return EXIT_CODE;
            }
            else if (setupInput == 3) {
                if (baseShips.empty()) {
                    std::cout << RED << "Ship list is empty.\n" << RESET;
                }
                else {
                    break;
                }
            }
            else if (setupInput == 2) {
                while (true) {
                    std::cout << "Enter the name of the ship to add or 'cancel' to return to option selection: ";
                    std::string name;
                    std::getline(std::cin, name);
                    trimAndToLower(name);
                    if (name == "cancel") {
                        break;
                    }
                    else if (!std::filesystem::exists(shipPath + "/" + name + ".txt")) {
                        std::cout << RED << "Could not find ship '" << name << "'.\n" << RESET;
                    }
                    else {
                        CustomShip ship = CustomShip(0, 0, 0, 0, {});
                        if (!importShip(shipPath + "/" + name + ".txt", ship, gridSize)) {
                            std::cout << RED << "There was an error adding ship '" << name << "'.\n" << RESET;
                        }
                        else {
                            baseShips.push_back(std::make_unique<CustomShip>(ship));
                            std::cout << GREEN << "Added ship '" << name << "' successfully!\n" << RESET;
                            break;
                        }
                    }
                }
            }
            else if (setupInput == 1) {
                if (baseShips.empty()) {
                    std::cout << YELLOW << "No ships added.\n" << RESET;
                }
                else {
                    std::cout << "\nShips:\n";
                    printShips(baseShips);
                    int removeInput = getIntegerInput("Enter the ship number to remove (1-" + std::to_string(baseShips.size()) + ") or 'cancel' to return to option selection: ", 1, baseShips.size(), true, "cancel");
                    if (removeInput == EXIT_CODE) {
                        continue;
                    }
                    baseShips.erase(baseShips.begin() + removeInput - 1);
                    std::cout << GREEN << "Ship " << removeInput << " removed successfully!\n" << RESET;
                }
            }
        }
        std::cout << GREEN << "\nShip list confirmed!\n" << RESET;
        printShips(baseShips);
        std::cout << "Press Enter to return to continue...";
        std::cin.get();
    }
	for (int i = 0; i < baseShips.size(); i++) {
		baseShips[i]->setShipNum(i + 1);
	}
    return gridSize;
}

void shipCreation() {
    std::pair<int, int> dimensions = getTwoIntegersInput("Enter the width and height (" + std::to_string(MIN_SHIP_SIZE) + "-" + std::to_string(MAX_SHIP_SIZE) + ") of the ship (width height) or 'exit' to return to Main Menu: ",
        MIN_SHIP_SIZE, MAX_SHIP_SIZE, MIN_SHIP_SIZE, MAX_SHIP_SIZE, true);
    if (dimensions.first == EXIT_CODE) {
        clearOutput();
        return;
    }
    std::vector<std::vector<char>> shipGrid = std::vector<std::vector<char>>(
        dimensions.second, std::vector<char>(dimensions.first, '.')
    );
    std::string name;
    bool save = false;
    while (!save) {
        int creationInput = getIntegerInput("\n===== Custom Ship Creation Menu =====\n[1] Edit/View Ship\n[2] Save Ship\n[3] Return to Main Menu\nEnter your choice: ", 1, 3);
        if (creationInput == 3) {
            clearOutput();
            return;
        }
        else if (creationInput == 2) {
            if (!isValidShip(shipGrid)) {
                std::cout << RED << "Invalid ship. Cannot save it.\n" << RESET;
                continue;
            }
            else {
                std::cout << "Enter the name of the ship or 'cancel' to return to option selection: ";
                std::getline(std::cin, name);
                trimAndToLower(name);
                if (name != "cancel") {
                    save = true;
                }
            }
        }
        else {
            std::cout << "\nShip:\n";
            printShipCreation(shipGrid);
            std::cout << "\n";
            std::pair<int, int> point = getTwoIntegersInput("Enter the point (row col) to toggle on/off or 'cancel' to return to option selection: ", 0, dimensions.second - 1, 0, dimensions.first - 1, true, "cancel");
            if (point.first == EXIT_CODE) {
                continue;
            }
            if (shipGrid[point.first][point.second] == '.') {
                shipGrid[point.first][point.second] = 'o';
            }
            else {
                shipGrid[point.first][point.second] = '.';
            }
            std::cout << "\nShip:\n";
            printShipCreation(shipGrid);
        }
    }
    CustomShip newShip = buildShip(shipGrid);
    std::ofstream file(shipPath + "/" + name + ".txt", std::ios::trunc);
    if (!file.is_open()) {
        std::cout << RED << "There was an error creating the save file.\n" << RESET;
        std::cout << "\nPress Enter to return to Main Menu...";
        std::cin.get();
        clearOutput();
        return;
    }
    file << newShip.getWidth() << " " << newShip.getHeight() << "\n";
    for (std::pair<int, int> ex : newShip.getExclusions()) {
        file << ex.first << " " << ex.second << "\n";
    }
    file.flush();
    file.close();
    std::cout << GREEN << "Ship '" << name << "' saved successfully!\n" << RESET;
    std::cout << "\nPress Enter to return to Main Menu...";
    std::cin.get();
    clearOutput();
}

void printShipCreation(const std::vector<std::vector<char>>& ship) {
    int height = ship.size();
    int width = ship[0].size();
    std::cout << YELLOW << "  ";
    for (int i = 0; i < width; i++) {
        std::cout << i << " ";
    }
    std::cout << "\n" << RESET;
    for (int i = 0; i < height; i++) {
        std::cout << YELLOW << i << " " << RESET;
        for (int j = 0; j < width; j++) {
            if (ship[i][j] == 'o') {
                std::cout << BLUE;
            }
            std::cout << ship[i][j] << " " << RESET;
        }
        std::cout << "\n";
    }
}

bool isValidShip(std::vector<std::vector<char>> ship) {
    int height = ship.size();
    int width = ship[0].size();
    int groups = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (ship[i][j] == 'o') {
                if (groups > 0) {
                    return false;
                }
                traverse(ship, i, j);
                groups++;
            }
        }
    }
    return groups == 1;
}

void traverse(std::vector<std::vector<char>>& ship, int r, int c) {
    int height = ship.size();
    int width = ship[0].size();
    std::queue<std::pair<int, int>> traverseQueue = std::queue<std::pair<int, int>>();
    traverseQueue.push({ r, c });
    ship[r][c] = '.';
    while (!traverseQueue.empty()) {
        std::pair<int, int> point = traverseQueue.front();
        traverseQueue.pop();
        if (point.first > 0 && ship[point.first - 1][point.second] == 'o') {
            traverseQueue.push({ point.first - 1, point.second });
            ship[point.first - 1][point.second] = '.';
        }
        if (point.first < height - 1 && ship[point.first + 1][point.second] == 'o') {
            traverseQueue.push({ point.first + 1, point.second });
            ship[point.first + 1][point.second] = '.';
        }
        if (point.second > 0 && ship[point.first][point.second - 1] == 'o') {
            traverseQueue.push({ point.first, point.second - 1 });
            ship[point.first][point.second - 1] = '.';
        }
        if (point.second < width - 1 && ship[point.first][point.second + 1] == 'o') {
            traverseQueue.push({ point.first, point.second + 1 });
            ship[point.first][point.second + 1] = '.';
        }
    }
}

CustomShip buildShip(const std::vector<std::vector<char>> shipGrid) {
    int height = shipGrid.size();
    int width = shipGrid[0].size();
    int smallestR = MAX_SHIP_SIZE;
    int smallestC = MAX_SHIP_SIZE;
    int largestR = 0;
    int largestC = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (shipGrid[i][j] == 'o') {
                smallestR = std::min(smallestR, i);
                smallestC = std::min(smallestC, j);
                largestR = std::max(largestR, i);
                largestC = std::max(largestC, j);
            }
        }
    }
    std::vector<std::pair<int, int>> exclusions;
    for (int i = smallestR; i <= largestR; i++) {
        for (int j = smallestC; j <= largestC; j++) {
            if (shipGrid[i][j] == '.') {
                exclusions.push_back({ i - smallestR, j - smallestC });
            }
        }
    }

    CustomShip newShip = CustomShip(largestC - smallestC + 1, largestR - smallestR + 1, 0, 0, exclusions);
    return newShip;
}

bool importShip(std::string path, CustomShip& ship, int gridSize) {
    std::fstream file(path);
    std::string line;
    std::getline(file, line);
    int width;
    int height;
    try {
        std::size_t pos;
        width = std::stoi(line, &pos);
        std::string second = line.substr(pos);
        height  = std::stoi(second, &pos);
        if (pos != second.size()) {
            return false;
        }
    }
    catch (...) {
        return false;
    }
    if (width < MIN_SHIP_SIZE || width > MAX_SHIP_SIZE || height < MIN_SHIP_SIZE || height > MAX_SHIP_SIZE || width > gridSize || height > gridSize) {
        return false;
    }
    std::vector<std::pair<int, int>> exclusions = std::vector<std::pair<int, int>>();
    while (std::getline(file, line))
    {
        int r, c;
        try {
            std::size_t pos;
            r = std::stoi(line, &pos);
            std::string second = line.substr(pos);
            c = std::stoi(second, &pos);
            if (pos != second.size()) {
                return false;
            }
        }
        catch (...) {
            return false;
        }
        if (r < 0 || r >= height || c < 0 || c >= width) {
            return false;
        }
        exclusions.push_back({ r, c });
    }
    ship = CustomShip(width, height, 0, 0, exclusions);
    return true;
}

int shipPlacement(int players, int gridSize, bool classic, const std::vector<std::unique_ptr<Ship>>& baseShips, PlayerManager& player1, PlayerManager& player2) {
    int totalShips = baseShips.size();
    for (int i = 1; i <= players; i++) {
        PlayerManager& player = (i == 1) ? player1 : player2;
        std::vector<std::unique_ptr<Ship>> ships = std::vector<std::unique_ptr<Ship>>();
        for (const auto& ship : baseShips)
        {
            ships.push_back(ship->clone());
        }
		std::vector<bool> placed(totalShips, false);
        std::vector<std::vector<char>> placementBoard = std::vector<std::vector<char>>(
            gridSize, std::vector<char>(gridSize, '.')
        );
        int shipsPlaced = 0;
        while (true) {
            std::cout << "\n===== Player " + std::to_string(i) + " Setup =====\n";
            std::cout << "\nBoard:\n";
            printPlacementBoard(placementBoard, totalShips);
            std::cout << "\nShips:\n";
            printShips(baseShips, placed);
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
                ships[placementInput - 1] = baseShips[placementInput - 1]->clone();
                if (classic) {
                    while (true) {
                        std::cout << "\nShip " + std::to_string(placementInput) << "\n";
                        for (int j = 0; j < std::max(4, ships[placementInput - 1]->getWidth() * 2); j++) {
                            std::cout << "-";
                        }
                        std::cout << "\n";
                        printShip(ships[placementInput - 1]);
                        for (int j = 0; j < std::max(4, ships[placementInput - 1]->getWidth() * 2); j++) {
                            std::cout << "-";
                        }
                        std::cout << "\n";
                        transformInput = getIntegerInput("Transform the ship:\n[1] Rotate 90 degrees\n[2] Confirm Orientation\n[3] Return to Ship Selection\nEnter your choice: ", 1, 3);
                        if (transformInput >= 2) {
                            break;
                        }
                        ships[placementInput - 1]->rotateClockwise();
                    }
                }
                else {
                    while (true) {
                        std::cout << "\nShip " + std::to_string(placementInput) << "\n";
                        for (int j = 0; j < std::max(4, ships[placementInput - 1]->getWidth() * 2); j++) {
                            std::cout << "-";
                        }
                        std::cout << "\n";
                        printShip(ships[placementInput - 1]);
                        for (int j = 0; j < std::max(4, ships[placementInput - 1]->getWidth() * 2); j++) {
                            std::cout << "-";
                        }
                        std::cout << "\n";
                        transformInput = getIntegerInput("Transform the ship:\n[1] Rotate 90 Degrees Clockwise\n[2] Rotate 90 Degrees Counter-Clockwise\n[3] Flip Horizontally\n[4] Flip Vertically\n[5] Confirm Orientation\n[6] Return to Ship Selection\nEnter your choice: ", 1, 6);
                        if (transformInput >= 5) {
                            break;
                        }
                        switch (transformInput) {
                        case 1:
                            ships[placementInput - 1]->rotateClockwise();
                            break;
                        case 2:
                            ships[placementInput - 1]->rotateCounterClockwise();
                            break;
                        case 3:
                            ships[placementInput - 1]->flipHorizontal();
                            break;
                        case 4:
                            ships[placementInput - 1]->flipVertical();
                            break;
                        }
                    }
                }
                
                if ((transformInput == 3 && classic) || (transformInput == 6 && !classic)) {
                    continue;
                }
                // Place ship
                while (true) {
                    std::cout << "\nBoard:\n";
                    printPlacementBoard(placementBoard, totalShips);
                    std::cout << "\n";

                    std::cout << "\nShip " + std::to_string(placementInput) << "\n";
                    for (int j = 0; j < std::max(4, ships[placementInput - 1]->getWidth() * 2); j++) {
                        std::cout << "-";
                    }
                    std::cout << "\n";
                    printShip(ships[placementInput - 1]);
                    for (int j = 0; j < std::max(4, ships[placementInput - 1]->getWidth() * 2); j++) {
                        std::cout << "-";
                    }
                    std::cout << "\n";
                    shipPos = getTwoIntegersInput("Enter the position to place the ship (row col of the top left point) or 'cancel' to return to ship selection: ", 0, gridSize - 1, 0, gridSize - 1, true, "cancel");
					if (shipPos.first == EXIT_CODE) {
						break;
					}
					ships[placementInput - 1]->setPosition(shipPos.first, shipPos.second);
                    if (ships[placementInput - 1]->addToBoard(placementBoard)) {
                        placed[placementInput - 1] = true;
                        shipsPlaced++;
						std::cout << GREEN << "Ship " + std::to_string(placementInput) + " placed successfully!\n" << RESET;
                        break;
                    }
                    else {
						std::cout << RED << "Invalid placement. Please try again.\n" << RESET;
                    }
                }
				if (shipPos.first == EXIT_CODE) {
					continue;
				}
            }
            else {
                ships[placementInput - 1]->removeFromBoard(placementBoard);
                placed[placementInput - 1] = false;
                shipsPlaced--;
                std::cout << GREEN << "Ship " + std::to_string(placementInput) + " removed successfully!\n" << RESET;
            }
        }
        player.setPlacementBoard(placementBoard);
        player.setShips(ships);
        std::cout << "\n===== Player " + (players > 1 ? std::to_string(i) + " " : "") + "Setup =====\n";
        std::cout << "Board:\n";
        printPlacementBoard(placementBoard, totalShips);
		std::cout << "\nPlayer " + (players > 1 ? std::to_string(i) + " " : "") + "setup complete!\n";
		std::cout << "\nPress Enter to continue...";
        std::cin.get();
		clearOutput(); // clear output so that the next player doesn't see the previous player's board
    }
    std::cout << "Bot setup in progress...\n";
    if (players == 1) {
        // Bot setup
        // Bot is player2
        std::vector<std::unique_ptr<Ship>> ships;
        for (const auto& ship : baseShips)
        {
            ships.push_back(ship->clone());
        }
        std::vector<std::vector<char>> placementBoard = std::vector<std::vector<char>>(
            gridSize, std::vector<char>(gridSize, '.')
        );
        if (placeShips(placementBoard, gridSize, ships, 0)) {
            player2.setPlacementBoard(placementBoard);
            player2.setShips(ships);
        }
        else {
            std::cout << RED << "\nThere was an error during bot setup. Press Enter to return to Main Menu..." << RESET;
            std::cin.get();
            clearOutput();
            return EXIT_CODE;
        }
        
    }
    clearOutput();
    return 0;
}

bool placeShips(std::vector<std::vector<char>>& placementBoard, int gridSize, std::vector<std::unique_ptr<Ship>>& ships, int index) {
    if (index == ships.size()) return true;
    std::unique_ptr<Ship>& ship = ships[index];
    if (randomInt(0, 1) == 1) { // choose whether to start flipped or not
        ship->flipHorizontal();
    }
    int rotations[] = {0, 1, 2, 3};
    std::shuffle(std::begin(rotations), std::end(rotations), gen);
    for (int rotations : rotations) {
        for (int r = 0; r < rotations; r++) {
            ship->rotateClockwise();
        }
        std::vector<std::pair<int, int>> positions = std::vector<std::pair<int, int>>();
        for (int i = 0; i <= gridSize - ship->getHeight(); i++) {
            for (int j = 0; j <= gridSize - ship->getWidth(); j++) {
                positions.push_back({ i, j });
            }
        }
        std::shuffle(positions.begin(), positions.end(), gen);
        for (std::pair<int, int> position : positions) {
            ship->setPosition(position.first, position.second);
            if (ship->addToBoard(placementBoard)) {
                if (placeShips(placementBoard, gridSize, ships, index + 1)) {
                    return true;
                }
                ship->removeFromBoard(placementBoard);
            }
        }
        // reset
        for (int r = 0; r < rotations; r++) {
            ship->rotateCounterClockwise();
        }
    }
    // do other orientation
    ship->flipHorizontal();
    std::shuffle(std::begin(rotations), std::end(rotations), gen);
    for (int rotations : rotations) {
        for (int r = 0; r < rotations; r++) {
            ship->rotateClockwise();
        }
        std::vector<std::pair<int, int>> positions = std::vector<std::pair<int, int>>();
        for (int i = 0; i <= gridSize - ship->getHeight(); i++) {
            for (int j = 0; j <= gridSize - ship->getWidth(); j++) {
                positions.push_back({ i, j });
            }
        }
        std::shuffle(positions.begin(), positions.end(), gen);
        for (std::pair<int, int> position : positions) {
            ship->setPosition(position.first, position.second);
            if (ship->addToBoard(placementBoard)) {
                if (placeShips(placementBoard, gridSize, ships, index + 1)) {
                    return true;
                }
                ship->removeFromBoard(placementBoard);
            }
        }
        // reset
        for (int r = 0; r < rotations; r++) {
            ship->rotateCounterClockwise();
        }
    }
    return false;
}

int playSingleplayer(PlayerManager& player, PlayerManager& bot, const std::vector<std::unique_ptr<Ship>>& baseShips, int gridSize) {
    int turn = 1;
    std::deque<std::pair<std::pair<int, int>, std::pair<int, int>>> botAttackDeque = std::deque<std::pair<std::pair<int, int>, std::pair<int, int>>>();
    int winner = -1;
    bool tiePotential = false;
    while (winner == -1 || tiePotential) {
        if (turn == 1) {
			std::cout << "\n===== Player's Turn =====\n";
            std::cout << "\nBoard:\n";
            printGameBoard(bot.getDisplayBoard());
            std::cout << "\nShips:\n";
            printShips(baseShips, bot.getSunkList());
			std::pair<int, int> attackPos = getTwoIntegersInput("Enter the position to attack (row col) or 'exit' to leave the game and return to Main Menu: ", 0, gridSize - 1, 0, gridSize - 1, true);
            if (attackPos.first == EXIT_CODE) {
                clearOutput();
                return EXIT_CODE;
            }
            switch (bot.attack(attackPos.first, attackPos.second)) {
            case -1:
				std::cout << RED << "Invalid attack. Please try again.\n" << RESET;
                continue;
            case 0:
                std::cout << YELLOW << "Miss!\n" << RESET;
                break;
            case 1:
				std::cout << YELLOW << "Hit!\n" << RESET;
                break;
            case 2:
				std::cout << YELLOW << "Hit! You sunk a ship!\n" << RESET;
                if (bot.getShipsRemaining() == 0) {
                    if (player.hasOnePointRemaining()) {
                        tiePotential = true;
                        std::cout << YELLOW << "You sunk all of the ships! The Bot has one chance to tie.\n" << RESET;
                    }
                    winner = 1;
                }
                break;
            }
            std::cout << "\nBoard:\n";
            printGameBoard(bot.getDisplayBoard());
            std::cout << "\nShips:\n";
            printShips(baseShips, bot.getSunkList());
        }
        else {
			std::cout << "\n===== Bot's Turn =====\n";
            std::pair<int, int> attackPos;
            int result = -1;
            do {
                if (botAttackDeque.empty()) {
                    attackPos = { randomInt(0, gridSize - 1), randomInt(0, gridSize - 1) };
                    result = player.attack(attackPos.first, attackPos.second);
                    if (result == 1) {
                        // add adjacent positions
                        switch(randomInt(0, 1)) { // randomize going up/down vs left/right first
                        case 0: // up/down first
                            // left and right
                            if (randomInt(0, 1) == 0) { // left then right
                                if (attackPos.second < gridSize - 1) {
                                    botAttackDeque.push_front({ attackPos, {attackPos.first, attackPos.second + 1} });
                                }
                                if (attackPos.second >  0) {
                                    botAttackDeque.push_front({ attackPos, {attackPos.first, attackPos.second - 1} });
                                }
                            }
                            else { // right then left
                                if (attackPos.second > 0) {
                                    botAttackDeque.push_front({ attackPos, {attackPos.first, attackPos.second - 1} });
                                }
                                if (attackPos.second < gridSize - 1) {
                                    botAttackDeque.push_front({ attackPos, {attackPos.first, attackPos.second + 1} });
                                }
                            }
                            // up and down
                            if (randomInt(0, 1) == 0) { // up then down
                                if (attackPos.first < gridSize - 1) {
                                    botAttackDeque.push_front({ attackPos, {attackPos.first + 1, attackPos.second} });
                                }
                                if (attackPos.first > 0) {
                                    botAttackDeque.push_front({ attackPos, {attackPos.first - 1, attackPos.second} });
                                }
                            }
                            else { // down then up
                                if (attackPos.first > 0) {
                                    botAttackDeque.push_front({ attackPos, {attackPos.first - 1, attackPos.second} });
                                }
                                if (attackPos.first < gridSize - 1) {
                                    botAttackDeque.push_front({ attackPos, {attackPos.first + 1, attackPos.second} });
                                }
                            }
                            break;
                        case 1: // left/right first
                            // up and down
                            if (randomInt(0, 1) == 0) { // up then down
                                if (attackPos.first < gridSize - 1) {
                                    botAttackDeque.push_front({ attackPos, {attackPos.first + 1, attackPos.second} });
                                }
                                if (attackPos.first > 0) {
                                    botAttackDeque.push_front({ attackPos, {attackPos.first - 1, attackPos.second} });
                                }
                            }
                            else { // down then up
                                if (attackPos.first > 0) {
                                    botAttackDeque.push_front({ attackPos, {attackPos.first - 1, attackPos.second} });
                                }
                                if (attackPos.first < gridSize - 1) {
                                    botAttackDeque.push_front({ attackPos, {attackPos.first + 1, attackPos.second} });
                                }
                            }
                            // left and right
                            if (randomInt(0, 1) == 0) { // left then right
                                if (attackPos.second < gridSize - 1) {
                                    botAttackDeque.push_front({ attackPos, {attackPos.first, attackPos.second + 1} });
                                }
                                if (attackPos.second > 0) {
                                    botAttackDeque.push_front({ attackPos, {attackPos.first, attackPos.second - 1} });
                                }
                            }
                            else { // right then left
                                if (attackPos.second > 0) {
                                    botAttackDeque.push_front({ attackPos, {attackPos.first, attackPos.second - 1} });
                                }
                                if (attackPos.second < gridSize - 1) {
                                    botAttackDeque.push_front({ attackPos, {attackPos.first, attackPos.second + 1} });
                                }
                            }
                            break;
                        }
                    }
                }
                else {
                    std::pair<std::pair<int, int>, std::pair<int, int>> nextAttack = botAttackDeque.front();
                    botAttackDeque.pop_front();
                    std::pair<int, int> prevPos = nextAttack.first;
                    if (player.alreadySunk(prevPos.first, prevPos.second)) {
                        continue;
                    }
                    attackPos = nextAttack.second;
                    result = player.attack(attackPos.first, attackPos.second);
                    if (result == 1) {
                        // add same direction to the front and perpendicular to the back
                        std::pair<int, int> diff = { attackPos.first - prevPos.first, attackPos.second - prevPos.second };
                        if (diff.first == 1) { // down
                            if (attackPos.first < gridSize - 1) {
                                botAttackDeque.push_front({ attackPos, { attackPos.first + 1, attackPos.second } });
                            }
                            // left and right
                            if (randomInt(0, 1) == 0) { // right then left
                                if (attackPos.second < gridSize - 1) {
                                    botAttackDeque.push_back({ attackPos, {attackPos.first, attackPos.second + 1} });
                                }
                                if (attackPos.second > 0) {
                                    botAttackDeque.push_back({ attackPos, {attackPos.first, attackPos.second - 1} });
                                }
                            }
                            else { // left then right
                                if (attackPos.second > 0) {
                                    botAttackDeque.push_back({ attackPos, {attackPos.first, attackPos.second - 1} });
                                }
                                if (attackPos.second < gridSize - 1) {
                                    botAttackDeque.push_back({ attackPos, {attackPos.first, attackPos.second + 1} });
                                }
                            }
                        }
                        else if (diff.first == -1) { // up
                            if (attackPos.first >  0) {
                                botAttackDeque.push_front({ attackPos, { attackPos.first - 1, attackPos.second } });
                            }
                            // left and right
                            if (randomInt(0, 1) == 0) { // right then left
                                if (attackPos.second < gridSize - 1) {
                                    botAttackDeque.push_back({ attackPos, {attackPos.first, attackPos.second + 1} });
                                }
                                if (attackPos.second > 0) {
                                    botAttackDeque.push_back({ attackPos, {attackPos.first, attackPos.second - 1} });
                                }
                            }
                            else { // left then right
                                if (attackPos.second > 0) {
                                    botAttackDeque.push_back({ attackPos, {attackPos.first, attackPos.second - 1} });
                                }
                                if (attackPos.second < gridSize - 1) {
                                    botAttackDeque.push_back({ attackPos, {attackPos.first, attackPos.second + 1} });
                                }
                            }
                        }
                        else if (diff.second == 1) { // right
                            if (attackPos.second < gridSize - 1) {
                                botAttackDeque.push_front({ attackPos, {attackPos.first, attackPos.second + 1} });
                            }
                            // up and down
                            if (randomInt(0, 1) == 0) { // down then up
                                if (attackPos.first < gridSize - 1) {
                                    botAttackDeque.push_back({ attackPos, {attackPos.first + 1, attackPos.second} });
                                }
                                if (attackPos.first > 0) {
                                    botAttackDeque.push_back({ attackPos, {attackPos.first - 1, attackPos.second} });
                                }
                            }
                            else { // up then down
                                if (attackPos.first > 0) {
                                    botAttackDeque.push_back({ attackPos, {attackPos.first - 1, attackPos.second} });
                                }
                                if (attackPos.first < gridSize - 1) {
                                    botAttackDeque.push_back({ attackPos, {attackPos.first + 1, attackPos.second} });
                                }
                            }
                        }
                        else { // left
                            if (attackPos.second > 0) {
                                botAttackDeque.push_front({ attackPos, {attackPos.first, attackPos.second - 1} });
                            }
                            // up and down
                            if (randomInt(0, 1) == 0) { // down then up
                                if (attackPos.first < gridSize - 1) {
                                    botAttackDeque.push_back({ attackPos, {attackPos.first + 1, attackPos.second} });
                                }
                                if (attackPos.first > 0) {
                                    botAttackDeque.push_back({ attackPos, {attackPos.first - 1, attackPos.second} });
                                }
                            }
                            else { // up then down
                                if (attackPos.first > 0) {
                                    botAttackDeque.push_back({ attackPos, {attackPos.first - 1, attackPos.second} });
                                }
                                if (attackPos.first < gridSize - 1) {
                                    botAttackDeque.push_back({ attackPos, {attackPos.first + 1, attackPos.second} });
                                }
                            }
                        }
                    }
                }
            } while (result == -1);
            
            switch (result) {
            case 0:
				std::cout << YELLOW << "The bot attacked (" + std::to_string(attackPos.first) + ", " + std::to_string(attackPos.second) + ") and missed!\n" << RESET;
				break;
            case 1:
				std::cout << YELLOW << "The bot attacked (" + std::to_string(attackPos.first) + ", " + std::to_string(attackPos.second) + ") and hit your ship!\n" << RESET;
				break;
            case 2:
                std::cout << YELLOW << "The bot attacked (" + std::to_string(attackPos.first) + ", " + std::to_string(attackPos.second) + ") and sunk your ship!\n" << RESET;
                if (player.getShipsRemaining() == 0) {
                    if (tiePotential) {
                        winner = 0;
                        tiePotential = false;
                    }
                    else {
                        winner = 2;
                    }
                }
                break;
            }
            std::cout << "\nBoard:\n";
            printGameBoard(player.getDisplayBoard());
            std::cout << "\nShips:\n";
            printShips(baseShips, player.getSunkList());
        }
        if (winner == -1) {
            std::cout << "Press Enter to continue...";
            std::cin.get();
        }
        turn = 3 - turn;
    }

    if (winner == 0) {
        std::cout << YELLOW << "Tie!\n" << RESET;
    }
    else if (winner == 1) {
        std::cout << YELLOW << "You Win!\n" << RESET;
    }
    else {
        std::cout << YELLOW << "The Bot Wins!\n" << RESET;
    }
    std::cout << "\nPress Enter to view boards...";
    std::cin.get();
    clearOutput();
    printEndBoards(winner, true, player, bot, baseShips);
    return 0;
}

int playMultiplayer(PlayerManager& player1, PlayerManager& player2, const std::vector<std::unique_ptr<Ship>>& baseShips, int gridSize) {
    int turn = 1;
    int winner = -1;
    bool tiePotential = false;
    while (winner == -1 || tiePotential) {
        PlayerManager& currPlayerManager = (turn == 1 ? player2 : player1);
        std::cout << "\n===== Player " << turn << "'s Turn =====\n";
        std::cout << "\nBoard:\n";
        printGameBoard(currPlayerManager.getDisplayBoard());
        std::cout << "\nShips:\n";
        printShips(baseShips, currPlayerManager.getSunkList());
        std::pair<int, int> attackPos = getTwoIntegersInput("Enter the position to attack (row col) or 'exit' to leave the game and return to Main Menu: ", 0, gridSize - 1, 0, gridSize - 1, true);
        if (attackPos.first == EXIT_CODE) {
            clearOutput();
            return EXIT_CODE;
        }
        switch (currPlayerManager.attack(attackPos.first, attackPos.second)) {
        case -1:
            std::cout << RED << "Invalid attack. Please try again.\n" << RESET;
            continue;
        case 0:
            std::cout << YELLOW << "Miss!\n" << RESET;
            break;
        case 1:
            std::cout << YELLOW << "Hit!\n" << RESET;
            break;
        case 2:
            std::cout << YELLOW << "Hit! Player " << turn << " sunk a ship!\n" << RESET;
            if (currPlayerManager.getShipsRemaining() == 0) {
                if (tiePotential) {
                    tiePotential = false;
                    winner = 0;
                }
                else {
                    if (turn == 1) {
                        if (player1.hasOnePointRemaining()) {
                            tiePotential = true;
                            std::cout << YELLOW << "Player 1 sunk all of the ships! Player 2 has one chance to tie.\n" << RESET;
                        }
                    }
                    winner = turn;
                }
            }
            break;
        }
        std::cout << "\nBoard:\n";
        printGameBoard(currPlayerManager.getDisplayBoard());
        std::cout << "\nShips:\n";
        printShips(baseShips, currPlayerManager.getSunkList());
        if (winner == 0) {
            std::cout << "Press Enter to continue...";
            std::cin.get();
        }
        turn = 3 - turn;
    }

    if (winner == 0) {
        std::cout << YELLOW << "Tie!\n" << RESET;
    }
    else if (winner == 1) {
        std::cout << YELLOW << "\nPlayer 1 Wins!\n" << RESET;
    }
    else {
        std::cout << YELLOW << "\nPlayer 2 Wins!\n" << RESET;
    }
    std::cout << "\nPress Enter to view boards...";
    std::cin.get();
    clearOutput();
    printEndBoards(winner, false, player1, player2, baseShips);
    return 0;
}

void printEndBoards(int winner, bool singleplayer, PlayerManager& player1, PlayerManager& player2, const std::vector<std::unique_ptr<Ship>>& baseShips) {
    std::cout << "===== Boards =====\n";
    if (singleplayer) {
        std::cout << "\nPlayer Board:\n";
    }
    else {
        std::cout << "\nPlayer 1 Board:\n";
    }
    printEndBoard(player1.getPlacementBoard(), player1.getDisplayBoard(), player1.getSunkList().size());
    if (singleplayer) {
        std::cout << "\nPlayer Ships:\n";
    }
    else {
        std::cout << "\nPlayer 1 Ships:\n";
    }
    printShips(baseShips, player1.getSunkList());
    std::cout << "\n";
    if (singleplayer) {
        std::cout << "\nBot Board:\n";
    }
    else {
        std::cout << "\nPlayer 2 Board:\n";
    }
    printEndBoard(player2.getPlacementBoard(), player2.getDisplayBoard(), player2.getSunkList().size());
    if (singleplayer) {
        std::cout << "\nBot Ships:\n";
    }
    else {
        std::cout << "\nPlayer 2 Ships:\n";
    }
    printShips(baseShips, player2.getSunkList());
    if (winner == 0) {
        std::cout << YELLOW << "Tie\n" << RESET;
    }
    else {
        std::cout << YELLOW << "Winner: ";
        if (winner == 1) {
            if (singleplayer) {
                std::cout << "Player\n" << RESET;
            }
            else {
                std::cout << "Player 1\n" << RESET;
            }
        }
        else {
            if (singleplayer) {
                std::cout << "Bot\n" << RESET;
            }
            else {
                std::cout << "Player 2\n" << RESET;
            }
        }
    }
    std::cout << "\nPress Enter to return to Main Menu...";
    std::cin.get();
    clearOutput();
}

void printEndBoard(const std::vector<std::vector<char>>& placementBoard, const std::vector<std::vector<char>>& displayBoard, int numShips) {
    int n = placementBoard.size();
    std::cout << "  ";
    bool extraSpace = n > 10 || numShips >= 10;
    if (extraSpace) {
        std::cout << " ";
    }
    std::cout << YELLOW;
    for (int i = 0; i < n; i++) {
        std::cout << i << " ";
        if (extraSpace && i < 10) {
            std::cout << " ";
        }
    }
    std::cout << RESET;
    std::cout << "\n";
    for (int i = 0; i < n; i++) {
        std::cout << YELLOW << i << " " << RESET;
        if (extraSpace && i < 10) {
            std::cout << " ";
        }
        for (int j = 0; j < n; j++) {
            if (displayBoard[i][j] == '-') {
                if (placementBoard[i][j] == '.') {
                    std::cout << RESET << placementBoard[i][j] << " ";
                    if (extraSpace) {
                        std::cout << " ";
                    }
                }
                else {
                    std::cout << GREEN << placementBoard[i][j] - '0' << " ";
                    if (extraSpace && placementBoard[i][j] - '0' < 10) {
                        std::cout << " ";
                    }
                }
            }
            else {
                if (displayBoard[i][j] == '.') {
                    std::cout << BLUE;
                }
                else {
                    std::cout << RED;
                }
                std::cout << displayBoard[i][j] << " ";
                if (extraSpace) {
                    std::cout << " ";
                }
            }
            std::cout << RESET;
        }
        std::cout << "\n";
    }
    std::cout << RESET;
}

void printShip(const std::unique_ptr<Ship>& ship) {
    std::cout << BLUE;
    for (int i = 0; i < ship->getHeight(); i++) {
        for (int j = 0; j < ship->getWidth(); j++) {
            if (ship->getShipGrid()[i][j]) {
                std::cout << "o";
            }
            else {
                std::cout << " ";
            }
            std::cout << " ";
        }
        std::cout << "\n";
    }
    std::cout << RESET;
}

void printGameBoard(const std::vector<std::vector<char>>& board) {
    int n = board.size();
    std::cout << "  ";
    if (n > 10) {
        std::cout << " ";
    }
    std::cout << YELLOW;
    for (int i = 0; i < n; i++) {
        std::cout << i << " ";
        if (n > 10 && i < 10) {
            std::cout << " ";
        }
    }
    std::cout << RESET;
    std::cout << "\n";
    for (int i = 0; i < n; i++) {
        std::cout << YELLOW << i << " " << RESET;
        if (n > 10 && i < 10) {
            std::cout << " ";
        }
        for (int j = 0; j < n; j++) {
            if (board[i][j] == '.') {
                std::cout << BLUE;
            }
            else if (board[i][j] == 'o' || board[i][j] == 'x') {
                std::cout << RED;
            }
            std::cout << board[i][j] << " ";
            if (n > 10) {
                std::cout << " ";
            }
            std::cout << RESET;
        }
        std::cout << "\n";
    }
    std::cout << RESET;
}

void printPlacementBoard(const std::vector<std::vector<char>>& board, int numShips) {
    int n = board.size();
	std::cout << "  ";
    bool extraSpace = n > 10 || numShips >= 10;
    if (extraSpace) {
        std::cout << " ";
    }
    std::cout << YELLOW;
    for (int i = 0; i < n; i++) {
		std::cout << i << " ";
		if (extraSpace && i < 10) {
			std::cout << " ";
		}
	}
    std::cout << RESET;
	std::cout << "\n";
	for (int i = 0; i < n; i++) {
		std::cout << YELLOW << i << " " << RESET;
        if (extraSpace && i < 10) {
            std::cout << " ";
        }
		for (int j = 0; j < n; j++) {
            if (board[i][j] != '.') {
                std::cout << BLUE << board[i][j] - '0' << " ";
            }
            else {
                std::cout << board[i][j] << " ";
            }
            if (extraSpace && (board[i][j] == '.' || (board[i][j] != '.' && board[i][j] - '0' < 10))) {
                std::cout << " ";
            }
            std::cout << RESET;
		}
		std::cout << "\n";
	}
    std::cout << RESET;
}

void printShips(const std::vector<std::unique_ptr<Ship>>& ships) {
    int n = ships.size();
    int i = 0;
    std::cout << RESET;
    while (i < n) {
        int maxHeight = 0;
        // print first row and get max height
        for (int j = i; j < i + SHIP_ROW_LENGTH && j < n; j++) {
            std::cout << "[" + std::to_string(j + 1) + "] ";
            for (int k = 0; k < ships[j]->getWidth(); k++) {
                if (ships[j]->getShipGrid()[0][k]) {
                    std::cout << BLUE << "o" << RESET;
                }
                else {
                    std::cout << " ";
                }
                std::cout << " ";
                maxHeight = std::max(maxHeight, ships[j]->getHeight());
            }
            std::cout << "  ";
        }
        std::cout << "\n";
        for (int j = 1; j < maxHeight; j++) {
            for (int k = i; k < i + SHIP_ROW_LENGTH && k < n; k++) {
                std::cout << "    ";
                if (j >= ships[k]->getHeight()) {
                    for (int l = 0; l < ships[k]->getWidth(); l++) {
                        std::cout << "  ";
                    }
                }
                else {
                    for (int l = 0; l < ships[k]->getWidth(); l++) {
                        if (ships[k]->getShipGrid()[j][l]) {
                            std::cout << BLUE << "o" << RESET;
                        }
                        else {
                            std::cout << " ";
                        }
                        std::cout << " ";
                    }
                }
                std::cout << "  ";

            }
            std::cout << "\n";
        }
        std::cout << "\n";
        i += SHIP_ROW_LENGTH;
    }
}

void printShips(const std::vector<std::unique_ptr<Ship>>& ships, const std::vector<bool>& placed) {
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
            for (int k = 0; k < ships[j]->getWidth(); k++) {
                if (ships[j]->getShipGrid()[0][k]) {
                    std::cout << "o";
                }
                else {
                    std::cout << " ";
                }
				std::cout << " ";
				maxHeight = std::max(maxHeight, ships[j]->getHeight());
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
                if (j >= ships[k]->getHeight()) {
					for (int l = 0; l < ships[k]->getWidth(); l++) {
						std::cout << "  ";
					}
                }
                else {
                    for (int l = 0; l < ships[k]->getWidth(); l++) {
                        if (ships[k]->getShipGrid()[j][l]) {
                            std::cout << "o";
                        }
                        else {
                            std::cout << " ";
                        }
                        std::cout << " ";
                    }
                }
                std::cout << "  ";
                std::cout << RESET;
                
            }
            std::cout << "\n";
        }
        std::cout << "\n";
		i += SHIP_ROW_LENGTH;
    }
}

int getIntegerInput(std::string prompt, int min, int max, bool allowExit, std::string exitString) {
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
                std::cout << RED << "Invalid input. Please enter an integer from " << min << " to " << max << (allowExit ? " or '" + exitString + "'" : "") + ".\n" << RESET;
                continue;
            }
        }
        catch (...) {
            std::cout << RED << "Invalid input. Please enter an integer from " << min << " to " << max << (allowExit ? " or '" + exitString + "'" : "") + ".\n" << RESET;
            continue;
        }
		if (input < min || input > max) {
			std::cout << RED << "Invalid input. Please enter an integer from " << min << " to " << max << (allowExit ? " or '" + exitString + "'" : "") + ".\n" << RESET;
			continue;
		}
        return input;
    }
}

std::pair<int, int> getTwoIntegersInput(std::string prompt, int min1, int max1, int min2, int max2, bool allowExit, std::string exitString) {
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
				std::cout << RED << "Invalid input. Please enter an integer from " << min1 << " to " << max1 << " and an integer from " << min2 << " to " << max2 << ", separated by a space" + (allowExit ? ", or '" + exitString + "'" : "") + ".\n" << RESET;
				continue;
			}
        }
        catch (...) {
            std::cout << RED << "Invalid input. Please enter an integer from " << min1 << " to " << max1 << " and an integer from " << min2 << " to " << max2 << ", separated by a space" + (allowExit ? ", or '" + exitString + "'" : "") + ".\n" << RESET;
            continue;
        }
        if (input.first < min1 || input.first > max1 || input.second < min2 || input.second > max2) {
            std::cout << RED << "Invalid input. Please enter an integer from " << min1 << " to " << max1 << " and an integer from " << min2 << " to " << max2 << ", separated by a space" + (allowExit ? ", or '" + exitString + "'" : "") + ".\n" << RESET;
            continue;
        }
        return input;
    }
}
