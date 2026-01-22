#include "../src/lib/GraphicsAdapter.h"
#include "../src/lib/ShapeDrawingLib.h"
#include <gtest/gtest.h>
#include <sstream>

using namespace shape_drawing_lib;

TEST(ClassModernCanvasAdapterTest, DrawTriangle)
{
	std::ostringstream output;
	{
		ClassModernCanvasAdapter adapter(output);
		CanvasPainter painter(adapter);

		Triangle triangle(
			Point(10, 10), Point(50, 30), Point(20, 60), 0xFF0000);
		painter.Draw(triangle);
	}
	std::string expected = "<draw>\n"
						   "  <line fromX=\"10\" fromY=\"10\" toX=\"50\" "
						   "toY=\"30\"><color value=\"#FF0000\"/></line>\n"
						   "  <line fromX=\"50\" fromY=\"30\" toX=\"20\" "
						   "toY=\"60\"><color value=\"#FF0000\"/></line>\n"
						   "  <line fromX=\"20\" fromY=\"60\" toX=\"10\" "
						   "toY=\"10\"><color value=\"#FF0000\"/></line>\n"
						   "</draw>\n";

	EXPECT_EQ(output.str(), expected);
}

TEST(ClassModernCanvasAdapterTest, DrawRectangle)
{
	std::ostringstream output;
	{
		ClassModernCanvasAdapter adapter(output);
		CanvasPainter painter(adapter);

		Rectangle rectangle(Point(5, 5), 40, 30, 0x00FF00);
		painter.Draw(rectangle);
	}

	std::string expected = "<draw>\n"
						   "  <line fromX=\"5\" fromY=\"5\" toX=\"45\" "
						   "toY=\"5\"><color value=\"#00FF00\"/></line>\n"
						   "  <line fromX=\"45\" fromY=\"5\" toX=\"45\" "
						   "toY=\"35\"><color value=\"#00FF00\"/></line>\n"
						   "  <line fromX=\"45\" fromY=\"35\" toX=\"5\" "
						   "toY=\"35\"><color value=\"#00FF00\"/></line>\n"
						   "  <line fromX=\"5\" fromY=\"35\" toX=\"5\" "
						   "toY=\"5\"><color value=\"#00FF00\"/></line>\n"
						   "</draw>\n";

	EXPECT_EQ(output.str(), expected);
}
