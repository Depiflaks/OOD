#ifndef MOCKCANVAS_H
#define MOCKCANVAS_H

#include "../src/lib/Geometry.h"
#include "../src/lib/Shape.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using MockPoint = std::pair<double, double>;
class MockCanvas : public ICanvas
{
public:
	MOCK_METHOD(void, MoveTo, (double x, double y), (override));
	MOCK_METHOD(void, SetColor, (const Color& color), (override));
	MOCK_METHOD(void, LineTo, (double x, double y), (override));
	MOCK_METHOD(void, DrawEllipse, (double cx, double cy, double rx, double ry), (override));
	MOCK_METHOD(void, DrawFilledRectangle, (double x1, double y1, double x2, double y2), (override));
	MOCK_METHOD(void, DrawFilledTriangle, (double x1, double y1, double x2, double y2, double x3, double y3), (override));
	MOCK_METHOD(void, DrawFilledRegularPolygon, (const std::vector<MockPoint>& points), (override));
	MOCK_METHOD(void, DrawText, (double left, double top, double fontSize, const std::string& text), (override));
	MOCK_METHOD(void, Render, (), (override));
};

#endif /* MOCKCANVAS_H */
