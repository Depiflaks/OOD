//
// Created by smmm on 03.12.2025.
//

#ifndef OOD_COMMAND_H
#define OOD_COMMAND_H
#include "GumBallMachine.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

inline std::string FormatExpectedCommands(
	const std::vector<std::string>& commands)
{
	std::string result = "Expected commands:\n";
	for (const auto& cmd : commands)
	{
		result += "- " + cmd + "\n";
	}
	return result;
}

namespace command
{
inline std::vector<std::string> GetSupportedCommands()
{
	return { "help", "insert", "eject", "turn", "refill <count>" };
}

class CommandParseError final : public std::runtime_error
{
public:
	explicit CommandParseError(const std::string& message)
		: std::runtime_error(message)
	{
	}
};

class ICommand
{
public:
	virtual ~ICommand() = default;
	virtual void Execute(GumballMachine& machine) = 0;
};

class HelpCommand final : public ICommand
{
public:
	HelpCommand() = default;

	void Execute(GumballMachine& machine) override
	{
		std::cout << FormatExpectedCommands(GetSupportedCommands());
		std::cout << machine.ToString();
	}
};

class InsertQuarterCommand final : public ICommand
{
public:
	InsertQuarterCommand() = default;

	void Execute(GumballMachine& machine) override
	{
		machine.InsertQuarter();
	}
};

class EjectQuarterCommand final : public ICommand
{
public:
	EjectQuarterCommand() = default;

	void Execute(GumballMachine& machine) override
	{
		machine.EjectQuarter();
	}
};

class TurnCrankCommand final : public ICommand
{
public:
	TurnCrankCommand() = default;

	void Execute(GumballMachine& machine) override
	{
		machine.TurnCrank();
	}
};

class RefillMachineCommand final : public ICommand
{
public:
	explicit RefillMachineCommand(unsigned count)
		: m_count(count)
	{
	}

	void Execute(GumballMachine& machine) override
	{
		machine.RefillMachine(m_count);
	}

private:
	unsigned m_count = 0;
};

class CommandFactory
{
public:
	explicit CommandFactory(GumballMachine& machine)
		: m_machine(machine)
	{
	}

	std::unique_ptr<ICommand> Parse(const std::string& line) const
	{
		std::istringstream iss(line);
		std::string cmd;
		iss >> cmd;

		if (cmd.empty())
		{
			throw CommandParseError("Empty command");
		}

		if (cmd == "help")
		{
			std::string extra;
			if (iss >> extra)
			{
				throw CommandParseError("help: unexpected arguments");
			}
			return std::make_unique<HelpCommand>();
		}

		if (cmd == "insert")
		{
			std::string extra;
			if (iss >> extra)
			{
				throw CommandParseError("insert: unexpected arguments");
			}
			return std::make_unique<InsertQuarterCommand>();
		}

		if (cmd == "eject")
		{
			std::string extra;
			if (iss >> extra)
			{
				throw CommandParseError("eject: unexpected arguments");
			}
			return std::make_unique<EjectQuarterCommand>();
		}

		if (cmd == "turn")
		{
			std::string extra;
			if (iss >> extra)
			{
				throw CommandParseError("turn: unexpected arguments");
			}
			return std::make_unique<TurnCrankCommand>();
		}

		if (cmd == "refill")
		{
			unsigned count = 0;
			if (!(iss >> count))
			{
				throw CommandParseError("refill: expected unsigned count");
			}
			std::string extra;
			if (iss >> extra)
			{
				throw CommandParseError("refill: unexpected arguments");
			}
			return std::make_unique<RefillMachineCommand>(count);
		}

		throw CommandParseError("Unknown command: " + cmd);
	}

private:
	GumballMachine& m_machine;
};
} // namespace command
#endif // OOD_COMMAND_H
