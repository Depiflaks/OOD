#ifndef MERGEABLE_H
#define MERGEABLE_H
#include "Unexecutable.h"
#include <execution>
#include <iostream>
#include <memory>

class MergableCommand : public UnexecutableCommand
{
protected:
	MergableCommand(std::weak_ptr<IDocument> document)
		: UnexecutableCommand(document)
	{
	}

public:
	virtual bool TryReplace(std::shared_ptr<MergableCommand> command) = 0;
};

class ReplaceTextCommand : public MergableCommand
{
public:
	ReplaceTextCommand(std::weak_ptr<IDocument> document, size_t position,
		const std::string& newText)
		: MergableCommand(document)
		, m_position(position)
		, m_newText(newText)
	{
	}

	void Execute() override
	{
		auto& document = TryGetDocument();
		auto& item = document.GetItem(m_position);
		auto paragraph = item.GetParagraph();
		if (!paragraph)
		{
			throw std::runtime_error("No paragraph at specified position");
		}

		if (!m_executed)
		{
			m_oldText = paragraph->GetText();
		}

		paragraph->SetText(m_newText);
		m_executed = true;
	}

	void Unexecute() override
	{
		if (m_executed)
		{
			auto& document = TryGetDocument();
			auto& item = document.GetItem(m_position);
			auto paragraph = item.GetParagraph();
			if (paragraph)
			{
				paragraph->SetText(m_oldText);
			}
		}
	}

	bool TryReplace(std::shared_ptr<MergableCommand> command) override
	{
		auto other = std::dynamic_pointer_cast<ReplaceTextCommand>(command);
		if (!other)
		{
			return false;
		}

		if (m_position != other->m_position)
		{
			return false;
		}

		m_newText = other->m_newText;

		auto& document = TryGetDocument();
		auto& item = document.GetItem(m_position);
		auto paragraph = item.GetParagraph();
		if (paragraph)
		{
			paragraph->SetText(m_newText);
			return true;
		}
		return false;
		// доработать, чтобы Undo прерывало склеивание
	}

private:
	size_t m_position;
	std::string m_newText;
	std::string m_oldText;
};

#endif /* MERGEABLE_H */
