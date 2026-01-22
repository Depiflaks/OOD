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
	AbstractCommand(std::weak_ptr<IDocument> document)
		: m_document(document)
	{
	}

	IDocument& TryGetDocument() const
	{
		auto document = m_document.lock();
		if (!document)
		{
			throw std::runtime_error("Document no longer exists");
		}
		return *document;
	}

	std::weak_ptr<IDocument> GetDocument() const
	{
		return m_document;
	}

private:
	std::weak_ptr<IDocument> m_document;
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
	UndoCommand(std::weak_ptr<IDocument> document)
		: AbstractCommand(document)
	{
	}

	void Execute() override
	{
		auto& document = TryGetDocument();
		if (!document.CanUndo())
		{
			throw std::runtime_error("Cannot undo");
		}
		document.Undo();
	}
};

class RedoCommand : public AbstractCommand
{
public:
	RedoCommand(std::weak_ptr<IDocument> document)
		: AbstractCommand(document)
	{
	}

	void Execute() override
	{
		auto& document = TryGetDocument();
		if (!document.CanRedo())
		{
			throw std::runtime_error("Cannot redo");
		}
		document.Redo();
	}
};

class SaveCommand : public AbstractCommand
{
public:
	SaveCommand(std::weak_ptr<IDocument> document, const std::string& path)
		: AbstractCommand(document)
		, m_path(path)
	{
	}

	void Execute() override
	{
		auto& document = TryGetDocument();
		document.Save(m_path);
	}

private:
	std::string m_path;
};

class ListCommand : public AbstractCommand
{
public:
	ListCommand(std::weak_ptr<IDocument> document)
		: AbstractCommand(document)
	{
	}

	void Execute() override
	{
		auto& document = TryGetDocument();
		std::cout << "Title: " << document.GetTitle() << std::endl;

		for (size_t i = 0; i < document.GetItemsCount(); ++i)
		{
			auto item = document.GetItem(i);

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
