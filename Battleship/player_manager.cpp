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
}
