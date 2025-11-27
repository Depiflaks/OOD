//
// Created by smmm on 28.11.2025.
//

#include "../src/lib/Canvas.h"
#include "../src/lib/Shapes/Shapes.h"
#include <cmath>
#include <functional>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <string>

class MockCanvas final : public ICanvas
{
public:
	MOCK_METHOD(void, SetLineColor, (RGBAColor color), (override));
	MOCK_METHOD(void, BeginFill, (RGBAColor color), (override));
	MOCK_METHOD(void, EndFill, (), (override));
	MOCK_METHOD(void, MoveTo, (double x, double y), (override));
	MOCK_METHOD(void, LineTo, (double x, double y), (override));
	MOCK_METHOD(void, DrawEllipse,
		(double left, double top, double width, double height), (override));
	MOCK_METHOD(void, SetLineWidth, (int width), (override));
};

class SimpleShapeTest : public testing::Test
{
protected:
	MockCanvas canvas;
	const RectD TEST_FRAME = { 10.0, 20.0, 100.0, 50.0 };
	const RGBAColor FILL_COLOR = 0xFF0000FF;
	const RGBAColor OUTLINE_COLOR = 0x00FF00FF;
	const RGBAColor DEFAULT_COLOR = 0x0;
	const RGBAColor DISABLE_MASK = 0x00FFFFFF;

	std::shared_ptr<IShape> createShape(
		std::shared_ptr<DrawingStrategy> strategy, bool fillEnabled = true,
		bool outlineEnabled = true)
	{
		return std::make_shared<SimpleShape>(std::move(strategy), TEST_FRAME,
			std::make_unique<Style>(fillEnabled, FILL_COLOR),
			std::make_unique<Style>(outlineEnabled, OUTLINE_COLOR));
	}

	testing::Matcher<double> DoubleNear(double value) const
	{
		return testing::DoubleNear(value, 0.0001);
	}
};

TEST_F(SimpleShapeTest, EllipseStrategyDrawsCorrectly)
{
	auto strategy = std::make_shared<DrawingStrategy>(MakeEllipseStrategy());
	auto shape = createShape(strategy);

	testing::InSequence s;
	EXPECT_CALL(canvas, BeginFill(FILL_COLOR));
	EXPECT_CALL(canvas, SetLineColor(OUTLINE_COLOR));
	EXPECT_CALL(canvas,
		DrawEllipse(DoubleNear(TEST_FRAME.left), DoubleNear(TEST_FRAME.top),
			DoubleNear(TEST_FRAME.width), DoubleNear(TEST_FRAME.height)));
	EXPECT_CALL(canvas, EndFill());

	shape->Draw(canvas);
}

TEST_F(SimpleShapeTest, RectangleStrategyDrawsCorrectly)
{
	auto strategy = std::make_shared<DrawingStrategy>(MakeRectangleStrategy());
	auto shape = createShape(strategy);

	testing::InSequence s;
	EXPECT_CALL(canvas, BeginFill(FILL_COLOR));
	EXPECT_CALL(canvas, SetLineColor(OUTLINE_COLOR));

	EXPECT_CALL(canvas, MoveTo(DoubleNear(10.0), DoubleNear(20.0)));
	EXPECT_CALL(canvas, LineTo(DoubleNear(110.0), DoubleNear(20.0)));
	EXPECT_CALL(canvas, LineTo(DoubleNear(110.0), DoubleNear(70.0)));
	EXPECT_CALL(canvas, LineTo(DoubleNear(10.0), DoubleNear(70.0)));
	EXPECT_CALL(canvas, LineTo(DoubleNear(10.0), DoubleNear(20.0)));

	EXPECT_CALL(canvas, EndFill());

	shape->Draw(canvas);
}

TEST_F(SimpleShapeTest, PolygonStrategyDrawsCorrectly)
{
	auto strategy = std::make_shared<DrawingStrategy>(MakePolygonStrategy(4));
	auto shape = createShape(strategy);

	testing::InSequence s;
	EXPECT_CALL(canvas, BeginFill(FILL_COLOR));
	EXPECT_CALL(canvas, SetLineColor(OUTLINE_COLOR));

	EXPECT_CALL(canvas, MoveTo(DoubleNear(110.0), DoubleNear(45.0)));

	EXPECT_CALL(canvas, LineTo(DoubleNear(60.0), DoubleNear(70.0)));
	EXPECT_CALL(canvas, LineTo(DoubleNear(10.0), DoubleNear(45.0)));
	EXPECT_CALL(canvas, LineTo(DoubleNear(60.0), DoubleNear(20.0)));

	EXPECT_CALL(canvas, LineTo(DoubleNear(110.0), DoubleNear(45.0)));

	EXPECT_CALL(canvas, EndFill());

	shape->Draw(canvas);
}

