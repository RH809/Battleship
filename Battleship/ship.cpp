#include <iostream>

#include "ship.h"

Ship::Ship(int w, int h, int _r, int _c) : width(w), height(h), r(_r), c(_c) {
	shipNum = 0;
	numPoints = (w * h);
	generateShipGrid();
}

std::unique_ptr<Ship> Ship::clone() const {
	return std::make_unique<Ship>(*this);
}

int Ship::getWidth() const { return width; }
int Ship::getHeight() const { return height; }
int Ship::getR() const { return r; }
int Ship::getC() const { return c; }
void Ship::setPosition(int _r, int _c) { r = _r; c = _c; }
void Ship::setShipNum(int num) { shipNum = num; }
int Ship::getShipNum() const { return shipNum; }

std::vector<std::vector<bool>> Ship::getShipGrid() const { return shipGrid; }

bool Ship::isValidPosition(const std::vector<std::vector<char>>& board) const {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
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
	return true;
}

bool Ship::addToBoard(std::vector<std::vector<char>>& board) const {
	if (!isValidPosition(board)) {
		return false;
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			board[r + i][c + j] = '0' + shipNum;
		}
	}
	return true;
}

void Ship::removeFromBoard(std::vector<std::vector<char>>& board) const {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			board[r + i][c + j] = '.';
		}
	}
}

bool Ship::hit(int hitR, int hitC, std::vector<std::vector<char>>& board) {
	numPoints--;
	board[hitR][hitC] = 'o';
	if (numPoints == 0) {
		sink(board);
		return true;
	}
	return false;
}

void Ship::sink(std::vector<std::vector<char>>& board) const {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			board[r + i][c + j] = 'x';
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