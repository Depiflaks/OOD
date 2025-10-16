//
// Created by smmm on 12.05.2025.
//
#include <canvas/mock/MockCanvas.h>
#include <color/Color.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <point/Point.h>
#include <shape/lineSegment/LineSegment.h>
#include <shape/solidShape/circle/Circle.h>
#include <shape/solidShape/rectangle/Rectangle.h>
#include <shape/solidShape/triangle/Triangle.h>

using ::testing::_;
using ::testing::Eq;
using ::testing::ElementsAre;
using ::testing::Pointwise;

TEST(TriangleDrawTest, DrawCallsCanvasWithCorrectArguments)
{
    const Point v1{0.0, 0.0};
    const Point v2{1.0, 0.0};
    const Point v3{0.5, 1.0};
    const Color outlineColor{0xFF0000FF}; // красный
    const Color fillColor{0x00FF00FF};    // зелёный

    Triangle triangle(v1, v2, v3, outlineColor, fillColor);
    MockCanvas canvas;

    EXPECT_CALL(canvas, DrawPolygon(ElementsAre(v1, v2, v3), Eq(outlineColor))).Times(1);

    EXPECT_CALL(canvas, FillPolygon(ElementsAre(v1, v2, v3), Eq(fillColor))).Times(1);

    triangle.Draw(canvas);
}

TEST(RectangleDrawTest, DrawCallsCanvasWithCorrectArguments)
{
    const Point topLeft{0.0, 0.0};
    const double width = 2.0;
    const double height = 1.0;
    const Color outlineColor{0x112233FF};
    const Color fillColor{0x445566FF};

    Rectangle rect(topLeft, width, height, outlineColor, fillColor);
    MockCanvas canvas;

    std::vector<Point> expectedPoints = {
        topLeft,
        { topLeft.x + width, topLeft.y },
        { topLeft.x + width, topLeft.y + height },
        { topLeft.x, topLeft.y + height }
    };

    EXPECT_CALL(canvas, DrawPolygon(testing::ElementsAreArray(expectedPoints), Eq(outlineColor))).Times(1);
    EXPECT_CALL(canvas, FillPolygon(testing::ElementsAreArray(expectedPoints), Eq(fillColor))).Times(1);

    rect.Draw(canvas);
}

TEST(CircleDrawTest, DrawCallsCanvasWithCorrectArguments)
{
    const Point center{1.0, 1.0};
    const double radius = 0.5;
    const Color outlineColor{0xABCDEF11};
    const Color fillColor{0x123456FF};

    Circle circle(center, radius, outlineColor, fillColor);
    MockCanvas canvas;

    EXPECT_CALL(canvas, DrawCircle(center, radius, outlineColor)).Times(1);
    EXPECT_CALL(canvas, FillCircle(center, radius, fillColor)).Times(1);

    circle.Draw(canvas);
}

TEST(LineSegmentDrawTest, DrawCallsCanvasWithCorrectArguments)
{
    const Point start{0.0, 0.0};
    const Point end{2.0, 2.0};
    const Color outlineColor{0xFF00FF00};

    LineSegment line(start, end, outlineColor);
    MockCanvas canvas;

    EXPECT_CALL(canvas, DrawLine(start, end, outlineColor)).Times(1);

    line.Draw(canvas);
}