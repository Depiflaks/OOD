#ifndef CIRCLEMODEL_H
#define CIRCLEMODEL_H

#include "IShapeModel.h"

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

	void Draw(ICanvas& canvas, const Color& fillColor) override
	{
		canvas.SetColor(fillColor);
		canvas.DrawEllipse(m_center.x, m_center.y, m_r, m_r);
	}

private:
	Position m_center;
	double m_r;
};

#endif /* CIRCLEMODEL_H */
