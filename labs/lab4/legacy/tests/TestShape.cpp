//
// Created by smmm on 12.05.2025.
//

#include <gtest/gtest.h>
#include <shape/lineSegment/LineSegment.h>
#include <shape/solidShape/circle/Circle.h>
#include <shape/solidShape/rectangle/Rectangle.h>
#include <shape/solidShape/triangle/Triangle.h>

// Triangle
TEST(TriangleTest, DefaultConstructor)
{
    Triangle t;

    EXPECT_EQ(t.GetVertex1(), Point{});
    EXPECT_EQ(t.GetVertex2(), Point{});
    EXPECT_EQ(t.GetVertex3(), Point{});

    EXPECT_EQ(t.GetOutlineColor(), k_black);
    EXPECT_EQ(t.GetFillColor(), k_empty);

    EXPECT_DOUBLE_EQ(t.GetArea(), 0.0);
    EXPECT_DOUBLE_EQ(t.GetPerimeter(), 0.0);
}

TEST(TriangleTest, PointConstructor)
{
    Point a{0, 0}, b{3, 0}, c{0, 4};
    Triangle t(a, b, c);

    EXPECT_EQ(t.GetVertex1(), a);
    EXPECT_EQ(t.GetVertex2(), b);
    EXPECT_EQ(t.GetVertex3(), c);

    EXPECT_EQ(t.GetOutlineColor(), k_black);
    EXPECT_EQ(t.GetFillColor(), k_empty);

    EXPECT_DOUBLE_EQ(t.GetArea(), 6.0);
    EXPECT_DOUBLE_EQ(t.GetPerimeter(), 12.0);
}

TEST(TriangleTest, OutlineColorConstructor)
{
    Point a{1, 1}, b{4, 1}, c{1, 5};
    Color outline(0x112233FF);
    Triangle t(a, b, c, outline);

    EXPECT_EQ(t.GetVertex1(), a);
    EXPECT_EQ(t.GetVertex2(), b);
    EXPECT_EQ(t.GetVertex3(), c);

    EXPECT_EQ(t.GetOutlineColor(), outline);
    EXPECT_EQ(t.GetFillColor(), k_empty);

    EXPECT_DOUBLE_EQ(t.GetArea(), 6.0);
    EXPECT_DOUBLE_EQ(t.GetPerimeter(), 12.0);
}

TEST(TriangleTest, OutlineAndFillColorConstructor)
{
    constexpr Point a{2, 2}, b{5, 2}, c{2, 6};
    Color outline(0xAABBCCDD);
    Color fill(0xEEFF0011);
    Triangle t(a, b, c, outline, fill);

    EXPECT_EQ(t.GetVertex1(), a);
    EXPECT_EQ(t.GetVertex2(), b);
    EXPECT_EQ(t.GetVertex3(), c);

    EXPECT_EQ(t.GetOutlineColor(), outline);
    EXPECT_EQ(t.GetFillColor(), fill);

    EXPECT_DOUBLE_EQ(t.GetArea(), 6.0);
    EXPECT_DOUBLE_EQ(t.GetPerimeter(), 12.0);
}

TEST(TriangleTest, FloatingPointPrecision)
{
    Point v1{0.1, 0.1};
    Point v2{1.1, 0.1};
    Point v3{0.1, 1.1};
    Triangle t(v1, v2, v3);

    EXPECT_NEAR(t.GetArea(), 0.5, 1e-9);
    EXPECT_NEAR(t.GetPerimeter(), 3.414213562, 1e-9);
}

TEST(RectangleTest, DefaultConstructor)
{
    Rectangle r;

    EXPECT_EQ(r.GetLeftTop(), Point{});
    EXPECT_DOUBLE_EQ(r.GetWidth(), 0.0);
    EXPECT_DOUBLE_EQ(r.GetHeight(), 0.0);

    EXPECT_EQ(r.GetRightBottom(), Point{});

    EXPECT_EQ(r.GetOutlineColor(), k_black);
    EXPECT_EQ(r.GetFillColor(), k_empty);

    EXPECT_DOUBLE_EQ(r.GetArea(), 0.0);
    EXPECT_DOUBLE_EQ(r.GetPerimeter(), 0.0);
}

