#include "lib/Designer.h"
#include "lib/Shape.h"
#include "lib/ShapeFactory.h"

#include <fstream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <sstream>

TEST(DesignerTest, EmptyStream)
{
	auto factory = std::make_unique<ShapeFactory>();
	Designer designer(std::move(factory));

	std::istringstream iss("");
	PictureDraft draft = designer.CreateDraft(iss);

	EXPECT_EQ(draft.GetShapeCount(), 0);
}

TEST(DesignerTest, InvalidShapeFormat)
{
	auto factory = std::make_unique<ShapeFactory>();
	Designer designer(std::move(factory));

	std::istringstream iss("triangle 0 0 10 0 5\nrectangle 0 0 10");

	EXPECT_THROW(designer.CreateDraft(iss), std::runtime_error);
}

TEST(DesignerTest, SingleShape)
{
	auto factory = std::make_unique<ShapeFactory>();
	Designer designer(std::move(factory));

	std::istringstream iss("rectangle 0 0 10 10 #ff0000ff");
	PictureDraft draft = designer.CreateDraft(iss);

	EXPECT_EQ(draft.GetShapeCount(), 1);

	Shape& shape = draft.GetShape(0);
	auto* rect = dynamic_cast<Rectangle*>(&shape);
	ASSERT_NE(rect, nullptr);
	EXPECT_EQ(rect->GetLeftTop().x, 0);
	EXPECT_EQ(rect->GetLeftTop().y, 0);
	EXPECT_EQ(rect->GetRightBottom().x, 10);
	EXPECT_EQ(rect->GetRightBottom().y, 10);
	EXPECT_EQ(rect->GetColor().ToHexString(), "#ff0000ff");
}

TEST(DesignerTest, FromFile)
{
	auto factory = std::make_unique<ShapeFactory>();
	Designer designer(std::move(factory));

	std::ofstream outFile("test_shapes.txt");
	outFile << "rectangle 0 0 10 10 #ff0000ff\n";
	outFile << "triangle 0 0 10 0 5 10 #00ff00ff\n";
	outFile << "ellipse 5 5 3 2 #0000ffff\n";
	outFile.close();

	std::ifstream inFile("test_shapes.txt");
	PictureDraft draft = designer.CreateDraft(inFile);
	inFile.close();

	std::remove("test_shapes.txt");

	EXPECT_EQ(draft.GetShapeCount(), 3);

	Shape& shape1 = draft.GetShape(0);
	auto* rect = dynamic_cast<Rectangle*>(&shape1);
	ASSERT_NE(rect, nullptr);
	EXPECT_EQ(rect->GetColor().ToHexString(), "#ff0000ff");

	Shape& shape2 = draft.GetShape(1);
	auto* triangle = dynamic_cast<Triangle*>(&shape2);
	ASSERT_NE(triangle, nullptr);
	EXPECT_EQ(triangle->GetColor().ToHexString(), "#00ff00ff");

	Shape& shape3 = draft.GetShape(2);
	auto* ellipse = dynamic_cast<Ellipse*>(&shape3);
	ASSERT_NE(ellipse, nullptr);
	EXPECT_EQ(ellipse->GetColor().ToHexString(), "#0000ffff");
}

TEST(DesignerTest, AllShapeTypes)
{
	auto factory = std::make_unique<ShapeFactory>();
	Designer designer(std::move(factory));

	std::istringstream iss(
		"rectangle 0 0 10 10 #ff0000ff\n"
		"triangle 0 0 10 0 5 10 #00ff00ff\n"
		"ellipse 5 5 3 2 #0000ffff\n"
		"polygon 7 7 4 6 #ffff00ff\n");

	PictureDraft draft = designer.CreateDraft(iss);

	EXPECT_EQ(draft.GetShapeCount(), 4);

	Shape& shape1 = draft.GetShape(0);
	auto* rect = dynamic_cast<Rectangle*>(&shape1);
	ASSERT_NE(rect, nullptr);
	EXPECT_EQ(rect->GetLeftTop().x, 0);
	EXPECT_EQ(rect->GetLeftTop().y, 0);
	EXPECT_EQ(rect->GetRightBottom().x, 10);
	EXPECT_EQ(rect->GetRightBottom().y, 10);

	Shape& shape2 = draft.GetShape(1);
	auto* triangle = dynamic_cast<Triangle*>(&shape2);
	ASSERT_NE(triangle, nullptr);
	EXPECT_EQ(triangle->GetVertex1().x, 0);
	EXPECT_EQ(triangle->GetVertex1().y, 0);
	EXPECT_EQ(triangle->GetVertex2().x, 10);
	EXPECT_EQ(triangle->GetVertex2().y, 0);
	EXPECT_EQ(triangle->GetVertex3().x, 5);
	EXPECT_EQ(triangle->GetVertex3().y, 10);

	Shape& shape3 = draft.GetShape(2);
	auto* ellipse = dynamic_cast<Ellipse*>(&shape3);
	ASSERT_NE(ellipse, nullptr);
	EXPECT_EQ(ellipse->GetCenter().x, 5);
	EXPECT_EQ(ellipse->GetCenter().y, 5);
	EXPECT_EQ(ellipse->GetHorizontalRadius(), 3);
	EXPECT_EQ(ellipse->GetVerticalRadius(), 2);

	Shape& shape4 = draft.GetShape(3);
	auto* polygon = dynamic_cast<RegularPolygon*>(&shape4);
	ASSERT_NE(polygon, nullptr);
	EXPECT_EQ(polygon->GetCenter().x, 7);
	EXPECT_EQ(polygon->GetCenter().y, 7);
	EXPECT_EQ(polygon->GetRadius(), 4);
	EXPECT_EQ(polygon->GetVertexCount(), 6);
}
