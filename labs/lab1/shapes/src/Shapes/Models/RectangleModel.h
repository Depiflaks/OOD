#ifndef RECTANGLEMODEL_H
#define RECTANGLEMODEL_H

#include "IShapeModel.h"

namespace shapes
{

class RectangleModel : public IShapeModel
{
public:
	RectangleModel(Bounds bounds)
		: m_bounds(bounds)
	{
	}

	Bounds GetBounds() override
	{
		return m_bounds;
	}

	void Draw(gfx::ICanvas& canvas, const Color& fillColor) override
	{
		canvas.SetColor(fillColor);
		canvas.MoveTo(m_bounds.position.x, m_bounds.position.y);
		canvas.LineTo(m_bounds.position.x + m_bounds.size.w, m_bounds.position.y);
		canvas.LineTo(m_bounds.position.x + m_bounds.size.w, m_bounds.position.y + m_bounds.size.h);
		canvas.LineTo(m_bounds.position.x, m_bounds.position.y + m_bounds.size.h);
		canvas.LineTo(m_bounds.position.x, m_bounds.position.y);
	}

	void Move(double dx, double dy) override
	{
		m_bounds.position.x += dx;
		m_bounds.position.y += dy;
	}

private:
	Bounds m_bounds;
};

} // namespace shapes

#endif /* RECTANGLEMODEL_H */
