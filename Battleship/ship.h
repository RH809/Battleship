#ifndef SHIP_H
#define SHIP_H

#include <vector>

class Ship {
protected:
	const int width, height;
	int x, y;
	int rotation; // 0 - 3 based on the number of 90 degree rotations
	int numPoints;

	bool isValidPosition(const std::vector<std::vector<char>>& board) const; // check if the ship can be placed at the current position and rotation
public:
	Ship(int w, int h, int _x, int _y, int r);
	int getWidth() const;
	int getHeight() const;
	int getX() const;
	int getY() const;
	void setPosition(int _x, int _y);
	int getRotation() const;
	void setRotation(int r);

	bool addToBoard(std::vector<std::vector<char>>& board) const;
	void removeFromBoard(std::vector<std::vector<char>>& board) const;
	bool hit(int hitX, int hitY); // ship point on ship; true = sunk
};

#endif