#include "ship.h"

Ship::Ship(int w, int h, int _x, int _y) : width(w), height(h), x(_x), y(_y) {
	shipNum = 0;
	numPoints = (w * h);
	generateShipGrid();
}

int Ship::getWidth() const { return width; }
int Ship::getHeight() const { return height; }
int Ship::getX() const { return x; }
int Ship::getY() const { return y; }
void Ship::setPosition(int _x, int _y) { x = _x; y = _y; }
void Ship::setShipNum(int num) { shipNum = num; }

bool Ship::isValidPosition(const std::vector<std::vector<char>>& board) const {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
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
	return true;
}

bool Ship::addToBoard(std::vector<std::vector<char>>& board) const {
	if (!isValidPosition(board)) {
		return false;
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			board[y + i][x + j] = '0' + shipNum;
		}
	}
	return true;
}

void Ship::removeFromBoard(std::vector<std::vector<char>>& board) const {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			board[y + i][x + j] = '.';
		}
	}
}

bool Ship::hit(int hitX, int hitY, std::vector<std::vector<char>>& board) {
	if (!(hitX >= x && hitX < x + width && hitY >= y && hitY < y + height) || board[hitX][hitY] != '0' + shipNum) {
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

void Ship::sink(std::vector<std::vector<char>>& board) const {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			board[y + i][x + j] = 'x';
		}
	}
}

void Ship::rotateClockwise() {
	int temp = width;
	width = height;
	height = temp;

	generateShipGrid();
}

void Ship::rotateCounterClockwise() {
	int temp = width;
	width = height;
	height = temp;

	generateShipGrid();
}

void Ship::flipHorizontal() {
	return;
}

void Ship::flipVertical() {
	return;
}

void Ship::generateShipGrid() {
	shipGrid = std::vector<std::vector<bool>>(
		height,
		std::vector<bool>(width, true)
	);
}