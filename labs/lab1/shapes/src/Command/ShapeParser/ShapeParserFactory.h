#ifndef SHAPEPARSERFACTORY_H
#define SHAPEPARSERFACTORY_H

#include <string>
#include <unordered_map>

#include "../src/Command/ShapeParser/CircleParser.h"
#include "../src/Command/ShapeParser/LineParser.h"
#include "../src/Command/ShapeParser/RectangleParser.h"
#include "../src/Command/ShapeParser/TextParser.h"
#include "../src/Command/ShapeParser/TriangleParser.h"

namespace commands
{

class ShapeParserFactory
{
public:
	static std::unique_ptr<IShapeParser> GetParserByType(const std::string& type)
	{
		static const std::unordered_map<std::string, std::function<std::unique_ptr<IShapeParser>()>> parsers = {
			{ "rectangle", []() { return std::make_unique<RectangleParser>(); } },
			{ "circle", []() { return std::make_unique<CircleParser>(); } },
			{ "text", []() { return std::make_unique<TextParser>(); } },
			{ "line", []() { return std::make_unique<LineParser>(); } },
			{ "triangle", []() { return std::make_unique<TriangleParser>(); } }
		};

		auto it = parsers.find(type);
		if (it == parsers.end())
		{
			throw std::runtime_error("Unknown shape type: " + type);
		}
		return it->second();
	}
};

} // namespace commands

#endif /* SHAPEPARSERFACTORY_H */
