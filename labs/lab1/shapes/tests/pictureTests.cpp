#include "../src/Shapes/Geometry.h"
#include "../src/Shapes/Models/CircleModel.h"
#include "../src/Shapes/Models/RectangleModel.h"
#include "../src/Shapes/Picture.h"

#include "./MockCanvas.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;

namespace shapes
{
TEST(PictureTest, AddShapeUpdatesBounds)
{
	gfx::MockCanvas canvas;
	Picture picture(canvas);

	auto model = std::make_unique<RectangleModel>(Bounds{ { 10, 20 }, { 30, 40 } });
	picture.AddShape("rect1", Color("#ff0000ff"), std::move(model));

	auto bounds = picture.GetBounds();
	EXPECT_DOUBLE_EQ(bounds.position.x, 10);
	EXPECT_DOUBLE_EQ(bounds.position.y, 20);
	EXPECT_DOUBLE_EQ(bounds.size.w, 30);
	EXPECT_DOUBLE_EQ(bounds.size.h, 40);
}

TEST(PictureTest, DrawPictureCallsCanvasMethods)
{
	gfx::MockCanvas canvas;
	Picture picture(canvas);

	auto rectModel = std::make_unique<RectangleModel>(Bounds{ { 0, 0 }, { 10, 10 } });
	auto circleModel = std::make_unique<CircleModel>(Position{ 3, 3 }, 15);
	picture.AddShape("rect1", Color("#ff0000ff"), std::move(rectModel));
	picture.AddShape("circle1", Color("#ffaa00ff"), std::move(circleModel));

	EXPECT_CALL(canvas, SetColor(_)).Times(2);
	EXPECT_CALL(canvas, MoveTo(0, 0));
	EXPECT_CALL(canvas, LineTo(10, 0));
	EXPECT_CALL(canvas, LineTo(10, 10));
	EXPECT_CALL(canvas, LineTo(0, 10));
	EXPECT_CALL(canvas, LineTo(0, 0));
	EXPECT_CALL(canvas, DrawEllipse(3, 3, 15, 15));

	picture.DrawPicture();
}

TEST(PictureTest, DrawShapeCallsCanvasMethods)
{
	gfx::MockCanvas canvas;
	Picture picture(canvas);

	auto rectModel = std::make_unique<RectangleModel>(Bounds{ { 0, 0 }, { 10, 10 } });
	picture.AddShape("rect1", Color("#ff0000ff"), std::move(rectModel));

	EXPECT_CALL(canvas, SetColor(_)).Times(1);
	EXPECT_CALL(canvas, MoveTo(0, 0));
	EXPECT_CALL(canvas, LineTo(10, 0));
	EXPECT_CALL(canvas, LineTo(10, 10));
	EXPECT_CALL(canvas, LineTo(0, 10));
	EXPECT_CALL(canvas, LineTo(0, 0));

	picture.DrawShape("rect1");
}

TEST(PictureTest, ChangeShapeUpdatesBounds)
{
	gfx::MockCanvas canvas;
	Picture picture(canvas);

	auto rectModel = std::make_unique<RectangleModel>(Bounds{ { 0, 0 }, { 10, 10 } });
	picture.AddShape("shape1", Color("#ff0000ff"), std::move(rectModel));

	auto circleModel = std::make_unique<CircleModel>(Position{ 50, 50 }, 25);
	picture.ChangeShape("shape1", std::move(circleModel));

	auto bounds = picture.GetBounds();
	EXPECT_DOUBLE_EQ(bounds.position.x, 25);
	EXPECT_DOUBLE_EQ(bounds.position.y, 25);
	EXPECT_DOUBLE_EQ(bounds.size.w, 50);
	EXPECT_DOUBLE_EQ(bounds.size.h, 50);
}

TEST(PictureTest, DeleteShapeUpdatesBounds)
{
	gfx::MockCanvas canvas;
	Picture picture(canvas);

	auto rectModel = std::make_unique<RectangleModel>(Bounds{ { 0, 0 }, { 10, 10 } });
	picture.AddShape("rect1", Color("#ff0000ff"), std::move(rectModel));

	picture.DeleteShape("rect1");
	auto bounds = picture.GetBounds();
	EXPECT_DOUBLE_EQ(bounds.position.x, 0);
	EXPECT_DOUBLE_EQ(bounds.position.y, 0);
	EXPECT_DOUBLE_EQ(bounds.size.w, 0);
	EXPECT_DOUBLE_EQ(bounds.size.h, 0);
}

TEST(PictureTest, MovePictureUpdatesBounds)
{
	gfx::MockCanvas canvas;
	Picture picture(canvas);

	auto rectModel = std::make_unique<RectangleModel>(Bounds{ { 0, 0 }, { 10, 10 } });
	picture.AddShape("rect1", Color("#ff0000ff"), std::move(rectModel));

	picture.MovePicture(5, 10);
	auto bounds = picture.GetBounds();
	EXPECT_DOUBLE_EQ(bounds.position.x, 5);
	EXPECT_DOUBLE_EQ(bounds.position.y, 10);
	EXPECT_DOUBLE_EQ(bounds.size.w, 10);
	EXPECT_DOUBLE_EQ(bounds.size.h, 10);
}

TEST(PictureTest, MoveShapeUpdatesBounds)
{
	gfx::MockCanvas canvas;
	Picture picture(canvas);

	auto rectModel = std::make_unique<RectangleModel>(Bounds{ { 0, 0 }, { 10, 10 } });
	picture.AddShape("rect1", Color("#ff0000ff"), std::move(rectModel));

	picture.MoveShape("rect1", 15, 20);
	auto bounds = picture.GetBounds();
	EXPECT_DOUBLE_EQ(bounds.position.x, 15);
	EXPECT_DOUBLE_EQ(bounds.position.y, 20);
	EXPECT_DOUBLE_EQ(bounds.size.w, 10);
	EXPECT_DOUBLE_EQ(bounds.size.h, 10);
}

TEST(PictureTest, MultipleShapesBounds)
{
	gfx::MockCanvas canvas;
	Picture picture(canvas);

	auto rect1 = std::make_unique<RectangleModel>(Bounds{ { 0, 0 }, { 10, 10 } });
	auto rect2 = std::make_unique<RectangleModel>(Bounds{ { 20, 30 }, { 15, 25 } });

	picture.AddShape("rect1", Color("#ff0000ff"), std::move(rect1));
	picture.AddShape("rect2", Color("#00ff00ff"), std::move(rect2));

	auto bounds = picture.GetBounds();
	EXPECT_DOUBLE_EQ(bounds.position.x, 0);
	EXPECT_DOUBLE_EQ(bounds.position.y, 0);
	EXPECT_DOUBLE_EQ(bounds.size.w, 35);
	EXPECT_DOUBLE_EQ(bounds.size.h, 55);
}
// TODO: допилить тесты для List
} // namespace shapes
