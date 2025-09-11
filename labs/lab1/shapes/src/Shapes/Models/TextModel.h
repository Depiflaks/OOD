#ifndef TEXTMODEL_H
#define TEXTMODEL_H

#include "IShapeModel.h"

class TextModel : public IShapeModel
{
public:
	TextModel(Position p, double size, std::string content)
		: m_position(p)
		, m_size(size)
		, m_content(std::move(content))
	{
	}

	Bounds GetBounds() override
	{
		return { m_position, { m_content.length() * m_size * 0.6, m_size } };
	}

	void Draw(ICanvas& canvas, const Color& fillColor) override
	{
		canvas.SetColor(fillColor);
		canvas.DrawText(m_position.x, m_position.y, m_size, m_content);
	}

private:
	Position m_position;
	double m_size;
	std::string m_content;
};

#endif /* TEXTMODEL_H */
