#ifndef UNEXECUTABLE_H
#define UNEXECUTABLE_H

#include "Command.h"

class UnexecutableCommand : public AbstractCommand
{
protected:
	UnexecutableCommand(IDocument& document)
		: AbstractCommand(document)
		, m_executed(false)
	{
	}

	virtual void Unexecute() = 0;

	bool IsExecuted() const
	{
		return m_executed;
	}

	bool m_executed;
};

class SetTitleCommand : public UnexecutableCommand
{
public:
	SetTitleCommand(IDocument& document, const std::string& newTitle)
		: UnexecutableCommand(document)
		, m_newTitle(newTitle)
	{
	}

	void Execute() override
	{
		if (!m_executed)
		{
			m_oldTitle = m_document.GetTitle();
		}
		m_document.SetTitle(m_newTitle);
		m_executed = true;
	}

	void Unexecute() override
	{
		if (m_executed)
		{
			m_document.SetTitle(m_oldTitle);
		}
	}

private:
	std::string m_newTitle;
	std::string m_oldTitle;
};

class ResizeImageCommand : public UnexecutableCommand
{
public:
	ResizeImageCommand(
		IDocument& document, size_t position, int newWidth, int newHeight)
		: UnexecutableCommand(document)
		, m_position(position)
		, m_newWidth(newWidth)
		, m_newHeight(newHeight)
	{
	}

	void Execute() override
	{
		auto& item = m_document.GetItem(m_position);
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
			auto& item = m_document.GetItem(m_position);
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
	InsertParagraphCommand(IDocument& document, std::optional<size_t> position,
		const std::string& text)
		: UnexecutableCommand(document)
		, m_position(position)
	{
	}

	~InsertParagraphCommand()
	{
		if (m_executed)
		{
			size_t pos = m_position.value_or(m_document.GetItemsCount() - 1);
			auto& item = m_document.GetItem(pos);
			if (item.IsDeleted())
			{
				m_document.DeleteItem(pos);
			}
		}
	}

	void Execute() override
	{
		if (!m_executed)
		{
			m_document.InsertParagraph(m_text, m_position);
			m_executed = true;
		}
		else
		{
			size_t pos = m_position.value_or(m_document.GetItemsCount() - 1);
			auto& item = m_document.GetItem(pos);
			item.SetIsDeleted(false);
		}
	}

	void Unexecute() override
	{
		if (m_executed)
		{
			size_t pos = m_position.value_or(m_document.GetItemsCount() - 1);
			auto& item = m_document.GetItem(pos);
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
	InsertImageCommand(IDocument& document, std::optional<size_t> position,
		const fs::path& path, int width, int height)
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
			size_t pos = m_position.value_or(m_document.GetItemsCount() - 1);
			auto& item = m_document.GetItem(pos);
			if (item.IsDeleted())
			{
				m_document.DeleteItem(pos);
			}
		}
	}

	void Execute() override
	{
		if (!m_executed)
		{
			m_document.InsertImage(m_path, m_width, m_height, m_position);
			m_executed = true;
		}
		else
		{
			size_t pos = m_position.value_or(m_document.GetItemsCount() - 1);
			auto& item = m_document.GetItem(pos);
			item.SetIsDeleted(false);
		}
	}

	void Unexecute() override
	{
		if (m_executed)
		{
			size_t pos = m_position.value_or(m_document.GetItemsCount() - 1);
			auto& item = m_document.GetItem(pos);
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
	DeleteItemCommand(IDocument& document, size_t position)
		: UnexecutableCommand(document)
		, m_position(position)
	{
	}

	~DeleteItemCommand()
	{
		if (m_executed)
		{
			auto& item = m_document.GetItem(m_position);
			if (item.IsDeleted())
			{
				m_document.DeleteItem(m_position);
			}
		}
	}

	void Execute() override
	{
		if (!m_executed)
		{
			auto& item = m_document.GetItem(m_position);
			m_wasDeleted = item.IsDeleted();
		}
		auto& item = m_document.GetItem(m_position);
		item.SetIsDeleted(true);
		m_executed = true;
	}

	void Unexecute() override
	{
		if (m_executed)
		{
			auto& item = m_document.GetItem(m_position);
			item.SetIsDeleted(m_wasDeleted);
		}
	}

private:
	size_t m_position;
	bool m_wasDeleted = false;
};

#endif /* UNEXECUTABLE_H */
