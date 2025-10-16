//
// Created by smmm on 06.05.2025.
//

#include "LineSegment.h"

LineSegment::LineSegment(const Point start, const Point end)
	: m_startPoint(start)
	, m_endPoint(end)
{
}

LineSegment::LineSegment(const Point start, const Point end, const Color outlineColor)
	: Shape(outlineColor)
	, m_startPoint(start)
	, m_endPoint(end)
{
}

double LineSegment::GetArea() const
{
	return 0;
}

double LineSegment::GetPerimeter() const
{
	return Point::Distance(m_startPoint, m_endPoint);
}

std::string LineSegment::ToString() const
{
	return "lineSegment";
}

void LineSegment::Draw(ICanvas& canvas)
{
	canvas.DrawLine(m_startPoint, m_endPoint, GetOutlineColor());
}

void LineSegment::Print(std::ostream& os) const
{
    os << "Shape: " << ToString() << '\n';
    os << "Start Point: " << GetStartPoint() << '\n';
    os << "End Point: " << GetEndPoint() << '\n';
    os << "Outline Color(RGBA): #" << GetOutlineColor() << '\n';
    os << "Perimeter: " << GetPerimeter() << '\n';
}

std::istream& operator>>(std::istream& is, LineSegment& lineSegment)
{
	Color outlineColor;
	is >> lineSegment.m_startPoint >> lineSegment.m_endPoint >> outlineColor;
	lineSegment.SetOutlineColor(outlineColor);
	return is;
}

Point LineSegment::GetStartPoint() const
{
	return m_startPoint;
}

Point LineSegment::GetEndPoint() const
{
	return m_endPoint;
}