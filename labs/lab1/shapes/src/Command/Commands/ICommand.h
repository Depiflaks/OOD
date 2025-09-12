#ifndef ICOMMAND_H
#define ICOMMAND_H

#include "../../Shapes/Picture.h"
#include <string>
namespace commands
{

class ICommand
{
public:
	virtual ~ICommand() = default;
	virtual void Execute(shapes::Picture& picture, const std::string& argumentsLine) = 0;
};

} // namespace commands

#endif /* ICOMMAND_H */
