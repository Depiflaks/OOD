#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "../History.h"
#include "DocumentItems.h"

namespace fs = std::filesystem;

class IDocument
{
public:
	virtual std::shared_ptr<IParagraph> InsertParagraph(
		const std::string& text, std::optional<size_t> position = std::nullopt)
		= 0;
	virtual std::shared_ptr<IImage> InsertImage(const fs::path& path, int width,
		int height, std::optional<size_t> position = std::nullopt)
		= 0;
	virtual size_t GetItemsCount() const = 0;
	virtual ConstDocumentItem GetItem(size_t index) const = 0;
	virtual DocumentItem& GetItem(size_t index) = 0;
	virtual void DeleteItem(size_t index) = 0;
	virtual std::string GetTitle() const = 0;
	virtual void SetTitle(const std::string& title) = 0;
	virtual bool CanUndo() const = 0;
	virtual void Undo() = 0;
	virtual bool CanRedo() const = 0;
	virtual void Redo() = 0;
	virtual void Save(const fs::path& path) const = 0;
	virtual ~IDocument() = default;
};

class HtmlDocument : public IDocument
{
public:
	HtmlDocument(IHistoryManager& manager)
		: m_historyManager(manager)
	{
	}

	std::shared_ptr<IParagraph> InsertParagraph(const std::string& text,
		std::optional<size_t> position = std::nullopt) override
	{
		size_t pos = position.value_or(m_items.size());
		ValidateInsertPosition(pos);

		auto paragraph = std::make_shared<Paragraph>(text);

		auto documentItem = std::make_shared<DocumentItem>(paragraph);
		m_items.insert(m_items.begin() + pos, documentItem);

		return paragraph;
	}

	std::shared_ptr<IImage> InsertImage(const fs::path& path, int width,
		int height, std::optional<size_t> position = std::nullopt) override
	{
		ValidateImageDimensions(width, height);

		size_t pos = position.value_or(m_items.size());
		ValidateInsertPosition(pos);

		fs::path newPath = CopyImageToTmp(path);

		auto image = std::make_shared<Image>(
			newPath.filename().string(), width, height);

		auto documentItem = std::make_shared<DocumentItem>(image);
		m_items.insert(m_items.begin() + pos, documentItem);

		return image;
	}

	size_t GetItemsCount() const override
	{
		return m_items.size();
	}

	ConstDocumentItem GetItem(size_t index) const override
	{
		ValidateItemIndex(index);
		auto item = m_items[index];
		return ConstDocumentItem(
			item->GetImage(), item->GetParagraph(), item->IsDeleted());
	}

	DocumentItem& GetItem(size_t index) override
	{
		ValidateItemIndex(index);
		return *m_items[index];
	}

	void DeleteItem(size_t index) override
	{
		auto item = m_items[index];
		if (auto image = item->GetImage())
		{
			fs::path imagePath = "tmp" / fs::path(image->GetPath());
			if (fs::exists(imagePath))
			{
				fs::remove(imagePath);
			}
		}
		ValidateItemIndex(index);
		m_items.erase(m_items.begin() + index);
	}

	std::string GetTitle() const override
	{
		return m_title;
	}

	void SetTitle(const std::string& title) override
	{
		m_title = title;
	}

	bool CanUndo() const override
	{
		return m_historyManager.CanUndo();
	}

	void Undo() override
	{
		m_historyManager.Undo();
	}

	bool CanRedo() const override
	{
		return m_historyManager.CanRedo();
	}

	void Redo() override
	{
		m_historyManager.Redo();
	}

	void Save(const fs::path& path) const override
	{
		std::ofstream file(path);
		if (!file.is_open())
		{
			throw std::runtime_error("Cannot open file for saving");
		}

		file << "<!DOCTYPE html>\n<html>\n<head>\n<title>";
		file << HtmlEncode(m_title);
		file << "</title>\n</head>\n<body>\n<h1>";
		file << HtmlEncode(m_title);
		file << "</h1>\n";

		for (size_t i = 0; i < m_items.size(); ++i)
		{
			auto item = m_items[i];
			if (auto paragraph = item->GetParagraph())
			{
				file << "<p>" << HtmlEncode(paragraph->GetText()) << "</p>\n";
			}
			else if (auto image = item->GetImage())
			{
				file << "<img src=\"tmp/" << image->GetPath()
					 << "\" width=\"" << image->GetWidth() << "\" height=\""
					 << image->GetHeight() << "\" />\n";
			}
		}

		file << "</body>\n</html>";
	}

private:
	std::vector<std::shared_ptr<DocumentItem>> m_items;
	std::string m_title;
	IHistoryManager& m_historyManager;

	std::string HtmlEncode(const std::string& text) const
	{
		std::stringstream encoded;
		for (char c : text)
		{
			switch (c)
			{
			case '<':
				encoded << "&lt;";
				break;
			case '>':
				encoded << "&gt;";
				break;
			case '"':
				encoded << "&quot;";
				break;
			case '\'':
				encoded << "&apos;";
				break;
			case '&':
				encoded << "&amp;";
				break;
			default:
				encoded << c;
			}
		}
		return encoded.str();
	}

	fs::path CopyImageToTmp(const fs::path& path)
	{
		fs::create_directory("tmp");
		std::string imageId = GenerateUniqueId();
		fs::path newPath
			= "tmp" / fs::path(imageId + path.extension().string());
		fs::copy_file(path, newPath);
		return newPath;
	}

	void ValidateInsertPosition(size_t position) const
	{
		if (position > m_items.size())
		{
			throw std::runtime_error("Invalid position");
		}
	}

	void ValidateItemIndex(size_t index) const
	{
		if (index >= m_items.size())
		{
			throw std::out_of_range("Index out of range");
		}
	}

	void ValidateImageDimensions(int width, int height) const
	{
		if (width < 1 || width > 10000 || height < 1 || height > 10000)
		{
			throw std::runtime_error("Invalid image dimensions");
		}
	}

	std::string GenerateUniqueId() const
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 15);
		std::stringstream ss;
		for (int i = 0; i < 16; ++i)
		{
			ss << std::hex << dis(gen);
		}
		return ss.str();
	}
};
#endif /* DOCUMENT_H */
