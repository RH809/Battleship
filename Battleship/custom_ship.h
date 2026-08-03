#pragma once
#ifndef CUSTOM_SHIP_H
#define CUSTOM_SHIP_H

#include <vector>

#include "ship.h"

class CustomShip : public Ship {
private:
	std::vector<std::pair<int, int>> exclusions;
public:
	CustomShip(int w, int h, int _x, int _y, int r,const std::vector<std::pair<int, int>>& ex);
};

#endif	