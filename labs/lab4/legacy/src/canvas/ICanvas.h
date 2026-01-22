//
// Created by smmm on 06.05.2025.
//

#ifndef ICANVAS_H
#define ICANVAS_H

#include "../point/Point.h"
#include "color/Color.h"

#include <vector>

class ICanvas {
public:
	virtual ~ICanvas() = default;

	virtual void DrawLine(Point from, Point to, Color lineColor) = 0;
	virtual void DrawPolygon(std::vector<Point> points, Color lineColor) = 0;
	virtual void FillPolygon(std::vector<Point> points, Color fillColor) = 0;
	virtual void DrawCircle(Point center, double radius, Color lineColor) = 0;
	virtual void FillCircle(Point center, double radius, Color fillColor) = 0;
};


#endif //ICANVAS_H
