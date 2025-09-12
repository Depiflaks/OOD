#ifndef CIRCLEMODEL_H
#define CIRCLEMODEL_H

#include "IShapeModel.h"

namespace shapes
{

class CircleModel : public IShapeModel
{
public:
	CircleModel(Position center, double r)
		: m_center(center)
		, m_r(r)
	{
	}

	Bounds GetBounds() override
	{
		return { { m_center.x - m_r, m_center.y - m_r }, { 2 * m_r, 2 * m_r } };
	}

	void Draw(gfx::ICanvas& canvas, const Color& fillColor) override
	{
		canvas.SetColor(fillColor);
		canvas.DrawEllipse(m_center.x, m_center.y, m_r, m_r);
		canvas.Render();
	}

	void Move(double dx, double dy) override
	{
		m_center.x += dx;
		m_center.y += dy;
	}

private:
	Position m_center;
	double m_r;
};

}; // namespace shapes

#endif /* CIRCLEMODEL_H */
