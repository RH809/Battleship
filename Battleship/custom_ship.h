#pragma once
#ifndef CUSTOM_SHIP_H
#define CUSTOM_SHIP_H

#include <vector>

#include "ship.h"

class CustomShip : public Ship {
protected:
	std::vector<std::pair<int, int>> exclusions;

	bool isValidPosition(const std::vector<std::vector<char>>& board) const override;
	void sink(std::vector<std::vector<char>>& board) const override;
	void generateShipGrid() override;

public:
	CustomShip(int w, int h, int _r, int _c, const std::vector<std::pair<int, int>>& ex);

	void rotateClockwise() override;
	void rotateCounterClockwise() override;
	void flipHorizontal() override;
	void flipVertical() override;

	bool addToBoard(std::vector<std::vector<char>>& board) const override;
	void removeFromBoard(std::vector<std::vector<char>>& board) const override;
	bool hit(int hitR, int hitC, std::vector<std::vector<char>>& board) override;
};

#endif	