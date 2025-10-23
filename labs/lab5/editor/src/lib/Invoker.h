#ifndef INVOKER_H
#define INVOKER_H

#include "Command/Command.h"

class ICommandManager
{
public:
	virtual ~ICommandManager() = default;
	virtual void ExecuteAndAddCommand(std::unique_ptr<ICommand> command) = 0;
};

#endif /* INVOKER_H */
