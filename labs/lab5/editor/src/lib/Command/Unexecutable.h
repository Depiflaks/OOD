#ifndef UNEXECUTABLE_H
#define UNEXECUTABLE_H

#include "Command.h"

class UnexecutableCommand : public AbstractCommand
{
public:
	bool IsExecuted() const
	{
		return m_executed;
	}

	virtual void Unexecute() = 0;

protected:
	UnexecutableCommand(std::weak_ptr<IDocument> document)
		: AbstractCommand(document)
		, m_executed(false)
	{
	}

	bool m_executed;
};

class SetTitleCommand : public UnexecutableCommand
{
public:
	SetTitleCommand(
		std::weak_ptr<IDocument> document, const std::string& newTitle)
		: UnexecutableCommand(document)
		, m_newTitle(newTitle)
	{
	}

	void Execute() override
	{
		auto& document = GetDocument();
		if (!m_executed)
		{
			m_oldTitle = document.GetTitle();
		}
		document.SetTitle(m_newTitle);
		m_executed = true;
	}

	void Unexecute() override
	{
		if (m_executed)
		{
			auto& document = GetDocument();
			document.SetTitle(m_oldTitle);
		}
	}

private:
	std::string m_newTitle;
	std::string m_oldTitle;
};

class ResizeImageCommand : public UnexecutableCommand
{
public:
	ResizeImageCommand(std::weak_ptr<IDocument> document, size_t position,
		int newWidth, int newHeight)
		: UnexecutableCommand(document)
		, m_position(position)
		, m_newWidth(newWidth)
		, m_newHeight(newHeight)
	{
	}

	void Execute() override
	{
		auto& document = GetDocument();
		auto& item = document.GetItem(m_position);
		auto image = item.GetImage();
		if (!image)
		{
			throw std::runtime_error("No image at specified position");
		}

		if (!m_executed)
		{
			m_oldWidth = image->GetWidth();
			m_oldHeight = image->GetHeight();
		}

		image->Resize(m_newWidth, m_newHeight);
		m_executed = true;
	}

	void Unexecute() override
	{
		if (m_executed)
		{
			auto& document = GetDocument();
			auto& item = document.GetItem(m_position);
			auto image = item.GetImage();
			if (image)
			{
				image->Resize(m_oldWidth, m_oldHeight);
			}
		}
	}

private:
	size_t m_position;
	int m_newWidth;
	int m_newHeight;
	int m_oldWidth;
	int m_oldHeight;
};

class InsertParagraphCommand : public UnexecutableCommand
{
public:
	InsertParagraphCommand(std::weak_ptr<IDocument> document,
		std::optional<size_t> position, const std::string& text)
		: UnexecutableCommand(document)
		, m_position(position)
		, m_text(text)
	{
	}

	~InsertParagraphCommand()
	{
		if (m_executed)
		{
			auto& document = GetDocument();
			size_t pos = m_position.value_or(document.GetItemsCount() - 1);
			auto& item = document.GetItem(pos);
			if (item.IsDeleted())
			{
				document.DeleteItem(pos);
			}
		}
	}

	void Execute() override
	{
		auto& document = GetDocument();
		if (!m_executed)
		{
			document.InsertParagraph(m_text, m_position);
			m_executed = true;
		}
		else
		{
			size_t pos = m_position.value_or(document.GetItemsCount() - 1);
			auto& item = document.GetItem(pos);
			item.SetIsDeleted(false);
		}
	}

	void Unexecute() override
	{
		if (m_executed)
		{
			auto& document = GetDocument();
			size_t pos = m_position.value_or(document.GetItemsCount() - 1);
			auto& item = document.GetItem(pos);
			item.SetIsDeleted(true);
		}
	}

private:
	std::optional<size_t> m_position;
	std::string m_text;
};

class InsertImageCommand : public UnexecutableCommand
{
public:
	InsertImageCommand(std::weak_ptr<IDocument> document,
		std::optional<size_t> position, const fs::path& path, int width,
		int height)
		: UnexecutableCommand(document)
		, m_position(position)
		, m_path(path)
		, m_width(width)
		, m_height(height)
	{
	}

	~InsertImageCommand()
	{
		if (m_executed)
		{
			auto& document = GetDocument();
			size_t pos = m_position.value_or(document.GetItemsCount() - 1);
			auto& item = document.GetItem(pos);
			if (item.IsDeleted())
			{
				document.DeleteItem(pos);
			}
		}
	}

	void Execute() override
	{
		auto& document = GetDocument();
		if (!m_executed)
		{
			document.InsertImage(m_path, m_width, m_height, m_position);
			m_executed = true;
		}
		else
		{
			size_t pos = m_position.value_or(document.GetItemsCount() - 1);
			auto& item = document.GetItem(pos);
			item.SetIsDeleted(false);
		}
	}

	void Unexecute() override
	{
		if (m_executed)
		{
			auto& document = GetDocument();
			size_t pos = m_position.value_or(document.GetItemsCount() - 1);
			auto& item = document.GetItem(pos);
			item.SetIsDeleted(true);
		}
	}

private:
	std::optional<size_t> m_position;
	fs::path m_path;
	int m_width;
	int m_height;
};

class DeleteItemCommand : public UnexecutableCommand
{
public:
	DeleteItemCommand(std::weak_ptr<IDocument> document, size_t position)
		: UnexecutableCommand(document)
		, m_position(position)
	{
	}

	~DeleteItemCommand()
	{
		if (m_executed)
		{
			auto& document = GetDocument();
			auto& item = document.GetItem(m_position);
			if (item.IsDeleted())
			{
				document.DeleteItem(m_position);
			}
		}
	}

	void Execute() override
	{
		auto& document = GetDocument();
		if (!m_executed)
		{
			auto& item = document.GetItem(m_position);
			m_wasDeleted = item.IsDeleted();
		}
		auto& item = document.GetItem(m_position);
		item.SetIsDeleted(true);
		m_executed = true;
	}

	void Unexecute() override
	{
		if (m_executed)
		{
			auto& document = GetDocument();
			auto& item = document.GetItem(m_position);
			item.SetIsDeleted(m_wasDeleted);
		}
	}

private:
	size_t m_position;
	bool m_wasDeleted = false;
};

#endif /* UNEXECUTABLE_H */
