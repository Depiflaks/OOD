#ifndef CIRCLEPARSER_H
#define CIRCLEPARSER_H

#include "../../Shapes/Models/CircleModel.h"
#include "../Regex/Regex.h"
#include "./IShapeParser.h"
#include <regex>
namespace commands
{

class CircleParser : public IShapeParser
{
public:
	std::unique_ptr<shapes::IShapeModel> Parse(const std::string& shapeParameters) override
	{
		static const std::regex pattern(R"(circle\s+(\d+\.?\d*)\s+(\d+\.?\d*)\s+(\d+\.?\d*))");
		auto args = ParseRegex(shapeParameters, pattern);

		double x = std::stod(args[0]);
		double y = std::stod(args[1]);
		double r = std::stod(args[2]);

		return std::make_unique<shapes::CircleModel>(shapes::Position{ x, y }, r);
	}
};
} // namespace commands

#endif /* CIRCLEPARSER_H */
