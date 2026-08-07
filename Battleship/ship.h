#ifndef SHIP_H
#define SHIP_H

#include <vector>
#include <memory>

class Ship {
protected:
	int width, height;
	int r, c;
	int numPoints;
	int shipNum;
	std::vector<std::vector<bool>> shipGrid;

	virtual bool isValidPosition(const std::vector<std::vector<char>>& board) const; // check if the ship can be placed at the current position and rotation
	virtual void sink(std::vector<std::vector<char>>& board) const;
	virtual void generateShipGrid();

public:
	Ship(int w, int h, int _r, int _c);
	virtual ~Ship() = default;
	virtual std::unique_ptr<Ship> clone() const;
	int getWidth() const;
	int getHeight() const;
	int getR() const;
	int getC() const;
	void setPosition(int _r, int _c);
	void setShipNum(int num);
	int getShipNum() const;

	std::vector<std::vector<bool>> getShipGrid() const;

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