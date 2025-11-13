#include <cstdint>
#include <gtest/gtest.h>
#include <sstream>

#include "../src/lib/GraphicsLib.h"

namespace graphics_lib
{

TEST(CanvasTest, SetColorMoveAndLine)
{
	std::ostringstream output;
	Canvas canvas(output);

	canvas.SetColor(0xFF0000);
	canvas.MoveTo(10, 20);
	canvas.LineTo(50, 60);

	std::string expected
		= "SetColor (#FF0000)\nMoveTo (10, 20)\nLineTo (50, 60)\n";
	EXPECT_EQ(output.str(), expected);
}

TEST(CanvasTest, NegativeCoordinates)
{
	std::ostringstream output;
	Canvas canvas(output);

	canvas.SetColor(0x00FF00);
	canvas.MoveTo(-10, -20);
	canvas.LineTo(-50, -60);

	std::string expected
		= "SetColor (#00FF00)\nMoveTo (-10, -20)\nLineTo (-50, -60)\n";
	EXPECT_EQ(output.str(), expected);
}

TEST(CanvasTest, MultipleOperations)
{
	std::ostringstream output;
	Canvas canvas(output);

	canvas.SetColor(0x0000FF);
	canvas.MoveTo(0, 0);
	canvas.LineTo(100, 200);
	canvas.SetColor(0xFFFFFF);
	canvas.LineTo(300, 400);

	std::string expected = "SetColor (#0000FF)\nMoveTo (0, 0)\nLineTo (100, "
						   "200)\nSetColor (#FFFFFF)\nLineTo (300, 400)\n";
	EXPECT_EQ(output.str(), expected);
}

TEST(CanvasTest, ColorFormat)
{
	std::ostringstream output;
	Canvas canvas(output);

	canvas.SetColor(0x123456);
	canvas.SetColor(0xABCDEF);

	std::string expected = "SetColor (#123456)\nSetColor (#ABCDEF)\n";
	EXPECT_EQ(output.str(), expected);
}

} // namespace graphics_lib
