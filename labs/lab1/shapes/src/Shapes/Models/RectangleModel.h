#ifndef RECTANGLEMODEL_H
#define RECTANGLEMODEL_H

#include "IShapeModel.h"

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

	void Draw(ICanvas& canvas, const Color& fillColor) override
	{
		canvas.SetColor(fillColor);
		canvas.MoveTo(m_bounds.position.x, m_bounds.position.y);
		canvas.LineTo(m_bounds.position.x + m_bounds.size.w, m_bounds.position.y);
		canvas.LineTo(m_bounds.position.x + m_bounds.size.w, m_bounds.position.y + m_bounds.size.h);
		canvas.LineTo(m_bounds.position.x, m_bounds.position.y + m_bounds.size.h);
		canvas.LineTo(m_bounds.position.x, m_bounds.position.y);
	}

private:
	Bounds m_bounds;
};


#endif /* RECTANGLEMODEL_H */
