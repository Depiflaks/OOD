#ifndef SHAPEFACTORY_H
#define SHAPEFACTORY_H

#include "lib/Shape.h"
#include <memory>
#include <sstream>

enum class ShapeType
{
	Triangle,
	Rectangle,
	Ellipse,
	RegularPolygon,
	Unknown
};

class IShapeFactory
{
public:
	virtual ~IShapeFactory() = default;
	virtual std::unique_ptr<Shape> CreateShape(const std::string& descr) = 0;
};

class ShapeFactory : public IShapeFactory
{
public:
	std::unique_ptr<Shape> CreateShape(const std::string& descr) override
	{
		std::istringstream iss(descr);
		std::string shapeType;
		iss >> shapeType;

		ShapeType type = ExtractShapeType(shapeType);

		switch (type)
		{
		case ShapeType::Triangle:
			return CreateTriangle(descr);
		case ShapeType::Rectangle:
			return CreateRectangle(descr);
		case ShapeType::Ellipse:
			return CreateEllipse(descr);
		case ShapeType::RegularPolygon:
			return CreateRegularPolygon(descr);
		default:
			throw std::invalid_argument("Unknown shape type");
		}
	}

private:
	ShapeType ExtractShapeType(const std::string& typeStr)
	{
		if (typeStr == "triangle")
			return ShapeType::Triangle;
		if (typeStr == "rectangle")
			return ShapeType::Rectangle;
		if (typeStr == "ellipse")
			return ShapeType::Ellipse;
		if (typeStr == "polygon")
			return ShapeType::RegularPolygon;
		return ShapeType::Unknown;
	}

	std::unique_ptr<Shape> CreateTriangle(const std::string& descr)
	{
		std::istringstream iss(descr);
		std::string type;
		double x1, y1, x2, y2, x3, y3;
		std::string colorStr;

		iss >> type >> x1 >> y1 >> x2 >> y2 >> x3 >> y3 >> colorStr;
		if (iss.fail())
		{
			throw std::invalid_argument("Invalid triangle format");
		}

		return std::make_unique<Triangle>(Point{ x1, y1 }, Point{ x2, y2 },
			Point{ x3, y3 }, Color{ colorStr });
	}

	std::unique_ptr<Shape> CreateRectangle(const std::string& descr)
	{
		std::istringstream iss(descr);
		std::string type;
		double x1, y1, x2, y2;
		std::string colorStr;

		iss >> type >> x1 >> y1 >> x2 >> y2 >> colorStr;
		if (iss.fail())
		{
			throw std::invalid_argument("Invalid rectangle format");
		}

		return std::make_unique<Rectangle>(
			Point{ x1, y1 }, Point{ x2, y2 }, Color{ colorStr });
	}

	std::unique_ptr<Shape> CreateEllipse(const std::string& descr)
	{
		std::istringstream iss(descr);
		std::string type;
		double cx, cy, rx, ry;
		std::string colorStr;

		iss >> type >> cx >> cy >> rx >> ry >> colorStr;
		if (iss.fail())
		{
			throw std::invalid_argument("Invalid ellipse format");
		}

		return std::make_unique<Ellipse>(
			Point{ cx, cy }, rx, ry, Color{ colorStr });
	}

	std::unique_ptr<Shape> CreateRegularPolygon(const std::string& descr)
	{
		std::istringstream iss(descr);
		std::string type;
		double cx, cy, radius;
		int vertexCount;
		std::string colorStr;

		iss >> type >> cx >> cy >> radius >> vertexCount >> colorStr;
		if (iss.fail())
		{
			throw std::invalid_argument("Invalid polygon format");
		}

		return std::make_unique<RegularPolygon>(
			Point{ cx, cy }, radius, vertexCount, Color{ colorStr });
	}
};

#endif /* SHAPEFACTORY_H */
