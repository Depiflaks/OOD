#ifndef GEOMETRY_H
#define GEOMETRY_H

namespace shapes
{

struct Position
{
	double x;
	double y;

	Position(double x = 0.0, double y = 0.0)
		: x(x)
		, y(y)
	{
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
	Position position;
	Size size;

	Bounds(Position pos = Position(), Size size = Size())
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

} // namespace shapes

#endif /* GEOMETRY_H */
