//
// Created by smmm on 11.05.2025.
//

#include "Point.h"

bool Point::operator==(const Point& other) const
{
	return x == other.x && y == other.y;
}