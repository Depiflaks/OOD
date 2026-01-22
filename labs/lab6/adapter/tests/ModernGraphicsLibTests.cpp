#include <gtest/gtest.h>
#include <sstream>

#include "../src/lib/ModernGraphicsLib.h"

namespace modern_graphics_lib
{
TEST(ModernGraphicsRendererTest, DrawLineWithoutBeginDrawThrows)
{
	std::ostringstream output;
	ModernGraphicsRenderer renderer(output);

	EXPECT_THROW(
		renderer.DrawLine(Point(0, 0), Point(10, 10)), std::logic_error);
}

TEST(ModernGraphicsRendererTest, DrawMultipleColoredLines)
{
	std::ostringstream output;
	ModernGraphicsRenderer renderer(output);

	renderer.BeginDraw();
	renderer.DrawLine(Point(0, 0), Point(10, 10), 0xFF0000);
	renderer.DrawLine(Point(10, 10), Point(20, 20), 0x00FF00);
	renderer.DrawLine(Point(20, 20), Point(30, 30), 0x0000FF);
	renderer.EndDraw();

	std::string expected = "<draw>\n"
						   "  <line fromX=\"0\" fromY=\"0\" toX=\"10\" "
						   "toY=\"10\"><color value=\"#FF0000\"/></line>\n"
						   "  <line fromX=\"10\" fromY=\"10\" toX=\"20\" "
						   "toY=\"20\"><color value=\"#00FF00\"/></line>\n"
						   "  <line fromX=\"20\" fromY=\"20\" toX=\"30\" "
						   "toY=\"30\"><color value=\"#0000FF\"/></line>\n"
						   "</draw>\n";

	EXPECT_EQ(output.str(), expected);
}

TEST(ModernGraphicsRendererTest, DestructorEndsDrawing)
{
	std::ostringstream output;

	{
		ModernGraphicsRenderer renderer(output);
		renderer.BeginDraw();
		renderer.DrawLine(Point(0, 0), Point(10, 10), 0x123456);
	}

	std::string expected = "<draw>\n"
						   "  <line fromX=\"0\" fromY=\"0\" toX=\"10\" "
						   "toY=\"10\"><color value=\"#123456\"/></line>\n"
						   "</draw>\n";

	EXPECT_EQ(output.str(), expected);
}

} // namespace modern_graphics_lib
