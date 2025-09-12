#include "./MockCanvas.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../src/Command/Commands/Commands.h"
#include "../src/Shapes/Geometry.h"
#include "../src/Shapes/Picture.h"

using ::testing::_;

namespace commands
{

TEST(PictureCommandsTest, AddShapeCommandValid)
{
	gfx::MockCanvas canvas;
	shapes::Picture picture(canvas);
	AddShapeCommand cmd;

	EXPECT_NO_THROW(cmd.Execute(picture, "rect1 #ff0000ff rectangle 10 20 30 40"));

	auto bounds = picture.GetBounds();
	EXPECT_DOUBLE_EQ(bounds.position.x, 10);
	EXPECT_DOUBLE_EQ(bounds.position.y, 20);
	EXPECT_DOUBLE_EQ(bounds.size.w, 30);
	EXPECT_DOUBLE_EQ(bounds.size.h, 40);
}

TEST(PictureCommandsTest, AddShapeCommandInvalid)
{
	gfx::MockCanvas canvas;
	shapes::Picture picture(canvas);
	AddShapeCommand cmd;

	EXPECT_THROW(cmd.Execute(picture, "rect1 #ff0000ff rectangle 10 20"), std::runtime_error);
	EXPECT_THROW(cmd.Execute(picture, "rect1"), std::runtime_error);
	EXPECT_THROW(cmd.Execute(picture, ""), std::runtime_error);
}

TEST(PictureCommandsTest, DrawPictureCommandValid)
{
	gfx::MockCanvas canvas;
	shapes::Picture picture(canvas);
	AddShapeCommand addCmd;
	DrawPictureCommand drawCmd;

	addCmd.Execute(picture, "rect1 #ff0000ff rectangle 0 0 10 10");

	EXPECT_CALL(canvas, SetColor(_)).Times(1);
	EXPECT_CALL(canvas, MoveTo(0, 0));
	EXPECT_CALL(canvas, LineTo(10, 0));
	EXPECT_CALL(canvas, LineTo(10, 10));
	EXPECT_CALL(canvas, LineTo(0, 10));
	EXPECT_CALL(canvas, LineTo(0, 0));

	EXPECT_NO_THROW(drawCmd.Execute(picture, ""));
}

TEST(PictureCommandsTest, DrawPictureCommandInvalid)
{
	gfx::MockCanvas canvas;
	shapes::Picture picture(canvas);
	DrawPictureCommand cmd;

	EXPECT_THROW(cmd.Execute(picture, "extra"), std::runtime_error);
}

TEST(PictureCommandsTest, DrawShapeCommandValid)
{
	gfx::MockCanvas canvas;
	shapes::Picture picture(canvas);
	AddShapeCommand addCmd;
	DrawShapeCommand drawCmd;

	addCmd.Execute(picture, "rect1 #ff0000ff rectangle 0 0 10 10");

	EXPECT_CALL(canvas, SetColor(_)).Times(1);
	EXPECT_CALL(canvas, MoveTo(0, 0));
	EXPECT_CALL(canvas, LineTo(10, 0));
	EXPECT_CALL(canvas, LineTo(10, 10));
	EXPECT_CALL(canvas, LineTo(0, 10));
	EXPECT_CALL(canvas, LineTo(0, 0));

	EXPECT_NO_THROW(drawCmd.Execute(picture, "rect1"));
}

TEST(PictureCommandsTest, DrawShapeCommandInvalid)
{
	gfx::MockCanvas canvas;
	shapes::Picture picture(canvas);
	DrawShapeCommand cmd;

	EXPECT_THROW(cmd.Execute(picture, ""), std::runtime_error);
	EXPECT_THROW(cmd.Execute(picture, "id1 id2"), std::runtime_error);
}

TEST(PictureCommandsTest, ChangeShapeCommandValid)
{
	gfx::MockCanvas canvas;
	shapes::Picture picture(canvas);
	AddShapeCommand addCmd;
	ChangeShapeCommand changeCmd;

	addCmd.Execute(picture, "shape1 #ff0000ff rectangle 0 0 10 10");
	changeCmd.Execute(picture, "shape1 circle 50 50 25");

	auto bounds = picture.GetBounds();
	EXPECT_DOUBLE_EQ(bounds.position.x, 25);
	EXPECT_DOUBLE_EQ(bounds.position.y, 25);
	EXPECT_DOUBLE_EQ(bounds.size.w, 50);
	EXPECT_DOUBLE_EQ(bounds.size.h, 50);
}

TEST(PictureCommandsTest, ChangeShapeCommandInvalid)
{
	gfx::MockCanvas canvas;
	shapes::Picture picture(canvas);
	AddShapeCommand addCmd;
	ChangeShapeCommand cmd;

	addCmd.Execute(picture, "shape1 #ff0000ff rectangle 0 0 10 10");

	EXPECT_THROW(cmd.Execute(picture, "shape1 circle 50"), std::runtime_error);
	EXPECT_THROW(cmd.Execute(picture, "shape1"), std::runtime_error);
	EXPECT_THROW(cmd.Execute(picture, ""), std::runtime_error);
}

TEST(PictureCommandsTest, ChangeColorCommandValid)
{
	gfx::MockCanvas canvas;
	shapes::Picture picture(canvas);
	AddShapeCommand addCmd;
	ChangeColorCommand changeCmd;

	addCmd.Execute(picture, "rect1 #ff0000ff rectangle 0 0 10 10");
	EXPECT_NO_THROW(changeCmd.Execute(picture, "rect1 #00ff00ff"));
}

TEST(PictureCommandsTest, ChangeColorCommandInvalid)
{
	gfx::MockCanvas canvas;
	shapes::Picture picture(canvas);
	AddShapeCommand addCmd;
	ChangeColorCommand cmd;

	addCmd.Execute(picture, "rect1 #ff0000ff rectangle 0 0 10 10");

	EXPECT_THROW(cmd.Execute(picture, "rect1"), std::runtime_error);
	EXPECT_THROW(cmd.Execute(picture, ""), std::runtime_error);
}

TEST(PictureCommandsTest, ListCommandValid)
{
	gfx::MockCanvas canvas;
	shapes::Picture picture(canvas);
	AddShapeCommand addCmd;
	ListCommand listCmd;

	addCmd.Execute(picture, "rect1 #ff0000ff rectangle 0 0 10 10");
	EXPECT_NO_THROW(listCmd.Execute(picture, ""));
}

TEST(PictureCommandsTest, ListCommandInvalid)
{
	gfx::MockCanvas canvas;
	shapes::Picture picture(canvas);
	ListCommand cmd;

	EXPECT_THROW(cmd.Execute(picture, "extra"), std::runtime_error);
}

TEST(PictureCommandsTest, DeleteShapeCommandValid)
{
	gfx::MockCanvas canvas;
	shapes::Picture picture(canvas);
	AddShapeCommand addCmd;
	DeleteShapeCommand deleteCmd;

	addCmd.Execute(picture, "rect1 #ff0000ff rectangle 10 20 30 40");
	deleteCmd.Execute(picture, "rect1");

	auto bounds = picture.GetBounds();
	EXPECT_DOUBLE_EQ(bounds.position.x, 0);
	EXPECT_DOUBLE_EQ(bounds.position.y, 0);
	EXPECT_DOUBLE_EQ(bounds.size.w, 0);
	EXPECT_DOUBLE_EQ(bounds.size.h, 0);
}

TEST(PictureCommandsTest, DeleteShapeCommandInvalid)
{
	gfx::MockCanvas canvas;
	shapes::Picture picture(canvas);
	DeleteShapeCommand cmd;

	EXPECT_THROW(cmd.Execute(picture, ""), std::runtime_error);
	EXPECT_THROW(cmd.Execute(picture, "id1 id2"), std::runtime_error);
}

TEST(PictureCommandsTest, MovePictureCommandValid)
{
	gfx::MockCanvas canvas;
	shapes::Picture picture(canvas);
	AddShapeCommand addCmd;
	MovePictureCommand moveCmd;

	addCmd.Execute(picture, "rect1 #ff0000ff rectangle 0 0 10 10");
	moveCmd.Execute(picture, "5 10");

	auto bounds = picture.GetBounds();
	EXPECT_DOUBLE_EQ(bounds.position.x, 5);
	EXPECT_DOUBLE_EQ(bounds.position.y, 10);
	EXPECT_DOUBLE_EQ(bounds.size.w, 10);
	EXPECT_DOUBLE_EQ(bounds.size.h, 10);
}

TEST(PictureCommandsTest, MovePictureCommandInvalid)
{
	gfx::MockCanvas canvas;
	shapes::Picture picture(canvas);
	MovePictureCommand cmd;

	EXPECT_THROW(cmd.Execute(picture, "5"), std::runtime_error);
	EXPECT_THROW(cmd.Execute(picture, ""), std::runtime_error);
	EXPECT_THROW(cmd.Execute(picture, "5 10 15"), std::runtime_error);
}

TEST(PictureCommandsTest, MoveShapeCommandValid)
{
	gfx::MockCanvas canvas;
	shapes::Picture picture(canvas);
	AddShapeCommand addCmd;
	MoveShapeCommand moveCmd;

	addCmd.Execute(picture, "rect1 #ff0000ff rectangle 0 0 10 10");
	moveCmd.Execute(picture, "rect1 15 20");

	auto bounds = picture.GetBounds();
	EXPECT_DOUBLE_EQ(bounds.position.x, 15);
	EXPECT_DOUBLE_EQ(bounds.position.y, 20);
	EXPECT_DOUBLE_EQ(bounds.size.w, 10);
	EXPECT_DOUBLE_EQ(bounds.size.h, 10);
}

TEST(PictureCommandsTest, MoveShapeCommandInvalid)
{
	gfx::MockCanvas canvas;
	shapes::Picture picture(canvas);
	MoveShapeCommand cmd;

	EXPECT_THROW(cmd.Execute(picture, "rect1 15"), std::runtime_error);
	EXPECT_THROW(cmd.Execute(picture, "rect1"), std::runtime_error);
	EXPECT_THROW(cmd.Execute(picture, ""), std::runtime_error);
}
} // namespace commands
