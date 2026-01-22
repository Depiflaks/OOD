//
// Created by smmm on 06.05.2025.
//

#include "Rectangle.h"

Rectangle::Rectangle(const Point leftTop, const double width, const double height)
	: m_leftTop(leftTop)
	, m_width(width)
	, m_height(height)
{
}

Rectangle::Rectangle(const Point leftTop, const double width, const double height, const Color outlineColor)
	: SolidShape(outlineColor)
	, m_leftTop(leftTop)
	, m_width(width)
	, m_height(height)
{
}

Rectangle::Rectangle(const Point leftTop, const double width, const double height, const Color outlineColor, const Color fillColor)
	: SolidShape(outlineColor, fillColor)
	, m_leftTop(leftTop)
	, m_width(width)
	, m_height(height)
{
}

void Rectangle::Draw(ICanvas& canvas)
{
	canvas.DrawPolygon({
						   m_leftTop,
						   { m_leftTop.x + m_width, m_leftTop.y },
						   { m_leftTop.x + m_width, m_leftTop.y + m_height },
						   { m_leftTop.x, m_leftTop.y + m_height },
					   },
		GetOutlineColor());
	canvas.FillPolygon({
						   m_leftTop,
						   { m_leftTop.x + m_width, m_leftTop.y },
						   { m_leftTop.x + m_width, m_leftTop.y + m_height },
						   { m_leftTop.x, m_leftTop.y + m_height },
					   },
		GetFillColor());
}

double Rectangle::GetArea() const
{
	return m_width * m_height;
}

double Rectangle::GetPerimeter() const
{
	return 2 * (m_width + m_height);
}

std::string Rectangle::ToString() const
{
	return "Rectangle";
}

void Rectangle::Print(std::ostream& os) const
{
    os << "Shape: " << ToString() << '\n';
    os << "Left Top: " << GetLeftTop() << '\n';
    os << "Right Bottom: " << GetRightBottom() << '\n';
    os << "Width: " << GetWidth() << '\n';
    os << "Height: " << GetHeight() << '\n';

    if (GetOutlineColor() != k_empty)
        os << "Outline Color(RGBA): #" << GetOutlineColor() << '\n';

    if (GetFillColor() != k_empty)
        os << "Fill Color(RGBA): #" << GetFillColor() << '\n';

    os << "Perimeter: " << GetPerimeter() << '\n';
    os << "Area: " << GetArea() << '\n';
}

std::istream& operator>>(std::istream& is, Rectangle& rectangle)
{
	Color outlineColor, fillColor;
	is >> rectangle.m_leftTop >> rectangle.m_width >> rectangle.m_height >> outlineColor >> fillColor;
	rectangle.SetOutlineColor(outlineColor);
	rectangle.SetFillColor(fillColor);
	return is;
}

Point Rectangle::GetLeftTop() const
{
	return m_leftTop;
}

Point Rectangle::GetRightBottom() const
{
	return { m_leftTop.x + m_width, m_leftTop.y + m_height };
}

double Rectangle::GetWidth() const
{
	return m_width;
}

double Rectangle::GetHeight() const
{
	return m_height;
}