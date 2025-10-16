//
// Created by smmm on 11.05.2025.
//

#include "ShapeReader.h"

#include <cassert>
#include <iostream>
#include <shape/lineSegment/LineSegment.h>
#include <shape/solidShape/circle/Circle.h>
#include <shape/solidShape/rectangle/Rectangle.h>
#include <shape/solidShape/triangle/Triangle.h>

std::shared_ptr<Shape> ShapeReader::ReadShape(std::istream& is)
{
	std::string typeStr;
	is >> typeStr;
	if (is.fail())
		throw ShapeReadException("Failed to read shape type from input stream");

	switch (ParseShapeType(typeStr))
	{
	case ShapeType::Triangle: {
		Triangle triangle;
		is >> triangle;
		CheckShapeReadCorrect(is, triangle.ToString());
		return std::make_unique<Triangle>(triangle);
	}
	case ShapeType::Rectangle: {
		Rectangle rectangle;
		is >> rectangle;
		CheckShapeReadCorrect(is, rectangle.ToString());
		return std::make_unique<Rectangle>(rectangle);
	}
	case ShapeType::Circle: {
		Circle circle;
		is >> circle;
		CheckShapeReadCorrect(is, circle.ToString());
		return std::make_unique<Circle>(circle);
	}
	case ShapeType::LineSegment: {
		LineSegment line;
		is >> line;
		CheckShapeReadCorrect(is, line.ToString());
		return std::make_unique<LineSegment>(line);
	}
	case ShapeType::Unknown:
		throw ShapeReadException("Unknown shape type: " + typeStr);
	default:
		assert(false);
	}
}

std::vector<std::shared_ptr<Shape>> ShapeReader::ReadShapes(std::istream& is)
{
	std::vector<std::shared_ptr<Shape>> shapes;
	while (is)
	{
		try
		{
			auto shape = ReadShape(is);
			shapes.push_back(shape);
		}
		catch (ShapeReadException& message)
		{
			std::cout << message.what() << '\n';
		}
	}
	return shapes;
}

void ShapeReader::CheckShapeReadCorrect(const std::istream& is, const std::string& name)
{
	if (is.fail())
		throw ShapeReadException("Failed to read " + name);
}

ShapeType ShapeReader::ParseShapeType(const std::string& typeStr)
{
	if (typeStr == "triangle")
		return ShapeType::Triangle;
	if (typeStr == "rectangle")
		return ShapeType::Rectangle;
	if (typeStr == "circle")
		return ShapeType::Circle;
	if (typeStr == "line")
		return ShapeType::LineSegment;
	return ShapeType::Unknown;
}
