#ifndef DOCUMENTITEMS_H
#define DOCUMENTITEMS_H

#include <memory>
#include <string>

class IParagraph
{
public:
	virtual ~IParagraph() = default;
	virtual std::string GetText() const = 0;
	virtual void SetText(const std::string& text) = 0;
};

class IImage
{
public:
	virtual ~IImage() = default;
	virtual std::string GetPath() const = 0;
	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
	virtual void Resize(int width, int height) = 0;
};

class Paragraph : public IParagraph
{
public:
	Paragraph(std::string text)
		: m_text(text)
	{
	}

	std::string GetText() const override
	{
		return m_text;
	}

	void SetText(const std::string& text) override
	{
		m_text = text;
	}

private:
	std::string m_text;
};

class Image : public IImage
{
public:
	Image(std::string path, int width, int height)
		: m_path(path)
		, m_width(width)
		, m_height(height)
	{
	}

	std::string GetPath() const override
	{
		return m_path;
	}

	int GetWidth() const override
	{
		return m_width;
	}

	int GetHeight() const override
	{
		return m_height;
	}

	void Resize(int width, int height) override
	{
		m_width = width;
		m_height = height;
	}

private:
	std::string m_path;
	int m_width = 0;
	int m_height = 0;
};

class ConstDocumentItem
{
public:
	ConstDocumentItem(std::shared_ptr<IImage> image,
		std::shared_ptr<IParagraph> paragraph, bool isDeleted = false)
		: m_image(image)
		, m_paragraph(paragraph)
		, m_isDeleted(isDeleted)
	{
	}

	std::shared_ptr<IImage> GetImage() const
	{
		return m_image;
	}

	std::shared_ptr<IParagraph> GetParagraph() const
	{
		return m_paragraph;
	}

	bool IsDeleted() const
	{
		return m_isDeleted;
	}

private:
	std::shared_ptr<IParagraph> m_paragraph;
	std::shared_ptr<IImage> m_image;
	const bool m_isDeleted = false;
};

class DocumentItem
{
public:
	DocumentItem(std::shared_ptr<IParagraph> paragraph, bool isDeleted = false)
		: m_paragraph(paragraph)
		, m_isDeleted(isDeleted)
	{
	}

	DocumentItem(std::shared_ptr<IImage> image, bool isDeleted = false)
		: m_image(image)
		, m_isDeleted(isDeleted)
	{
	}

	std::shared_ptr<IImage> GetImage()
	{
		return m_image;
	}

	std::shared_ptr<IParagraph> GetParagraph()
	{
		return m_paragraph;
	}

	bool IsDeleted() const
	{
		return m_isDeleted;
	}

	void SetIsDeleted(bool isDeleted)
	{
		m_isDeleted = isDeleted;
	}

private:
	std::shared_ptr<IParagraph> m_paragraph;
	std::shared_ptr<IImage> m_image;
	bool m_isDeleted = false;
};

#endif /* DOCUMENTITEMS_H */