TEST_F(SimpleShapeTest, DrawWithFillDisabledUsesDefaultColor)
{
	auto strategy = std::make_shared<DrawingStrategy>(MakeEllipseStrategy());
	auto shape = createShape(strategy, false, true);

	testing::InSequence s;
	EXPECT_CALL(canvas, BeginFill(FILL_COLOR & DISABLE_MASK));
	EXPECT_CALL(canvas, SetLineColor(OUTLINE_COLOR));
	EXPECT_CALL(canvas,
		DrawEllipse(DoubleNear(TEST_FRAME.left), DoubleNear(TEST_FRAME.top),
			DoubleNear(TEST_FRAME.width), DoubleNear(TEST_FRAME.height)));
	EXPECT_CALL(canvas, EndFill());

	shape->Draw(canvas);
}

TEST_F(SimpleShapeTest, DrawWithOutlineDisabledUsesDefaultColor)
{
	auto strategy = std::make_shared<DrawingStrategy>(MakeEllipseStrategy());
	auto shape = createShape(strategy, true, false);

	testing::InSequence s;
	EXPECT_CALL(canvas, BeginFill(FILL_COLOR));
	EXPECT_CALL(canvas, SetLineColor(OUTLINE_COLOR & DISABLE_MASK));
	EXPECT_CALL(canvas,
		DrawEllipse(DoubleNear(TEST_FRAME.left), DoubleNear(TEST_FRAME.top),
			DoubleNear(TEST_FRAME.width), DoubleNear(TEST_FRAME.height)));
	EXPECT_CALL(canvas, EndFill());

	shape->Draw(canvas);
}

TEST_F(SimpleShapeTest, DrawWithBothStylesDisabledUsesDefaultColor)
{
	auto strategy = std::make_shared<DrawingStrategy>(MakeEllipseStrategy());
	auto shape = createShape(strategy, false, false);

	testing::InSequence s;
	EXPECT_CALL(canvas, BeginFill(FILL_COLOR & DISABLE_MASK));
	EXPECT_CALL(canvas, SetLineColor(OUTLINE_COLOR & DISABLE_MASK));
	EXPECT_CALL(canvas,
		DrawEllipse(DoubleNear(TEST_FRAME.left), DoubleNear(TEST_FRAME.top),
			DoubleNear(TEST_FRAME.width), DoubleNear(TEST_FRAME.height)));
	EXPECT_CALL(canvas, EndFill());

	shape->Draw(canvas);
}

TEST_F(SimpleShapeTest, CloneCreatesDeepCopy)
{
	auto strategy = std::make_shared<DrawingStrategy>(MakeEllipseStrategy());
	auto originalShape = createShape(strategy);

	std::shared_ptr<IShape> clonedShape = originalShape->Clone();

	ASSERT_NE(clonedShape, nullptr);

	ASSERT_EQ(clonedShape->GetFrame().left, TEST_FRAME.left);
	ASSERT_EQ(clonedShape->GetFillStyle().GetColor().value(), FILL_COLOR);

	const RectD NEW_FRAME = { 1.0, 1.0, 10.0, 10.0 };
	const RGBAColor NEW_FILL_COLOR = 0x000000FF;
	clonedShape->SetFrame(NEW_FRAME);
	clonedShape->GetFillStyle().SetColor(NEW_FILL_COLOR);

	EXPECT_EQ(clonedShape->GetFrame().left, NEW_FRAME.left);
	EXPECT_EQ(clonedShape->GetFillStyle().GetColor().value(), NEW_FILL_COLOR);

	EXPECT_EQ(originalShape->GetFrame().left, TEST_FRAME.left);
	EXPECT_EQ(originalShape->GetFillStyle().GetColor().value(), FILL_COLOR);

	EXPECT_CALL(canvas, BeginFill(NEW_FILL_COLOR));
	EXPECT_CALL(canvas, SetLineColor(OUTLINE_COLOR));
	EXPECT_CALL(canvas,
		DrawEllipse(DoubleNear(NEW_FRAME.left), DoubleNear(NEW_FRAME.top),
			DoubleNear(NEW_FRAME.width), DoubleNear(NEW_FRAME.height)));
	EXPECT_CALL(canvas, EndFill());
	clonedShape->Draw(canvas);
}
