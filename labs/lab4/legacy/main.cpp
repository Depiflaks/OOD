//
// Created by smmm on 06.05.2025.
//

#include "shape/shapeReader/ShapeReader.h"
#include "./src/openGl/openGl.h"

#include <iostream>

void PrintShape(const std::shared_ptr<Shape>& shape)
{
	if (shape == nullptr)
		std::cout << "No shapes available.\n";
	else
		shape->Print(std::cout);
}

int main()
{
	const auto shapes = ShapeReader::ReadShapes(std::cin);
	try
	{
		DrawPicture(shapes);
	}
	catch (OpenGlException& exception)
	{
		std::cout << exception.what() << '\n';
		return 1;
	}
	return 0;
}
