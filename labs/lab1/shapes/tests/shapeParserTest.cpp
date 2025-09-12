#include <gtest/gtest.h>

#include "../src/Command/ShapeParser/CircleParser.h"
#include "../src/Command/ShapeParser/LineParser.h"
#include "../src/Command/ShapeParser/RectangleParser.h"
#include "../src/Command/ShapeParser/TextParser.h"
#include "../src/Command/ShapeParser/TriangleParser.h"

namespace commands
{

TEST(RectangleParserTest, ParseValidRectangle)
{
	RectangleParser parser;
	auto model = parser.Parse("rectangle 10 20 30 40");

	auto bounds = model->GetBounds();
	EXPECT_DOUBLE_EQ(bounds.position.x, 10);
	EXPECT_DOUBLE_EQ(bounds.position.y, 20);
	EXPECT_DOUBLE_EQ(bounds.size.w, 30);
	EXPECT_DOUBLE_EQ(bounds.size.h, 40);
}

TEST(RectangleParserTest, ParseInvalidRectangle)
{
	RectangleParser parser;
	EXPECT_THROW(parser.Parse("rectangle 10 20 30"), std::runtime_error);
	EXPECT_THROW(parser.Parse("rectangle 10 20 30 40 50"), std::runtime_error);
	EXPECT_THROW(parser.Parse("invalid 10 20 30 40"), std::runtime_error);
}

TEST(CircleParserTest, ParseValidCircle)
{
	CircleParser parser;
	auto model = parser.Parse("circle 50 60 25");

	auto bounds = model->GetBounds();
	EXPECT_DOUBLE_EQ(bounds.position.x, 25);
	EXPECT_DOUBLE_EQ(bounds.position.y, 35);
	EXPECT_DOUBLE_EQ(bounds.size.w, 50);
	EXPECT_DOUBLE_EQ(bounds.size.h, 50);
}

TEST(CircleParserTest, ParseInvalidCircle)
{
	CircleParser parser;
	EXPECT_THROW(parser.Parse("circle 50 60"), std::runtime_error);
	EXPECT_THROW(parser.Parse("circle 50 60 25 30"), std::runtime_error);
	EXPECT_THROW(parser.Parse("invalid 50 60 25"), std::runtime_error);
}

TEST(TextParserTest, ParseValidText)
{
	TextParser parser;
	auto model = parser.Parse("text 100 200 24 Hello World");

	auto bounds = model->GetBounds();
	EXPECT_DOUBLE_EQ(bounds.position.x, 100);
	EXPECT_DOUBLE_EQ(bounds.position.y, 200);
	EXPECT_DOUBLE_EQ(bounds.size.h, 24);
}

TEST(TextParserTest, ParseInvalidText)
{
	TextParser parser;
	EXPECT_THROW(parser.Parse("text 100 200"), std::runtime_error);
	EXPECT_THROW(parser.Parse("text 100"), std::runtime_error);
	EXPECT_THROW(parser.Parse("invalid 100 200 24 Text"), std::runtime_error);
}

TEST(LineParserTest, ParseValidLine)
{
	LineParser parser;
	auto model = parser.Parse("line 0 0 100 100");

	auto bounds = model->GetBounds();
	EXPECT_DOUBLE_EQ(bounds.position.x, 0);
	EXPECT_DOUBLE_EQ(bounds.position.y, 0);
	EXPECT_DOUBLE_EQ(bounds.size.w, 100);
	EXPECT_DOUBLE_EQ(bounds.size.h, 100);
}

TEST(LineParserTest, ParseInvalidLine)
{
	LineParser parser;
	EXPECT_THROW(parser.Parse("line 0 0 100"), std::runtime_error);
	EXPECT_THROW(parser.Parse("line 0 0"), std::runtime_error);
	EXPECT_THROW(parser.Parse("invalid 0 0 100 100"), std::runtime_error);
}

TEST(TriangleParserTest, ParseValidTriangle)
{
	TriangleParser parser;
	auto model = parser.Parse("triangle 0 0 50 0 25 50");

	auto bounds = model->GetBounds();
	EXPECT_DOUBLE_EQ(bounds.position.x, 0);
	EXPECT_DOUBLE_EQ(bounds.position.y, 0);
	EXPECT_DOUBLE_EQ(bounds.size.w, 50);
	EXPECT_DOUBLE_EQ(bounds.size.h, 50);
}

TEST(TriangleParserTest, ParseInvalidTriangle)
{
	TriangleParser parser;
	EXPECT_THROW(parser.Parse("triangle 0 0 50 0 25"), std::runtime_error);
	EXPECT_THROW(parser.Parse("triangle 0 0 50"), std::runtime_error);
	EXPECT_THROW(parser.Parse("invalid 0 0 50 0 25 50"), std::runtime_error);
}

} // namespace commands
