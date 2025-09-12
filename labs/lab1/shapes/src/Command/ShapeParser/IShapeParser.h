#ifndef ISHAPEPARSER_H
#define ISHAPEPARSER_H

#include "../../Shapes/Models/IShapeModel.h"
#include <memory>
namespace commands
{

class IShapeParser
{
public:
	virtual ~IShapeParser() = default;
	virtual std::unique_ptr<shapes::IShapeModel> Parse(const std::string& shapeParameters) = 0;
};

} // namespace commands

#endif /* ISHAPEPARSER_H */
