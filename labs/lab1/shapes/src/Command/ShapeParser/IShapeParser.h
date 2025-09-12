#ifndef ISHAPEPARSER_H
#define ISHAPEPARSER_H

namespace commands
{

class IShapeParser
{
public:
	virtual ~IShapeParser() = default;
	virtual std::unique_ptr<IShapeModel> Parse(const std::string& shapeParameters) = 0;
};


} // namespace commands

#endif /* ISHAPEPARSER_H */
