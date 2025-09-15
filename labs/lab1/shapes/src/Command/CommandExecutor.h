#ifndef COMMANDEXECUTOR_H
#define COMMANDEXECUTOR_H

#include "../Command/Commands/CommandFactory.h"
#include "../Shapes/Picture.h"
#include <iostream>

namespace commands
{

class CommandExecutor
{
public:
	void Execute(shapes::Picture& picture, const std::string& commandLine)
	{
		std::string commandName = ParseCommandName(commandLine);
		std::string arguments = GetArguments(commandLine);

		auto command = CommandFactory::GetCommand(commandName);
		command->Execute(picture, arguments);
	}

private:
	std::string ParseCommandName(const std::string& commandLine)
	{
		size_t spacePos = commandLine.find(' ');
		if (spacePos == std::string::npos)
		{
			return commandLine;
		}
		return commandLine.substr(0, spacePos);
	}

	std::string GetArguments(const std::string& commandLine)
	{
		size_t spacePos = commandLine.find(' ');
		if (spacePos == std::string::npos)
		{
			return "";
		}
		return commandLine.substr(spacePos + 1);
	}
};
} // namespace commands

#endif /* COMMANDEXECUTOR_H */
