#ifndef LINEMODEL_H
#define LINEMODEL_H

#include "IShapeModel.h"

namespace shapes
{
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

	void Draw(gfx::ICanvas& canvas, const Color& fillColor) override
	{
		canvas.SetColor(fillColor);
		canvas.MoveTo(m_p1.x, m_p1.y);
		canvas.LineTo(m_p2.x, m_p2.y);
		canvas.Render();
	}

	void Move(double dx, double dy) override
	{
		m_p1 = {m_p1.x + dx, m_p1.y + dy};
		m_p2 = {m_p2.x + dx, m_p2.y + dy};
	}

private:
	Position m_p1;
	Position m_p2;
};

}; // namespace shapes

#endif /* LINEMODEL_H */
