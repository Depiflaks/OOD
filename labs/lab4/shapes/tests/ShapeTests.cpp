#include "../src/lib/Canvas.h"
#include "../src/lib/Geometry.h"
#include "../src/lib/Shape.h"

#include <gmock/gmock.h>

#include <gtest/gtest.h>

class MockCanvas : public ICanvas
{
public:
	MOCK_METHOD(void, MoveTo, (double x, double y), (override));
	MOCK_METHOD(void, SetColor, (const Color& color), (override));
	MOCK_METHOD(void, LineTo, (double x, double y), (override));
	MOCK_METHOD(void, DrawEllipse, (double cx, double cy, double rx, double ry), (override));
	MOCK_METHOD(void, DrawText, (double left, double top, double fontSize, const std::string& text), (override));
	MOCK_METHOD(void, Render, (), (override));
};

TEST(ShapeTest, RectangleDrawing)
{
	MockCanvas canvas;
	Point leftTop{ 0, 10 };
	Point rightBottom{ 20, 0 };
	Color color("#ff0000ff");
	Rectangle rect(leftTop, rightBottom, color);

	EXPECT_EQ(rect.GetLeftTop().x, 0);
	EXPECT_EQ(rect.GetLeftTop().y, 10);
	EXPECT_EQ(rect.GetRightBottom().x, 20);
	EXPECT_EQ(rect.GetRightBottom().y, 0);
	EXPECT_EQ(rect.GetColor().ToHexString(), "#ff0000ff");

	testing::InSequence seq;
	EXPECT_CALL(canvas, SetColor(color));
	EXPECT_CALL(canvas, MoveTo(0, 10));
	EXPECT_CALL(canvas, LineTo(20, 10));
	EXPECT_CALL(canvas, LineTo(20, 0));
	EXPECT_CALL(canvas, LineTo(0, 0));
	EXPECT_CALL(canvas, LineTo(0, 10));

	rect.Draw(canvas);
}

TEST(ShapeTest, TriangleDrawing)
{
	MockCanvas canvas;
	Point v1{ 0, 0 };
	Point v2{ 10, 20 };
	Point v3{ 20, 0 };
	Color color("#00ff00ff");
	Triangle triangle(v1, v2, v3, color);

	EXPECT_EQ(triangle.GetVertex1().x, 0);
	EXPECT_EQ(triangle.GetVertex1().y, 0);
	EXPECT_EQ(triangle.GetVertex2().x, 10);
	EXPECT_EQ(triangle.GetVertex2().y, 20);
	EXPECT_EQ(triangle.GetVertex3().x, 20);
	EXPECT_EQ(triangle.GetVertex3().y, 0);
	EXPECT_EQ(triangle.GetColor().ToHexString(), "#00ff00ff");

	testing::InSequence seq;
	EXPECT_CALL(canvas, SetColor(color));
	EXPECT_CALL(canvas, MoveTo(0, 0));
	EXPECT_CALL(canvas, LineTo(10, 20));
	EXPECT_CALL(canvas, LineTo(20, 0));
	EXPECT_CALL(canvas, LineTo(0, 0));

	triangle.Draw(canvas);
}

TEST(ShapeTest, EllipseDrawing)
{
	MockCanvas canvas;
	Point center{ 10, 10 };
	Color color("#0000ffff");
	Ellipse ellipse(center, 5.0, 3.0, color);

	EXPECT_EQ(ellipse.GetCenter().x, 10);
	EXPECT_EQ(ellipse.GetCenter().y, 10);
	EXPECT_EQ(ellipse.GetHorizontalRadius(), 5.0);
	EXPECT_EQ(ellipse.GetVerticalRadius(), 3.0);
	EXPECT_EQ(ellipse.GetColor().ToHexString(), "#0000ffff");

	testing::InSequence seq;
	EXPECT_CALL(canvas, SetColor(color));
	EXPECT_CALL(canvas, DrawEllipse(10, 10, 5.0, 3.0));

	ellipse.Draw(canvas);
}

TEST(ShapeTest, RegularPolygonDrawing)
{
	MockCanvas canvas;
	Point center{ 15, 15 };
	Color color("#ffff00ff");
	RegularPolygon polygon(center, 8.0, 5, color);

	EXPECT_EQ(polygon.GetCenter().x, 15);
	EXPECT_EQ(polygon.GetCenter().y, 15);
	EXPECT_EQ(polygon.GetRadius(), 8.0);
	EXPECT_EQ(polygon.GetVertexCount(), 5);
	EXPECT_EQ(polygon.GetColor().ToHexString(), "#ffff00ff");

	EXPECT_CALL(canvas, SetColor(color));
	EXPECT_CALL(canvas, MoveTo(testing::_, testing::_)).Times(1);
	EXPECT_CALL(canvas, LineTo(testing::_, testing::_)).Times(5);

	polygon.Draw(canvas);
}
