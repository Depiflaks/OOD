//
// Created by smmm on 06.05.2025.
//

#include "SolidShape.h"

SolidShape::SolidShape()
	: Shape(k_black)
	, m_fillColor(k_empty)
{
}

SolidShape::SolidShape(const Color outlineColor)
	: Shape(outlineColor)
	, m_fillColor(k_empty)
{
}

SolidShape::SolidShape(const Color outlineColor, const Color fillColor)
	: Shape(outlineColor)
	, m_fillColor(fillColor)
{
}

Color SolidShape::GetFillColor() const
{
	return m_fillColor;
}

void SolidShape::SetFillColor(const Color color)
{
	m_fillColor = color;
}
