//
// Created by smmm on 12.05.2025.
//

#ifndef MOCKCANVAS_H
#define MOCKCANVAS_H

#include "../ICanvas.h"
#include <gmock/gmock.h>

class MockCanvas final : public ICanvas
{
public:
	MOCK_METHOD(void, DrawLine, (Point from, Point to, Color lineColor), (override));
	MOCK_METHOD(void, DrawPolygon, (std::vector<Point> points, Color lineColor), (override));
	MOCK_METHOD(void, FillPolygon, (std::vector<Point> points, Color fillColor), (override));
	MOCK_METHOD(void, DrawCircle, (Point center, double radius, Color lineColor), (override));
	MOCK_METHOD(void, FillCircle, (Point center, double radius, Color fillColor), (override));
};

#endif // MOCKCANVAS_H
