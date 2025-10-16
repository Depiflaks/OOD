#include "../src/lib/Geometry.h"
#include "../src/lib/Shape.h"
#include "lib/ShapeFactory.h"

#include <gmock/gmock.h>

#include <gtest/gtest.h>

TEST(ShapeFactoryTest, CreateTriangleSuccess)
{
	ShapeFactory factory;
	auto shape = factory.CreateShape("triangle 0 0 10 0 5 10 #ff0000ff");

	auto* triangle = dynamic_cast<Triangle*>(shape.get());
	ASSERT_NE(triangle, nullptr);
	EXPECT_EQ(triangle->GetVertex1().x, 0);
	EXPECT_EQ(triangle->GetVertex1().y, 0);
	EXPECT_EQ(triangle->GetVertex2().x, 10);
	EXPECT_EQ(triangle->GetVertex2().y, 0);
	EXPECT_EQ(triangle->GetVertex3().x, 5);
	EXPECT_EQ(triangle->GetVertex3().y, 10);
	EXPECT_EQ(triangle->GetColor().ToHexString(), "#ff0000ff");
}

TEST(ShapeFactoryTest, CreateTriangleInvalidFormat)
{
	ShapeFactory factory;
	EXPECT_THROW(factory.CreateShape("triangle 0 0 10 0 5"), std::invalid_argument);
}

TEST(ShapeFactoryTest, CreateRectangleSuccess)
{
	ShapeFactory factory;
	auto shape = factory.CreateShape("rectangle 0 0 10 10 #00ff00ff");

	auto* rect = dynamic_cast<Rectangle*>(shape.get());
	ASSERT_NE(rect, nullptr);
	EXPECT_EQ(rect->GetLeftTop().x, 0);
	EXPECT_EQ(rect->GetLeftTop().y, 0);
	EXPECT_EQ(rect->GetRightBottom().x, 10);
	EXPECT_EQ(rect->GetRightBottom().y, 10);
	EXPECT_EQ(rect->GetColor().ToHexString(), "#00ff00ff");
}

TEST(ShapeFactoryTest, CreateRectangleInvalidFormat)
{
	ShapeFactory factory;
	EXPECT_THROW(factory.CreateShape("rectangle 0 0 10"), std::invalid_argument);
}

TEST(ShapeFactoryTest, CreateEllipseSuccess)
{
	ShapeFactory factory;
	auto shape = factory.CreateShape("ellipse 5 5 3 2 #0000ffff");

	auto* ellipse = dynamic_cast<Ellipse*>(shape.get());
	ASSERT_NE(ellipse, nullptr);
	EXPECT_EQ(ellipse->GetCenter().x, 5);
	EXPECT_EQ(ellipse->GetCenter().y, 5);
	EXPECT_EQ(ellipse->GetHorizontalRadius(), 3);
	EXPECT_EQ(ellipse->GetVerticalRadius(), 2);
	EXPECT_EQ(ellipse->GetColor().ToHexString(), "#0000ffff");
}

TEST(ShapeFactoryTest, CreateEllipseInvalidFormat)
{
	ShapeFactory factory;
	EXPECT_THROW(factory.CreateShape("ellipse 5 5 3"), std::invalid_argument);
}

TEST(ShapeFactoryTest, CreateRegularPolygonSuccess)
{
	ShapeFactory factory;
	auto shape = factory.CreateShape("polygon 7 7 4 6 #ffff00ff");

	auto* polygon = dynamic_cast<RegularPolygon*>(shape.get());
	ASSERT_NE(polygon, nullptr);
	EXPECT_EQ(polygon->GetCenter().x, 7);
	EXPECT_EQ(polygon->GetCenter().y, 7);
	EXPECT_EQ(polygon->GetRadius(), 4);
	EXPECT_EQ(polygon->GetVertexCount(), 6);
	EXPECT_EQ(polygon->GetColor().ToHexString(), "#ffff00ff");
}

TEST(ShapeFactoryTest, CreateRegularPolygonInvalidFormat)
{
	ShapeFactory factory;
	EXPECT_THROW(factory.CreateShape("polygon 7 7 4"), std::invalid_argument);
}

TEST(ShapeFactoryTest, UnknownShapeType)
{
	ShapeFactory factory;
	EXPECT_THROW(factory.CreateShape("circle 5 5 3 2 #ff0000ff"), std::invalid_argument);
}
