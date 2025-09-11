#ifndef TRIANGLEMODEL_H
#define TRIANGLEMODEL_H

#include "IShapeModel.h"
#include <algorithm>

namespace shapes
{

class TriangleModel : public IShapeModel
{
public:
	TriangleModel(Position vertex1, Position vertex2, Position vertex3)
		: m_vertex1(vertex1)
		, m_vertex2(vertex2)
		, m_vertex3(vertex3)
	{
	}

	Bounds GetBounds() override
	{
		double minX = std::min({ m_vertex1.x, m_vertex2.x, m_vertex3.x });
		double minY = std::min({ m_vertex1.y, m_vertex2.y, m_vertex3.y });
		double maxX = std::max({ m_vertex1.x, m_vertex2.x, m_vertex3.x });
		double maxY = std::max({ m_vertex1.y, m_vertex2.y, m_vertex3.y });

		return { { minX, minY }, { maxX - minX, maxY - minY } };
	}

	void Draw(gfx::ICanvas& canvas, const Color& fillColor) override
	{
		canvas.SetColor(fillColor);
		canvas.MoveTo(m_vertex1.x, m_vertex1.y);
		canvas.LineTo(m_vertex2.x, m_vertex2.y);
		canvas.LineTo(m_vertex3.x, m_vertex3.y);
		canvas.LineTo(m_vertex1.x, m_vertex1.y);
	}

	void Move(double dx, double dy) override
	{
		m_vertex1.x += dx;
		m_vertex1.y += dy;
		m_vertex2.x += dx;
		m_vertex2.x += dx;
		m_vertex3.y += dy;
		m_vertex3.y += dy;
	}

private:
	Position m_vertex1;
	Position m_vertex2;
	Position m_vertex3;
};

}; // namespace shapes

#endif /* TRIANGLEMODEL_H */
