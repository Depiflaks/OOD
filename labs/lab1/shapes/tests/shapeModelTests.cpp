#include "../src/Shapes/Geometry.h"
#include "../src/Shapes/Models/CircleModel.h"
#include "../src/Shapes/Models/LineModel.h"
#include "../src/Shapes/Models/RectangleModel.h"
#include "../src/Shapes/Models/TextModel.h"
#include "../src/Shapes/Models/TriangleModel.h"
#include "./MockCanvas.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;

TEST(ShapeModelsTest, RectangleModelDrawTest)
{
	gfx::MockCanvas canvas;
	Bounds bounds{ { 10, 20 }, { 30, 40 } };
	RectangleModel rectangle(bounds);
	Color color("#ff0000ff");

	EXPECT_CALL(canvas, SetColor(_));
	EXPECT_CALL(canvas, MoveTo(10, 20));
	EXPECT_CALL(canvas, LineTo(40, 20));
	EXPECT_CALL(canvas, LineTo(40, 60));
	EXPECT_CALL(canvas, LineTo(10, 60));
	EXPECT_CALL(canvas, LineTo(10, 20));

	rectangle.Draw(canvas, color);
}

TEST(ShapeModelsTest, TriangleModelDrawTest)
{
	gfx::MockCanvas canvas;
	TriangleModel triangle({ 0, 0 }, { 50, 0 }, { 25, 50 });
	Color color("#00ff00ff");

	EXPECT_CALL(canvas, SetColor(_));
	EXPECT_CALL(canvas, MoveTo(0, 0));
	EXPECT_CALL(canvas, LineTo(50, 0));
	EXPECT_CALL(canvas, LineTo(25, 50));
	EXPECT_CALL(canvas, LineTo(0, 0));

	triangle.Draw(canvas, color);
}

TEST(ShapeModelsTest, TextModelDrawTest)
{
	gfx::MockCanvas canvas;
	TextModel text({ 100, 200 }, 24, "Hello");
	Color color("#0000ffff");

	EXPECT_CALL(canvas, SetColor(_));
	EXPECT_CALL(canvas, DrawText(100, 200, 24, "Hello"));

	text.Draw(canvas, color);
}

TEST(ShapeModelsTest, LineModelDrawTest)
{
	gfx::MockCanvas canvas;
	LineModel line({ 0, 0 }, { 100, 100 });
	Color color("#ffff00ff");

	EXPECT_CALL(canvas, SetColor(_));
	EXPECT_CALL(canvas, MoveTo(0, 0));
	EXPECT_CALL(canvas, LineTo(100, 100));

	line.Draw(canvas, color);
}

TEST(ShapeModelsTest, CircleModelDrawTest)
{
	gfx::MockCanvas canvas;
	CircleModel circle({ 50, 50 }, 25);
	Color color("#ff00ffff");

	EXPECT_CALL(canvas, SetColor(_));
	EXPECT_CALL(canvas, DrawEllipse(50, 50, 25, 25));

	circle.Draw(canvas, color);
}

TEST(ShapeModelsTest, RectangleModelBoundsTest)
{
	Bounds bounds{ { 5, 10 }, { 15, 20 } };
	RectangleModel rectangle(bounds);

	auto result = rectangle.GetBounds();
	EXPECT_DOUBLE_EQ(result.position.x, 5);
	EXPECT_DOUBLE_EQ(result.position.y, 10);
	EXPECT_DOUBLE_EQ(result.size.w, 15);
	EXPECT_DOUBLE_EQ(result.size.h, 20);
}

TEST(ShapeModelsTest, TriangleModelBoundsTest)
{
	TriangleModel triangle({ 0, 10 }, { 30, 10 }, { 15, 40 });

	auto result = triangle.GetBounds();
	EXPECT_DOUBLE_EQ(result.position.x, 0);
	EXPECT_DOUBLE_EQ(result.position.y, 10);
	EXPECT_DOUBLE_EQ(result.size.w, 30);
	EXPECT_DOUBLE_EQ(result.size.h, 30);
}

TEST(ShapeModelsTest, TextModelBoundsTest)
{
	TextModel text({ 50, 60 }, 12, "Test");

	auto result = text.GetBounds();
	EXPECT_DOUBLE_EQ(result.position.x, 50);
	EXPECT_DOUBLE_EQ(result.position.y, 60);
	EXPECT_DOUBLE_EQ(result.size.w, 12 * 4 * 0.6);
	EXPECT_DOUBLE_EQ(result.size.h, 12);
}

TEST(ShapeModelsTest, LineModelBoundsTest)
{
	LineModel line({ 10, 20 }, { 30, 40 });

	auto result = line.GetBounds();
	EXPECT_DOUBLE_EQ(result.position.x, 10);
	EXPECT_DOUBLE_EQ(result.position.y, 20);
	EXPECT_DOUBLE_EQ(result.size.w, 20);
	EXPECT_DOUBLE_EQ(result.size.h, 20);
}

TEST(ShapeModelsTest, CircleModelBoundsTest)
{
	CircleModel circle({ 100, 100 }, 25);

	auto result = circle.GetBounds();
	EXPECT_DOUBLE_EQ(result.position.x, 75);
	EXPECT_DOUBLE_EQ(result.position.y, 75);
	EXPECT_DOUBLE_EQ(result.size.w, 50);
	EXPECT_DOUBLE_EQ(result.size.h, 50);
}
