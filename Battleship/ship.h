#ifndef SHIP_H
#define SHIP_H

#include <vector>

class Ship {
protected:
	int width, height;
	int x, y;
	int numPoints;
	int shipNum;
	std::vector<std::vector<bool>> shipGrid;

	virtual bool isValidPosition(const std::vector<std::vector<char>>& board) const; // check if the ship can be placed at the current position and rotation
	virtual void sink(std::vector<std::vector<char>>& board) const;
	virtual void generateShipGrid();

public:
	Ship(int w, int h, int _x, int _y);
	int getWidth() const;
	int getHeight() const;
	int getX() const;
	int getY() const;
	void setPosition(int _x, int _y);
	void setShipNum(int num);

	// Transformations
	virtual void rotateClockwise();
	virtual void rotateCounterClockwise();
	virtual void flipHorizontal();
	virtual void flipVertical();

	virtual bool addToBoard(std::vector<std::vector<char>>& board) const;
	virtual void removeFromBoard(std::vector<std::vector<char>>& board) const;
	virtual bool hit(int hitX, int hitY, std::vector<std::vector<char>>& board); // ship point on ship; true = sunk
};

#endif