TEST(RectangleTest, ConstructorWithParams)
{
    Point lt{1, 2};
    Rectangle r(lt, 3, 4);

    EXPECT_EQ(r.GetLeftTop(), lt);
    EXPECT_DOUBLE_EQ(r.GetWidth(), 3.0);
    EXPECT_DOUBLE_EQ(r.GetHeight(), 4.0);

    EXPECT_EQ(r.GetRightBottom(), Point(4.0, 6.0));

    EXPECT_EQ(r.GetOutlineColor(), k_black);
    EXPECT_EQ(r.GetFillColor(), k_empty);

    EXPECT_DOUBLE_EQ(r.GetArea(), 12.0);
    EXPECT_DOUBLE_EQ(r.GetPerimeter(), 14.0);
}

TEST(RectangleTest, ConstructorWithOutlineColor)
{
    Point lt{5, 5};
    Color outline(0x12345678);
    Rectangle r(lt, 2, 3, outline);

    EXPECT_EQ(r.GetLeftTop(), lt);
    EXPECT_DOUBLE_EQ(r.GetWidth(), 2.0);
    EXPECT_DOUBLE_EQ(r.GetHeight(), 3.0);

    EXPECT_EQ(r.GetRightBottom(), Point(7.0, 8.0));

    EXPECT_EQ(r.GetOutlineColor(), outline);
    EXPECT_EQ(r.GetFillColor(), k_empty);

    EXPECT_DOUBLE_EQ(r.GetArea(), 6.0);
    EXPECT_DOUBLE_EQ(r.GetPerimeter(), 10.0);
}

TEST(RectangleTest, ConstructorWithOutlineAndFillColor)
{
    Point lt{0, 0};
    Color outline(0xAABBCCDD);
    Color fill(0x11223344);
    Rectangle r(lt, 4, 5, outline, fill);

    EXPECT_EQ(r.GetLeftTop(), lt);
    EXPECT_DOUBLE_EQ(r.GetWidth(), 4.0);
    EXPECT_DOUBLE_EQ(r.GetHeight(), 5.0);

    EXPECT_EQ(r.GetRightBottom(), Point(4.0, 5.0));

    EXPECT_EQ(r.GetOutlineColor(), outline);
    EXPECT_EQ(r.GetFillColor(), fill);

    EXPECT_DOUBLE_EQ(r.GetArea(), 20.0);
    EXPECT_DOUBLE_EQ(r.GetPerimeter(), 18.0);
}

TEST(RectangleTest, FloatingPointPrecision)
{
    Point topLeft{2.5, 3.5};
    double width = 4.25;
    double height = 5.75;
    Rectangle r(topLeft, width, height);

    EXPECT_NEAR(r.GetArea(), 24.4375, 1e-9);
    EXPECT_NEAR(r.GetPerimeter(), 20.0, 1e-9);
}

// Circle
TEST(CircleTest, DefaultConstructor)
{
    Circle c;
    EXPECT_EQ(c.GetCenter(), Point{});
    EXPECT_DOUBLE_EQ(c.GetRadius(), 0.0);
}

TEST(CircleTest, ConstructorWithCenterAndRadius)
{
    Point center{1.0, 2.0};
    double radius = 5.0;
    Circle c(center, radius);
    EXPECT_EQ(c.GetCenter(), center);
    EXPECT_DOUBLE_EQ(c.GetRadius(), radius);
}

TEST(CircleTest, ConstructorWithOutlineColor)
{
    Point center{1.0, 2.0};
    double radius = 5.0;
    Color outlineColor{0xAABBCCDD};
    Circle c(center, radius, outlineColor);
    EXPECT_EQ(c.GetCenter(), center);
    EXPECT_DOUBLE_EQ(c.GetRadius(), radius);
    EXPECT_EQ(c.GetOutlineColor(), outlineColor);
}

