#ifndef COMMANDS_H
#define COMMANDS_H

#include "../Regex/Regex.h"
#include "../ShapeParser/ShapeParserFactory.h"
#include "./ICommand.h"
#include <regex>

namespace commands
{

class AddShapeCommand : public ICommand
{
public:
	void Execute(shapes::Picture& picture, const std::string& argumentsLine) override
	{
		static const std::regex pattern(R"((\w+)\s+(\#[\w]+)\s+(\w+)\s+(.+))");
		auto args = ParseRegex(argumentsLine, pattern);

		std::string id = args[0];
		Color color(args[1]);
		std::string shapeType = args[2];
		std::string shapeParams = args[3];

		auto parser = ShapeParserFactory::GetParserByType(shapeType);
		auto model = parser->Parse(shapeType + " " + shapeParams);

		picture.AddShape(id, color, std::move(model));
	}
};

class DrawPictureCommand : public ICommand
{
public:
	void Execute(shapes::Picture& picture, const std::string& argumentsLine) override
	{
		if (!argumentsLine.empty())
		{
			throw std::runtime_error("DrawPicture command takes no arguments");
		}
		picture.DrawPicture();
	}
};

class DrawShapeCommand : public ICommand
{
public:
	void Execute(shapes::Picture& picture, const std::string& argumentsLine) override
	{
		static const std::regex pattern(R"((\w+))");
		auto args = ParseRegex(argumentsLine, pattern);

		std::string id = args[0];
		picture.DrawShape(id);
	}
};

class ChangeShapeCommand : public ICommand
{
public:
	void Execute(shapes::Picture& picture, const std::string& argumentsLine) override
	{
		static const std::regex pattern(R"((\w+)\s+(\w+)\s+(.+))");
		auto args = ParseRegex(argumentsLine, pattern);

		std::string id = args[0];
		std::string shapeType = args[1];
		std::string shapeParams = args[2];

		auto parser = ShapeParserFactory::GetParserByType(shapeType);
		auto model = parser->Parse(shapeType + " " + shapeParams);

		picture.ChangeShape(id, std::move(model));
	}
};

class ChangeColorCommand : public ICommand
{
public:
	void Execute(shapes::Picture& picture, const std::string& argumentsLine) override
	{
		static const std::regex pattern(R"((\w+)\s+(\#[\w]+))");
		auto args = ParseRegex(argumentsLine, pattern);

		std::string id = args[0];
		Color color(args[1]);
		picture.ChangeColor(id, color);
	}
};

class ListCommand : public ICommand
{
public:
	void Execute(shapes::Picture& picture, const std::string& argumentsLine) override
	{
		if (!argumentsLine.empty())
		{
			throw std::runtime_error("List command takes no arguments");
		}
		picture.List();
	}
};

class DeleteShapeCommand : public ICommand
{
public:
	void Execute(shapes::Picture& picture, const std::string& argumentsLine) override
	{
		static const std::regex pattern(R"((\w+))");
		auto args = ParseRegex(argumentsLine, pattern);

		std::string id = args[0];
		picture.DeleteShape(id);
	}
};

class MovePictureCommand : public ICommand
{
public:
	void Execute(shapes::Picture& picture, const std::string& argumentsLine) override
	{
		static const std::regex pattern(R"((\-?\d+\.?\d*)\s+(\-?\d+\.?\d*))");
		auto args = ParseRegex(argumentsLine, pattern);

		double dx = std::stod(args[0]);
		double dy = std::stod(args[1]);
		picture.MovePicture(dx, dy);
	}
};

class MoveShapeCommand : public ICommand
{
public:
	void Execute(shapes::Picture& picture, const std::string& argumentsLine) override
	{
		static const std::regex pattern(R"((\w+)\s+(\-?\d+\.?\d*)\s+(\-?\d+\.?\d*))");
		auto args = ParseRegex(argumentsLine, pattern);

		std::string id = args[0];
		double dx = std::stod(args[1]);
		double dy = std::stod(args[2]);
		picture.MoveShape(id, dx, dy);
	}
};
} // namespace commands

#endif /* COMMANDS_H */
