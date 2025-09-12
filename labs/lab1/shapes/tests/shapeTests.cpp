#include "../src/Shapes/Geometry.h"
#include "../src/Shapes/Models/CircleModel.h"
#include "../src/Shapes/Models/RectangleModel.h"
#include "../src/Shapes/Shape.h"

#include "./MockCanvas.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;

namespace shapes
{

TEST(ShapeTest, ChangeShapeStrategyTest)
{
	gfx::MockCanvas canvas;
	auto rectangleModel = std::make_unique<RectangleModel>(Bounds{ { 0, 0 }, { 10, 10 } });
	auto circleModel = std::make_unique<CircleModel>(Position{ 50, 50 }, 25);

	Shape shape(std::move(rectangleModel), "test_shape", Color("#ff0000ff"));

	EXPECT_CALL(canvas, SetColor(_));
	EXPECT_CALL(canvas, MoveTo(0, 0));
	EXPECT_CALL(canvas, LineTo(10, 0));
	EXPECT_CALL(canvas, LineTo(10, 10));
	EXPECT_CALL(canvas, LineTo(0, 10));
	EXPECT_CALL(canvas, LineTo(0, 0));
	shape.Draw(canvas);

	testing::Mock::VerifyAndClearExpectations(&canvas);

	shape.ChangeShape(std::move(circleModel));

	EXPECT_CALL(canvas, SetColor(_));
	EXPECT_CALL(canvas, DrawEllipse(50, 50, 25, 25));
	shape.Draw(canvas);
}

TEST(ShapeTest, OldModelAfterChangeTest)
{
	auto oldModel = std::make_unique<RectangleModel>(Bounds{ { 0, 0 }, { 10, 10 } });
	auto* oldModelPtr = oldModel.get();
	auto newModel = std::make_unique<CircleModel>(Position{ 50, 50 }, 25);

	Shape shape(std::move(oldModel), "test_shape");

	auto oldBounds = oldModelPtr->GetBounds();
	EXPECT_DOUBLE_EQ(oldBounds.position.x, 0);
	EXPECT_DOUBLE_EQ(oldBounds.position.y, 0);

	shape.ChangeShape(std::move(newModel));

	EXPECT_FALSE(oldModel);
}

} // namespace shapes
