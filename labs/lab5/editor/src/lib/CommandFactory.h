#ifndef COMMANDFACTORY_H
#define COMMANDFACTORY_H

#include "./Command/Command.h"
#include "lib/Command/Mergeable.h"
#include "lib/Command/Unexecutable.h"
#include <cstddef>
#include <memory>
#include <regex>
#include <string>

class ICommandFactory
{
public:
	virtual ~ICommandFactory() = default;
	virtual std::unique_ptr<ICommand> CreateCommand() = 0;
	virtual bool CanCreateCommand() = 0;
	virtual void AppendCommand(
		const std::string& name, std::unique_ptr<ICommand> command)
		= 0;
};

class AbstractCommandFactory : public ICommandFactory
{
protected:
	AbstractCommandFactory(std::shared_ptr<IDocument> document)
		: m_document(document)
	{
	}

	std::shared_ptr<IDocument> m_document;
};

class DocumentCommandFactory : public AbstractCommandFactory
{
public:
	DocumentCommandFactory(std::istream& stream, std::shared_ptr<IDocument> document)
		: AbstractCommandFactory(document)
		, m_stream(&stream)
	{
		InitializeCommands();
	}

	std::unique_ptr<ICommand> CreateCommand() override
	{
		std::string line;
		if (!std::getline(*m_stream, line))
		{
			return nullptr;
		}

		std::istringstream iss(line);
		std::string commandName;
		iss >> commandName;

		std::string arguments;
		std::getline(iss, arguments);
		arguments = std::regex_replace(arguments, std::regex("^\\s+"), "");

		auto macroIt = m_macroCommands.find(commandName);
		if (macroIt != m_macroCommands.end())
		{
			return CreateMacroCommand(macroIt->second);
		}

		auto commandCreatorIt = m_commandCreators.find(commandName);
		if (commandCreatorIt == m_commandCreators.end())
		{
			throw std::invalid_argument("Unknown command: " + commandName);
		}

		return commandCreatorIt->second(arguments);
	}

	bool CanCreateCommand() override
	{
		return m_stream->good();
	}

	void AppendCommand(
		const std::string& name, std::unique_ptr<ICommand> command) override
	{
		m_macroCommands[name].push_back(std::move(command));
	}

private:
	void InitializeCommands()
	{
		m_commandCreators["InsertParagraph"] = [this](const std::string& args) {
			return CreateInsertParagraph(args);
		};
		m_commandCreators["InsertImage"] = [this](const std::string& args) {
			return CreateInsertImage(args);
		};
		m_commandCreators["SetTitle"]
			= [this](const std::string& args) { return CreateSetTitle(args); };
		m_commandCreators["List"]
			= [this](const std::string& args) { return CreateList(); };
		m_commandCreators["ReplaceText"] = [this](const std::string& args) {
			return CreateReplaceText(args);
		};
		m_commandCreators["ResizeImage"] = [this](const std::string& args) {
			return CreateResizeImage(args);
		};
		m_commandCreators["DeleteItem"] = [this](const std::string& args) {
			return CreateDeleteItem(args);
		};
		m_commandCreators["Help"]
			= [this](const std::string& args) { return CreateHelp(); };
		m_commandCreators["Undo"]
			= [this](const std::string& args) { return CreateUndo(); };
		m_commandCreators["Redo"]
			= [this](const std::string& args) { return CreateRedo(); };
		m_commandCreators["Save"]
			= [this](const std::string& args) { return CreateSave(args); };
	}

	std::unique_ptr<ICommand> CreateInsertParagraph(
		const std::string& arguments)
	{
		std::regex pattern(R"((\d+|end)\s+(.+))");
		std::smatch match;
		if (!std::regex_match(arguments, match, pattern))
		{
			throw std::invalid_argument("Invalid InsertParagraph arguments");
		}

		std::optional<size_t> position;
		if (match[1] != "end")
		{
			position = std::stoul(match[1]);
		}

		return std::make_unique<InsertParagraphCommand>(
			m_document, position, match[2]);
	}

	std::unique_ptr<ICommand> CreateInsertImage(const std::string& arguments)
	{
		std::regex pattern(R"((\d+|end)\s+(\d+)\s+(\d+)\s+(.+))");
		std::smatch match;
		if (!std::regex_match(arguments, match, pattern))
		{
			throw std::invalid_argument("Invalid InsertImage arguments");
		}

		std::optional<size_t> position;
		if (match[1] != "end")
		{
			position = std::stoul(match[1]);
		}

		int width = std::stoi(match[2]);
		int height = std::stoi(match[3]);
		std::string path = match[4];

		return std::make_unique<InsertImageCommand>(
			m_document, position, path, width, height);
	}

	std::unique_ptr<ICommand> CreateSetTitle(const std::string& arguments)
	{
		if (arguments.empty())
		{
			throw std::invalid_argument("SetTitle requires title argument");
		}
		return std::make_unique<SetTitleCommand>(m_document, arguments);
	}

	std::unique_ptr<ICommand> CreateList()
	{
		return std::make_unique<ListCommand>(m_document);
	}

	std::unique_ptr<ICommand> CreateReplaceText(const std::string& arguments)
	{
		std::regex pattern(R"((\d+)\s+(.+))");
		std::smatch match;
		if (!std::regex_match(arguments, match, pattern))
		{
			throw std::invalid_argument("Invalid ReplaceText arguments");
		}

		size_t position = std::stoul(match[1]);
		return std::make_unique<ReplaceTextCommand>(
			m_document, position, match[2]);
	}

	std::unique_ptr<ICommand> CreateResizeImage(const std::string& arguments)
	{
		std::regex pattern(R"((\d+)\s+(\d+)\s+(\d+))");
		std::smatch match;
		if (!std::regex_match(arguments, match, pattern))
		{
			throw std::invalid_argument("Invalid ResizeImage arguments");
		}

		size_t position = std::stoul(match[1]);
		int width = std::stoi(match[2]);
		int height = std::stoi(match[3]);
		return std::make_unique<ResizeImageCommand>(
			m_document, position, width, height);
	}

	std::unique_ptr<ICommand> CreateDeleteItem(const std::string& arguments)
	{
		std::regex pattern(R"((\d+))");
		std::smatch match;
		if (!std::regex_match(arguments, match, pattern))
		{
			throw std::invalid_argument("Invalid DeleteItem arguments");
		}

		size_t position = std::stoul(match[1]);
		return std::make_unique<DeleteItemCommand>(m_document, position);
	}

	std::unique_ptr<ICommand> CreateHelp()
	{
		return std::make_unique<HelpCommand>();
	}

	std::unique_ptr<ICommand> CreateUndo()
	{
		return std::make_unique<UndoCommand>(m_document);
	}

	std::unique_ptr<ICommand> CreateRedo()
	{
		return std::make_unique<RedoCommand>(m_document);
	}

	std::unique_ptr<ICommand> CreateSave(const std::string& arguments)
	{
		if (arguments.empty())
		{
			throw std::invalid_argument("Save requires path argument");
		}
		return std::make_unique<SaveCommand>(m_document, arguments);
	}

	std::unique_ptr<ICommand> CreateMacroCommand(
		const std::vector<std::unique_ptr<ICommand>>& commands)
	{
		// TODO: дописать реализацию для кастомных команд
		return nullptr;
	}

	std::istream* m_stream;
	std::unordered_map<std::string,
		std::function<std::unique_ptr<ICommand>(const std::string&)>>
		m_commandCreators;
	std::unordered_map<std::string, std::vector<std::unique_ptr<ICommand>>>
		m_macroCommands;
};

#endif /* COMMANDFACTORY_H */
