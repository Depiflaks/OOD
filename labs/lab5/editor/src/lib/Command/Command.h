#ifndef COMMAND_H
#define COMMAND_H

#include "../Document/Document.h"
#include <iostream>
#include <memory>
#include <stdexcept>

class ICommand
{
public:
	virtual ~ICommand() = default;
	virtual void Execute() = 0;
};

class AbstractCommand : public ICommand
{
protected:
	AbstractCommand(IDocument& document)
		: m_document(document)
	{
	}

	IDocument& m_document;
};

class HelpCommand : public ICommand
{
public:
	HelpCommand() = default;

	void Execute() override
	{
		std::cout << "Available commands:\n";
		std::cout << "InsertParagraph <position>|end <text>\n";
		std::cout << "InsertImage <position>|end <width> <height> <path>\n";
		std::cout << "SetTitle <title>\n";
		std::cout << "List\n";
		std::cout << "ReplaceText <position> <text>\n";
		std::cout << "ResizeImage <position> <width> <height>\n";
		std::cout << "DeleteItem <position>\n";
		std::cout << "Help\n";
		std::cout << "Undo\n";
		std::cout << "Redo\n";
		std::cout << "Save <path>\n";
	}
};

class UndoCommand : public AbstractCommand
{
public:
	UndoCommand(IDocument& document)
		: AbstractCommand(document)
	{
	}

	void Execute() override
	{
		if (!m_document.CanUndo())
		{
			throw std::runtime_error("Cannot undo");
		}
		m_document.Undo();
	}
};

class RedoCommand : public AbstractCommand
{
public:
	RedoCommand(IDocument& document)
		: AbstractCommand(document)
	{
	}

	void Execute() override
	{
		if (!m_document.CanRedo())
		{
			throw std::runtime_error("Cannot redo");
		}
		m_document.Redo();
	}
};

class SaveCommand : public AbstractCommand
{
public:
	SaveCommand(IDocument& document, const std::string& path)
		: AbstractCommand(document)
		, m_path(path)
	{
	}

	void Execute() override
	{
		m_document.Save(m_path);
	}

private:
	std::string m_path;
};

class ListCommand : public AbstractCommand
{
public:
	ListCommand(IDocument& document)
		: AbstractCommand(document)
	{
	}

	void Execute() override
	{
		std::cout << "Title: " << m_document.GetTitle() << std::endl;

		for (size_t i = 0; i < m_document.GetItemsCount(); ++i)
		{
			auto item = m_document.GetItem(i);

			std::cout << (i + 1) << ". ";

			if (auto paragraph = item.GetParagraph())
			{
				std::cout << "Paragraph: " << paragraph->GetText();
			}
			else if (auto image = item.GetImage())
			{
				std::cout << "Image: " << image->GetWidth() << " "
						  << image->GetHeight() << " " << image->GetPath();
			}

			if (item.IsDeleted())
			{
				std::cout << " [DELETED]";
			}

			std::cout << '\n';
		}
	}
};
#endif /* COMMAND_H */