TEST(CircleTest, ConstructorWithOutlineAndFillColor)
{
    Point center{1.0, 2.0};
    double radius = 5.0;
    Color outlineColor{0xAABBCCDD};
    Color fillColor{0x11223344};
    Circle c(center, radius, outlineColor, fillColor);
    EXPECT_EQ(c.GetCenter(), center);
    EXPECT_DOUBLE_EQ(c.GetRadius(), radius);
    EXPECT_EQ(c.GetOutlineColor(), outlineColor);
    EXPECT_EQ(c.GetFillColor(), fillColor);
}

TEST(CircleTest, FloatingPointPrecision)
{
    Point center{0.5, 0.5};
    double radius = 2.75;
    Circle c(center, radius);

    double expectedArea = M_PI * radius * radius;
    double expectedPerimeter = 2 * M_PI * radius;

    EXPECT_NEAR(c.GetArea(), expectedArea, 1e-9);
    EXPECT_NEAR(c.GetPerimeter(), expectedPerimeter, 1e-9);
}

// Line
TEST(LineSegmentTest, DefaultConstructor)
{
    LineSegment line;
    EXPECT_EQ(line.GetStartPoint(), Point{});
    EXPECT_EQ(line.GetEndPoint(), Point{});
}

TEST(LineSegmentTest, ConstructorWithPoints)
{
    Point start{1.0, 1.0};
    Point end{4.0, 5.0};
    LineSegment line(start, end);
    EXPECT_EQ(line.GetStartPoint(), start);
    EXPECT_EQ(line.GetEndPoint(), end);
}

TEST(LineSegmentTest, ConstructorWithOutlineColor)
{
    Point start{1.0, 1.0};
    Point end{4.0, 5.0};
    Color outline{0xAABBCCDD};
    LineSegment line(start, end, outline);
    EXPECT_EQ(line.GetStartPoint(), start);
    EXPECT_EQ(line.GetEndPoint(), end);
    EXPECT_EQ(line.GetOutlineColor(), outline);
}

TEST(LineSegmentTest, FloatingPointPrecision)
{
    Point start{0.1, 0.1};
    Point end{3.2, 4.5};
    LineSegment line(start, end);

    double dx = end.x - start.x;
    double dy = end.y - start.y;
    double expectedLength = std::sqrt(dx * dx + dy * dy);

    EXPECT_DOUBLE_EQ(line.GetArea(), 0.0);
    EXPECT_NEAR(line.GetPerimeter(), expectedLength, 1e-9);
}

TEST(ShapeUtilsTest, GetShapeWithMaxArea)
{
    std::vector<std::shared_ptr<Shape>> shapes;

    shapes.push_back(std::make_shared<Rectangle>(Point{0, 0}, 2.0, 3.0));
    shapes.push_back(std::make_shared<Triangle>(Point{0, 0}, Point{1, 0}, Point{0, 2}));
    shapes.push_back(std::make_shared<Circle>(Point{0, 0}, 1.5));

    auto maxAreaShape = GetShapeWithMaxArea(shapes);
    ASSERT_NE(maxAreaShape, nullptr);
    EXPECT_NEAR(maxAreaShape->GetArea(), 7.06858, 1e-5);
}

TEST(ShapeUtilsTest, GetShapeWithMaxPerimeter)
{
    std::vector<std::shared_ptr<Shape>> shapes;

    shapes.push_back(std::make_shared<Rectangle>(Point{0, 0}, 2.0, 3.0));
    shapes.push_back(std::make_shared<Triangle>(Point{0, 0}, Point{3, 0}, Point{0, 4}));
    shapes.push_back(std::make_shared<Circle>(Point{0, 0}, 1.5));

    auto maxPerimeterShape = GetShapeWithMaxPerimeter(shapes);
    ASSERT_NE(maxPerimeterShape, nullptr);
    EXPECT_NEAR(maxPerimeterShape->GetPerimeter(), 12.0, 1e-5);
}
