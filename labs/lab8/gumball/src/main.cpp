#include "lib/Command.h"
#include "lib/GumBallMachine.h"
#include <iostream>

int main()
{
	GumballMachine machine(5);
	command::CommandFactory factory(machine);

	std::string line;
	while (true)
	{
		if (!std::getline(std::cin, line))
		{
			break;
		}

		try
		{
			auto cmd = factory.Parse(line);
			cmd->Execute(machine);
		}
		catch (const command::CommandParseError& e)
		{
			std::cout << e.what() << "\n";
			std::cout << FormatExpectedCommands(
				command::GetSupportedCommands());
		}
	}

	return 0;
}
