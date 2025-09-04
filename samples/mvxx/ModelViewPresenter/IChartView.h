#pragma once

struct Point2D
{
	Point2D() = default;
	Point2D(double x, double y) 
		:x(x), y(y) 
	{}
	double x = 0;
	double y = 0;
};

class IChartView
{
public:
	typedef std::vector<Point2D> Points2D;
	virtual void SetData(const Points2D & data) = 0;
	virtual ~IChartView() = default;
};

