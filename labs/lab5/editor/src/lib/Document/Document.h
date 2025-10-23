#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "DocumentItems.h"
#include <optional>
#include <filesystem>

namespace fs = std::filesystem;

class IDocument
{
public:
	virtual ~IDocument() = default;
	virtual std::shared_ptr<IParagraph> InsertParagraph(
		const std::string& text, std::optional<size_t> position = std::nullopt)
		= 0;
	virtual std::shared_ptr<IImage> InsertImage(const fs::path& path, int width,
		int height, std::optional<size_t> position = std::nullopt)
		= 0;
	virtual size_t GetItemsCount() const = 0;
	virtual ConstDocumentItem GetItem(size_t index) const = 0;
	virtual DocumentItem GetItem(size_t index) = 0;
	virtual void DeleteItem(size_t index) = 0;
	virtual std::string GetTitle() const = 0;
	virtual void SetTitle(const std::string& title) = 0;
	virtual bool CanUndo() const = 0;
	virtual void Undo() = 0;
	virtual bool CanRedo() const = 0;
	virtual void Redo() = 0;
	virtual void Save(const fs::path& path) = 0;
};

#endif /* DOCUMENT_H */
