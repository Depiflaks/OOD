#ifndef SHAPESCOMMANDLINE_H
#define SHAPESCOMMANDLINE_H

#include "../Command/CommandListener.h"
#include "./Picture.h"
#include "Command/CommandExecutor.h"
#include <iostream>

namespace shapes
{

class ShapesCommandLine
{
public:
	ShapesCommandLine()
		: m_executor(commands::CommandExecutor{})
	{
	}

	void ExecuteCommand(Picture& picture, commands::ICommandListener& listener)
	{
		try
		{
			Execute(picture, listener);
		}
		catch (const std::runtime_error& e)
		{
			std::cout << "Error: " << e.what() << std::endl;
		}
	}

private:
	void Execute(Picture& picture, commands::ICommandListener& listener)
	{
		std::string commandLine = listener.GetLine();
		if (commandLine.empty())
		{
			return;
		}

		m_executor.Execute(picture, commandLine);
	}

	commands::CommandExecutor m_executor;
};
} // namespace shapes

#endif /* SHAPESCOMMANDLINE_H */
