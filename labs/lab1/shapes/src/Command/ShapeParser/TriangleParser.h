#ifndef TRIANGLEPARSER_H
#define TRIANGLEPARSER_H

#include "../../Shapes/Models/TriangleModel.h"
#include "../Regex/Regex.h"
#include "./IShapeParser.h"
#include <regex>
namespace commands
{

class TriangleParser : public IShapeParser
{
public:
	std::unique_ptr<shapes::IShapeModel> Parse(const std::string& shapeParameters) override
	{
		static const std::regex pattern(R"(triangle\s+(\d+\.?\d*)\s+(\d+\.?\d*)\s+(\d+\.?\d*)\s+(\d+\.?\d*)\s+(\d+\.?\d*)\s+(\d+\.?\d*))");
		auto args = ParseRegex(shapeParameters, pattern);

		double x1 = std::stod(args[0]);
		double y1 = std::stod(args[1]);
		double x2 = std::stod(args[2]);
		double y2 = std::stod(args[3]);
		double x3 = std::stod(args[4]);
		double y3 = std::stod(args[5]);

		return std::make_unique<shapes::TriangleModel>(shapes::Position{ x1, y1 }, shapes::Position{ x2, y2 }, shapes::Position{ x3, y3 });
	}
};

} // namespace commands

#endif /* TRIANGLEPARSER_H */
