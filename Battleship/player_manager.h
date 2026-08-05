#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include <string>
#include <vector>

#include "ship.h"
#include "custom_ship.h"

class PlayerManager {
private:
	int shipsRemaining;
	std::vector<std::vector<char>> placementBoard; // board for ship placement
	std::vector<std::vector<char>> displayBoard; // board for display while playing
	std::vector<bool> shipSunk;
	std::vector<Ship> ships;

public:
	int getShipsRemaining() const;
	const std::vector<bool>& getSunkList() const;
	const std::vector<std::vector<char>>& getDisplayBoard() const;

	void setPlacementBoard(std::vector<std::vector<char>>& const board);
	void setShips(std::vector<Ship>& const shipList);

	int attack(int r, int c);
};

#endif