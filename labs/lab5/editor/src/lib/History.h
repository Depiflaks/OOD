#ifndef HISTORY_H
#define HISTORY_H

#include "Command/Mergeable.h"
#include "Invoker.h"
#include <iostream>
#include <list>
#include <memory>
#include <utility>

class IHistoryManager
{
public:
	virtual ~IHistoryManager() = default;
	virtual void Undo() = 0;
	virtual void Redo() = 0;
	virtual bool CanUndo() const = 0;
	virtual bool CanRedo() const = 0;
};

class CommandHistoryManager
	: public IHistoryManager
	, public ICommandManager
{
public:
	CommandHistoryManager()
	{
		m_currentActionIndex = m_commands.end();
	}

	void Undo() override
	{
		if (!CanUndo())
		{
			throw std::runtime_error("Cannot undo");
		}

		--m_currentActionIndex;
		auto unexecutableCommand = *m_currentActionIndex;
		(*m_currentActionIndex)->Unexecute();
	}

	void Redo() override
	{
		if (!CanRedo())
		{
			throw std::runtime_error("Cannot redo");
		}

		(*m_currentActionIndex)->Execute();
		++m_currentActionIndex;
	}

	bool CanUndo() const override
	{
		return m_currentActionIndex != m_commands.begin();
	}

	bool CanRedo() const override
	{
		return m_currentActionIndex != m_commands.end();
	}

	void ExecuteAndAddCommand(std::shared_ptr<ICommand> command) override
	{
		auto unexecutableCommand
			= std::dynamic_pointer_cast<UnexecutableCommand>(command);

		if (!unexecutableCommand)
		{
			command->Execute();
			return;
		}

		auto mergableCommand
			= std::dynamic_pointer_cast<MergableCommand>(unexecutableCommand);
		if (mergableCommand && CanUndo())
		{
			auto lastCommand = std::dynamic_pointer_cast<MergableCommand>(
				*std::prev(m_currentActionIndex));
			if (lastCommand && lastCommand->TryReplace(mergableCommand))
			{
				return;
			}
		}

		if (m_currentActionIndex != m_commands.end())
		{
			for (auto it = std::prev(m_commands.end());;)
			{
				auto toErase = it;
				if (it == m_currentActionIndex)
				{
					m_commands.erase(toErase);
					break;
				}
				--it;
				m_commands.erase(toErase);
			}
		}

		m_commands.push_back(unexecutableCommand);
		m_currentActionIndex = m_commands.end();
		unexecutableCommand->Execute();
	}

private:
	std::list<std::shared_ptr<UnexecutableCommand>> m_commands;
	std::list<std::shared_ptr<UnexecutableCommand>>::iterator
		m_currentActionIndex;
};

#endif /* HISTORY_H */
