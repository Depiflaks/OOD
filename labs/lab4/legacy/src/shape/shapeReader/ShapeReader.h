//
// Created by smmm on 11.05.2025.
//

#ifndef SHAPEREADER_H
#define SHAPEREADER_H
#include <memory>
#include <shape/Shape.h>

class ShapeReadException final : public std::runtime_error
{
public:
	explicit ShapeReadException(const std::string& message)
		: std::runtime_error(message)
	{
	}
};

class ShapeReader
{
public:
	static std::shared_ptr<Shape> ReadShape(std::istream& is);
	static std::vector<std::shared_ptr<Shape>> ReadShapes(std::istream& is);

private:
	[[nodiscard]] static ShapeType ParseShapeType(const std::string& typeStr);
	static void CheckShapeReadCorrect(const std::istream& is, const std::string& name);
};

#endif // SHAPEREADER_H
