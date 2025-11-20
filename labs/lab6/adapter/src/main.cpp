#include <iostream>
#include <string>

#include "lib/GraphicsAdapter.h"
#include "lib/ModernGraphicsLib.h"
#include "lib/ShapeDrawingLib.h"

namespace app
{

using namespace shape_drawing_lib;

void PaintPicture(shape_drawing_lib::CanvasPainter& painter)
{
	shape_drawing_lib::Triangle triangle(
		Point(10, 15), Point(100, 200), Point(150, 250), 0x161415);
	shape_drawing_lib::Rectangle rectangle(Point(30, 40), 18, 24, 0xffaabb);

	painter.Draw(triangle);
	painter.Draw(rectangle);
};

void PaintPictureOnCanvas()
{
	graphics_lib::Canvas canvas(std::cout);
	shape_drawing_lib::CanvasPainter painter(canvas);
	PaintPicture(painter);
};

void PaintPictureOnModernGraphicsRenderer()
{
	modern_graphics_lib::ModernGraphicsRenderer renderer(std::cout);
	ObjectModernCanvasAdapter adapter(
		std::make_shared<modern_graphics_lib::ModernGraphicsRenderer>(
			renderer));
	shape_drawing_lib::CanvasPainter painter(adapter);
	PaintPicture(painter);
}
} // namespace app

int main()
{
	std::cout << "Should we use new API (y)?";
	std::string userInput;
	if (getline(std::cin, userInput) && (userInput == "y" || userInput == "Y"))
	{
		app::PaintPictureOnModernGraphicsRenderer();
	}
	else
	{
		app::PaintPictureOnCanvas();
	}
	return 0;
}
