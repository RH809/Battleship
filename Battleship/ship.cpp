#include "ship.h"

Ship::Ship(int w, int h, int _x, int _y, int r) : width(w), height(h), x(_x), y(_y), rotation(r) {
	numPoints = (w * h);
}

int Ship::getWidth() const { return width; }
int Ship::getHeight() const { return height; }
int Ship::getX() const { return x; }
int Ship::getY() const { return y; }
void Ship::setPosition(int _x, int _y) { x = _x; y = _y; }
int Ship::getRotation() const { return rotation; }
void Ship::setRotation(int r) { rotation = r; }