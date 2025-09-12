#ifndef COMMANDFACTORY_H
#define COMMANDFACTORY_H

#include <memory>

#include "./Commands.h"
#include "./ICommand.h"

namespace commands
{

class CommandFactory
{
public:
	static std::unique_ptr<ICommand> GetCommand(const std::string& commandName)
	{
		static const std::unordered_map<std::string, std::function<std::unique_ptr<ICommand>()>> commands = {
			{ "AddShape", []() { return std::make_unique<AddShapeCommand>(); } },
			{ "DrawPicture", []() { return std::make_unique<DrawPictureCommand>(); } },
			{ "DrawShape", []() { return std::make_unique<DrawShapeCommand>(); } },
			{ "ChangeShape", []() { return std::make_unique<ChangeShapeCommand>(); } },
			{ "ChangeColor", []() { return std::make_unique<ChangeColorCommand>(); } },
			{ "List", []() { return std::make_unique<ListCommand>(); } },
			{ "DeleteShape", []() { return std::make_unique<DeleteShapeCommand>(); } },
			{ "MovePicture", []() { return std::make_unique<MovePictureCommand>(); } },
			{ "MoveShape", []() { return std::make_unique<MoveShapeCommand>(); } }
		};

		auto it = commands.find(commandName);
		if (it == commands.end())
		{
			throw std::runtime_error("Unknown command: " + commandName);
		}
		return it->second();
	}
};
} // namespace commands

#endif /* COMMANDFACTORY_H */
