#include <vector>

#include "player_manager.h"

void PlayerManager::setPlacementBoard(std::vector<std::vector<char>>& const board) {
	placementBoard = board;
	displayBoard = std::vector<std::vector<char>>(
		board.size(),
		std::vector<char>(board[0].size(), '-')
	);
}

void PlayerManager::setShips(std::vector<Ship>& const shipList) {
	ships = shipList;
	shipSunk = std::vector<bool>(ships.size(), false);
	shipsRemaining = ships.size();
}

// -1 = invalid
// 0 = miss
// 1 = hit
// 2 = sunk
int PlayerManager::attack(int r, int c) {
	if (displayBoard[r][c] != '-') {
		return -1;
	}
	else if (placementBoard[r][c] == '.') {
		displayBoard[r][c] = '.';
		return 0;
	}
	else {
		int shipNum = placementBoard[r][c] - '0';
		if (ships[shipNum - 1].hit(r, c, displayBoard)) {
			shipsRemaining--;
			shipSunk[shipNum - 1] = true;
			return 2;
		}
		else {
			return 1;
		}
	}
}

int PlayerManager::getShipsRemaining() const {
	return shipsRemaining;
}

const std::vector<bool>& PlayerManager::getSunkList() const {
	return shipSunk;
}

const std::vector<std::vector<char>>& PlayerManager::getDisplayBoard() const {
	return displayBoard;
}