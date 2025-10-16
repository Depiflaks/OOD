//
// Created by smmm on 10.05.2025.
//

#include "Circle.h"

Circle::Circle()
{
}

Circle::Circle(const Point center, const double radius)
	: m_center(center)
	, m_radius(radius)
{
}

Circle::Circle(const Point center, const double radius, const Color outlineColor)
	: SolidShape(outlineColor)
	, m_center(center)
	, m_radius(radius)
{
}

Circle::Circle(const Point center, const double radius, const Color outlineColor, const Color fillColor)
	: SolidShape(outlineColor, fillColor)
	, m_center(center)
	, m_radius(radius)
{
}

double Circle::GetArea() const
{
	return m_radius * m_radius * std::numbers::pi;
}

double Circle::GetPerimeter() const
{
	return 2 * m_radius * std::numbers::pi;
}

std::string Circle::ToString() const
{
	return "Circle";
}

void Circle::Draw(ICanvas& canvas)
{
	canvas.DrawCircle(m_center, m_radius, GetOutlineColor());
	canvas.FillCircle(m_center, m_radius, GetFillColor());
}

void Circle::Print(std::ostream& os) const
{
    os << "Shape: " << ToString() << '\n';
    os << "Center: " << GetCenter() << '\n';
    os << "Radius: " << GetRadius() << '\n';

    if (GetOutlineColor() != k_empty)
        os << "Outline Color(RGBA): #" << GetOutlineColor() << '\n';

    if (GetFillColor() != k_empty)
        os << "Fill Color(RGBA): #" << GetFillColor() << '\n';

    os << "Perimeter: " << GetPerimeter() << '\n';
    os << "Area: " << GetArea() << '\n';
}

std::istream& operator>>(std::istream& is, Circle& circle)
{
	Color outlineColor, fillColor;
	is >> circle.m_center >> circle.m_radius >> outlineColor >> fillColor;
	circle.SetOutlineColor(outlineColor);
	circle.SetFillColor(fillColor);
	return is;
}

Point Circle::GetCenter() const
{
	return m_center;
}

double Circle::GetRadius() const
{
	return m_radius;
}