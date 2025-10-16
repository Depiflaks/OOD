#ifndef GEOMETRY_H
#define GEOMETRY_H

struct Point
{
	double x;
	double y;

	Point(double x = 0.0, double y = 0.0)
		: x(x)
		, y(y)
	{
	}

	bool operator==(const Point& other) const
	{
		return x == other.x && y == other.y;
	}
};

struct Size
{
	double w;
	double h;

	Size(double w = 0.0, double h = 0.0)
		: w(w)
		, h(h)
	{
	}
};

struct Bounds
{
	Point position;
	Size size;

	Bounds(Point pos = Point(), Size size = Size())
		: position(pos)
		, size(size)
	{
	}

	Bounds(double x, double y, double w, double h)
		: position(x, y)
		, size(w, h)
	{
	}
};

#endif /* GEOMETRY_H */
