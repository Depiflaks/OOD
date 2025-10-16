#ifndef SHAPE_H
#define SHAPE_H

#include "Canvas.h"
#include "Color.h"
#include "Geometry.h"
#include <cmath>
#include <memory>
#include <utility>
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
		canvas.DrawFilledRectangle(m_leftTop.x, m_leftTop.y, m_rightBottom.x, m_rightBottom.y);
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
		canvas.DrawFilledTriangle(m_vertex1.x, m_vertex1.y, m_vertex2.x, m_vertex2.y, m_vertex3.x, m_vertex3.y);
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

		std::vector<std::pair<double, double>> points;
		for (int i = 0; i < m_vertexCount; ++i)
		{
			double angle = 2 * M_PI * i / m_vertexCount;
			double x = m_center.x + m_radius * cos(angle);
			double y = m_center.y + m_radius * sin(angle);
			points.push_back({x, y});
		}

		canvas.DrawFilledRegularPolygon(points);
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

class PictureDraft
{
public:
	PictureDraft() = default;

	PictureDraft(std::vector<std::unique_ptr<Shape>>&& shapes)
	{
		for (auto& shape : shapes)
		{
			m_shapes.push_back(std::move(shape));
		}
	}

	size_t GetShapeCount() const
	{
		return m_shapes.size();
	}

	Shape& GetShape(size_t index)
	{
		if (index >= m_shapes.size())
		{
			throw std::out_of_range("Index out of range");
		}
		return *m_shapes[index];
	}

	void AppendShape(std::unique_ptr<Shape> shape)
	{
		m_shapes.push_back(std::move(shape));
	}

private:
	std::vector<std::unique_ptr<Shape>> m_shapes;
};

#endif /* SHAPE_H */
