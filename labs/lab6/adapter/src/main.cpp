#include <iostream>
#include <string>

#include "lib/ModernGraphicsLib.h"
#include "lib/ShapeDrawingLib.h"

namespace app
{
void PaintPicture(shape_drawing_lib::CanvasPainter& painter) {

};

void PaintPictureOnCanvas() {

};

void PaintPictureOnModernGraphicsRenderer()
{
	modern_graphics_lib::ModernGraphicsRenderer renderer(std::cout);
	(void)&renderer; // устраняем предупреждение о неиспользуемой переменной
	// TODO: при помощи существующей функции PaintPicture() нарисовать
	// картину на renderer
	// Подсказка: используйте паттерн "Адаптер"
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
