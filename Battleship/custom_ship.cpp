#include "custom_ship.h"

CustomShip::CustomShip(int w, int h, int _x, int _y, const std::vector<std::pair<int, int>>& ex) : Ship(w, h, _x, _y), exclusions(ex) {
	numPoints = (w * h) - exclusions.size();
	generateShipGrid();
}

bool CustomShip::isValidPosition(const std::vector<std::vector<char>>& board) const {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (shipGrid[i][j]) {
				int boardR = r + i;
				int boardC = c + j;
				if (boardR < 0 || boardR >= board.size() || boardC < 0 || boardC >= board[0].size()) {
					return false;
				}
				if (board[boardR][boardC] != '.') {
					return false;
				}
			}
		}
	}
	return true;
}


bool CustomShip::addToBoard(std::vector<std::vector<char>>& board) const {
	if (!isValidPosition(board)) {
		return false;
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (shipGrid[i][j]) {
				board[r + i][c + j] = '0' + shipNum;
			}
		}
	}
	return true;
}

void CustomShip::removeFromBoard(std::vector<std::vector<char>>& board) const {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (shipGrid[i][j]) {
				board[r + i][c + j] = '.';
			}
		}
	}
}

bool CustomShip::hit(int hitR, int hitC, std::vector<std::vector<char>>& board) {
	if (!(hitR >= r && hitR < r + height && hitC >= c && hitC < c + width) || !shipGrid[hitR - r][hitC - c] ||
			board[hitR][hitC] != '0' + shipNum) {
		return false;
	}
	numPoints--;
	board[hitR][hitC] = '*';
	if (numPoints == 0) {
		sink(board);
		return true;
	}
	return false;
}

void CustomShip::sink(std::vector<std::vector<char>>& board) const {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (shipGrid[i][j]) {
				board[r + i][c + j] = 'x';
			}
		}
	}
}

void CustomShip::rotateClockwise() {
	int temp = width;
	width = height;
	height = temp;

	for (std::pair<int, int> exclusion : exclusions) {
		int oldX = exclusion.first;
		int oldY = exclusion.second;

		exclusion.second = oldX;
		exclusion.first = width - 1 - oldY;
	}

	generateShipGrid();
}

void CustomShip::rotateCounterClockwise() {
	int temp = width;
	width = height;
	height = temp;

	for (std::pair<int, int> exclusion : exclusions) {
		int oldX = exclusion.first;
		int oldY = exclusion.second;

		exclusion.first = oldY;
		exclusion.second = height - 1 - oldX;
	}

	generateShipGrid();
}

void CustomShip::flipHorizontal() {
	for (std::pair<int, int> exclusion : exclusions) {
		exclusion.first = width - 1 - exclusion.first;
	}

	generateShipGrid();
}

void CustomShip::flipVertical() {
	for (std::pair<int, int> exclusion : exclusions) {
		exclusion.second = height - 1 - exclusion.second;
	}

	generateShipGrid();
}

void CustomShip::generateShipGrid() {
	shipGrid = std::vector<std::vector<bool>>(
		height,
		std::vector<bool>(width, true)
	);

	for (std::pair<int, int> exclusion : exclusions) {
		shipGrid[exclusion.second][exclusion.first] = false;
	}
}

