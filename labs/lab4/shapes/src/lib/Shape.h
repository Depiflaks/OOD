#ifndef SHAPE_H
#define SHAPE_H

#include "Canvas.h"
#include "Color.h"
#include "Geometry.h"
#include <cmath>
#include <vector>

class Shape
{
public:
	virtual ~Shape() = default;
	virtual void Draw(ICanvas& canvas) const = 0;
	virtual Color GetColor() const = 0;
};

class Rectangle : public Shape
{
public:
	Rectangle(const Point& leftTop, const Point& rightBottom, const Color& color)
		: m_leftTop(leftTop)
		, m_rightBottom(rightBottom)
		, m_color(color)
	{
	}

	void Draw(ICanvas& canvas) const override
	{
		canvas.SetColor(m_color);
		canvas.MoveTo(m_leftTop.x, m_leftTop.y);
		canvas.LineTo(m_rightBottom.x, m_leftTop.y);
		canvas.LineTo(m_rightBottom.x, m_rightBottom.y);
		canvas.LineTo(m_leftTop.x, m_rightBottom.y);
		canvas.LineTo(m_leftTop.x, m_leftTop.y);
	}

	Color GetColor() const override
	{
		return m_color;
	}

	Point GetLeftTop() const
	{
		return m_leftTop;
	}

	Point GetRightBottom() const
	{
		return m_rightBottom;
	}

private:
	Point m_leftTop;
	Point m_rightBottom;
	Color m_color;
};

class Triangle : public Shape
{
public:
	Triangle(const Point& v1, const Point& v2, const Point& v3, const Color& color)
		: m_vertex1(v1)
		, m_vertex2(v2)
		, m_vertex3(v3)
		, m_color(color)
	{
	}

	void Draw(ICanvas& canvas) const override
	{
		canvas.SetColor(m_color);
		canvas.MoveTo(m_vertex1.x, m_vertex1.y);
		canvas.LineTo(m_vertex2.x, m_vertex2.y);
		canvas.LineTo(m_vertex3.x, m_vertex3.y);
		canvas.LineTo(m_vertex1.x, m_vertex1.y);
	}

	Color GetColor() const override
	{
		return m_color;
	}

	Point GetVertex1() const
	{
		return m_vertex1;
	}

	Point GetVertex2() const
	{
		return m_vertex2;
	}

	Point GetVertex3() const
	{
		return m_vertex3;
	}

private:
	Point m_vertex1;
	Point m_vertex2;
	Point m_vertex3;
	Color m_color;
};

class Ellipse : public Shape
{
public:
	Ellipse(const Point& center, double horizontalRadius, double verticalRadius, const Color& color)
		: m_center(center)
		, m_horizontalRadius(horizontalRadius)
		, m_verticalRadius(verticalRadius)
		, m_color(color)
	{
	}

	void Draw(ICanvas& canvas) const override
	{
		canvas.SetColor(m_color);
		canvas.DrawEllipse(m_center.x, m_center.y, m_horizontalRadius, m_verticalRadius);
	}

	Color GetColor() const override
	{
		return m_color;
	}

	Point GetCenter() const
	{
		return m_center;
	}

	double GetHorizontalRadius() const
	{
		return m_horizontalRadius;
	}

	double GetVerticalRadius() const
	{
		return m_verticalRadius;
	}

private:
	Point m_center;
	double m_horizontalRadius;
	double m_verticalRadius;
	Color m_color;
};

class RegularPolygon : public Shape
{
public:
	RegularPolygon(const Point& center, double radius, int vertexCount, const Color& color)
		: m_center(center)
		, m_radius(radius)
		, m_vertexCount(vertexCount)
		, m_color(color)
	{
	}

	void Draw(ICanvas& canvas) const override
	{
		canvas.SetColor(m_color);

		std::vector<Point> vertices;
		for (int i = 0; i < m_vertexCount; ++i)
		{
			double angle = 2 * M_PI * i / m_vertexCount;
			double x = m_center.x + m_radius * cos(angle);
			double y = m_center.y + m_radius * sin(angle);
			vertices.push_back({ x, y });
		}

		if (!vertices.empty())
		{
			canvas.MoveTo(vertices[0].x, vertices[0].y);
			for (size_t i = 1; i < vertices.size(); ++i)
			{
				canvas.LineTo(vertices[i].x, vertices[i].y);
			}
			canvas.LineTo(vertices[0].x, vertices[0].y);
		}
	}

	Color GetColor() const override
	{
		return m_color;
	}

	Point GetCenter() const
	{
		return m_center;
	}

	double GetRadius() const
	{
		return m_radius;
	}

	int GetVertexCount() const
	{
		return m_vertexCount;
	}

private:
	Point m_center;
	double m_radius;
	int m_vertexCount;
	Color m_color;
};

#endif /* SHAPE_H */
