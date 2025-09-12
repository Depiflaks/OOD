#ifndef LINEPARSER_H
#define LINEPARSER_H

#include "../../Shapes/Models/LineModel.h"
#include "../Regex/Regex.h"
#include "./IShapeParser.h"
#include <regex>
namespace commands
{

class LineParser : public IShapeParser
{
public:
	std::unique_ptr<shapes::IShapeModel> Parse(const std::string& shapeParameters) override
	{
		static const std::regex pattern(R"(line\s+(\d+\.?\d*)\s+(\d+\.?\d*)\s+(\d+\.?\d*)\s+(\d+\.?\d*))");
		auto args = ParseRegex(shapeParameters, pattern);

		double x1 = std::stod(args[0]);
		double y1 = std::stod(args[1]);
		double x2 = std::stod(args[2]);
		double y2 = std::stod(args[3]);

		return std::make_unique<shapes::LineModel>(shapes::Position{ x1, y1 }, shapes::Position{ x2, y2 });
	}
};

} // namespace commands

#endif /* LINEPARSER_H */
