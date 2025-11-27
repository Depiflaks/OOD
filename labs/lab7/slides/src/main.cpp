//
// Created by smmm on 21.11.2025.
//

#include "lib/Shapes/Shapes.h"

#include <iostream>

int main()
{
	std::cout << "--- Initialization ---" << std::endl;
	auto ellipseStrategy
		= std::make_shared<DrawingStrategy>(MakeEllipseStrategy());

	RGBAColor originalFill = 0xFF0000FF; // Красный
	RGBAColor originalOutline = 0x00FF00FF; // Зеленый

	auto originalShape = std::make_shared<SimpleShape>(ellipseStrategy,
		RectD{ 10, 20, 100, 50 }, std::make_unique<Style>(true, originalFill),
		std::make_unique<Style>(true, originalOutline));

	// 3. Клонирование фигуры
	std::cout << "\n--- Cloning Shape ---" << std::endl;
	auto clonedShape = originalShape->Clone();

	RGBAColor newClonedFill = 0x0000FFFF; // Синий
	clonedShape->GetFillStyle().SetColor(newClonedFill);
	clonedShape->SetFrame(RectD{ 50, 50, 75, 75 });
	// 5. Проверка глубокого копирования (Original не изменилась)
	std::cout << "\n--- Final Check (Original vs Cloned) ---" << std::endl;
	std::cout << "Original Fill Color: "
			  << originalShape->GetFillStyle().GetColor().value() << std::endl;
	std::cout << "Cloned Fill Color:   "
			  << clonedShape->GetFillStyle().GetColor().value() << std::endl;
	std::cout << "Original Frame X:    " << originalShape->GetFrame().left
			  << std::endl;
	std::cout << "Cloned Frame X:      " << clonedShape->GetFrame().left
			  << std::endl;

	return 0;
}
