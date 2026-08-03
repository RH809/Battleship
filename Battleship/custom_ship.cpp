#include "custom_ship.h"

CustomShip::CustomShip(int w, int h, int _x, int _y, const std::vector<std::pair<int, int>>& ex) : Ship(w, h, _x, _y), exclusions(ex) {
	numPoints = (w * h) - exclusions.size();
	generateShipGrid();
}

bool CustomShip::isValidPosition(const std::vector<std::vector<char>>& board) const {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (shipGrid[i][j]) {
				int boardX = x + j;
				int boardY = y + i;
				if (boardY < 0 || boardY >= board.size() || boardX < 0 || boardX >= board[0].size()) {
					return false;
				}
				if (board[boardY][boardX] != '.') {
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
				board[y + i][x + j] = '0' + shipNum;
			}
		}
	}
	return true;
}

void CustomShip::removeFromBoard(std::vector<std::vector<char>>& board) const {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (shipGrid[i][j]) {
				board[y + i][x + j] = '.';
			}
		}
	}
}

bool CustomShip::hit(int hitX, int hitY, std::vector<std::vector<char>>& board) {
	if (!(hitX >= x && hitX < x + width && hitY >= y && hitY < y + height) || !shipGrid[hitX - x][hitY - y] ||
			board[hitX][hitY] != '0' + shipNum) {
		return false;
	}
	numPoints--;
	board[hitX][hitY] = '*';
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
				board[y + i][x + j] = 'x';
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

