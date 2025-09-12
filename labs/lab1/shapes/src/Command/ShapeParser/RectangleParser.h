#ifndef RECTANGLEPARSER_H
#define RECTANGLEPARSER_H

#include "../../Shapes/Models/RectangleModel.h"
#include "../Regex/Regex.h"
#include "./IShapeParser.h"
#include <regex>
namespace commands
{

class RectangleParser : public IShapeParser
{
public:
	std::unique_ptr<shapes::IShapeModel> Parse(const std::string& shapeParameters) override
	{
		static const std::regex pattern(R"(rectangle\s+(\d+\.?\d*)\s+(\d+\.?\d*)\s+(\d+\.?\d*)\s+(\d+\.?\d*))");
		auto args = ParseRegex(shapeParameters, pattern);

		double left = std::stod(args[0]);
		double top = std::stod(args[1]);
		double width = std::stod(args[2]);
		double height = std::stod(args[3]);

		return std::make_unique<shapes::RectangleModel>(shapes::Bounds{ { left, top }, { width, height } });
	}
};
} // namespace commands

#endif /* RECTANGLEPARSER_H */
