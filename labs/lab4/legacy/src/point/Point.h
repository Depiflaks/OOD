//
// Created by smmm on 06.05.2025.
//

#ifndef POINT_H
#define POINT_H
#include <cmath>
#include <istream>
#include <ostream>

struct Point
{
	double x{};
	double y{};

	static double Distance(const Point& a, const Point& b)
	{
		return std::hypot(a.x - b.x, a.y - b.y);
	}

	friend std::ostream& operator<<(std::ostream& os, const Point& point);
	friend std::istream& operator>>(std::istream& in, Point& point);
	bool operator==(const Point& other) const;
};

inline std::ostream& operator<<(std::ostream& os, const Point& point)
{
	os << point.x << " " << point.y;
	return os;
}

inline std::istream& operator>>(std::istream& in, Point& point)
{
	in >> std::ws >> point.x >> std::ws >> point.y;
	return in;
}

#endif // POINT_H
