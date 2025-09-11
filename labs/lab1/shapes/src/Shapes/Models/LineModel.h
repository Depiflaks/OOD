#ifndef LINEMODEL_H
#define LINEMODEL_H

#include "IShapeModel.h"

class LineModel : public IShapeModel
{
public:
	LineModel(Position p1, Position p2)
		: m_p1(p1)
		, m_p2(p2)
	{
	}

	Bounds GetBounds() override
	{
		double minX = std::min(m_p1.x, m_p2.x);
		double minY = std::min(m_p1.y, m_p2.y);
		double maxX = std::max(m_p1.x, m_p2.x);
		double maxY = std::max(m_p1.y, m_p2.y);

		return { { minX, minY }, { maxX - minX, maxY - minY } };
	}

	void Draw(ICanvas& canvas, const Color& fillColor) override
	{
		canvas.SetColor(fillColor);
		canvas.MoveTo(m_p1.x, m_p1.y);
		canvas.LineTo(m_p2.x, m_p2.y);
	}

private:
	Position m_p1;
	Position m_p2;
};
;

#endif /* LINEMODEL_H */
