#include "custom_ship.h"

CustomShip::CustomShip(int w, int h, int _x, int _y, int r, const std::vector<std::pair<int, int>>& ex) : Ship(w, h, _x, _y, r), exclusions(ex) {
	numPoints = (w * h) - exclusions.size();
}