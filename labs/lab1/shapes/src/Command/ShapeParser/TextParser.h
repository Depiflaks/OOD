#ifndef TEXTPARSER_H
#define TEXTPARSER_H

#include "../../Shapes/Models/TextModel.h"
#include "../Regex/Regex.h"
#include "./IShapeParser.h"
#include <regex>
namespace commands
{

class TextParser : public IShapeParser
{
public:
	std::unique_ptr<shapes::IShapeModel> Parse(const std::string& shapeParameters) override
	{
		static const std::regex pattern(R"(text\s+(\d+\.?\d*)\s+(\d+\.?\d*)\s+(\d+\.?\d*)\s+(.+))");
		auto args = ParseRegex(shapeParameters, pattern);

		double left = std::stod(args[0]);
		double top = std::stod(args[1]);
		double size = std::stod(args[2]);
		std::string text = args[3];

		return std::make_unique<shapes::TextModel>(shapes::Position{ left, top }, size, text);
	}
};

} // namespace commands

#endif /* TEXTPARSER_H */
