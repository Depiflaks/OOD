//
// Created by smmm on 06.05.2025.
//

#ifndef SOLID_SHAPE_H
#define SOLID_SHAPE_H
#include "canvas/ICanvas.h"
#include "shape/Shape.h"

class SolidShape : public Shape
{
public:
	explicit SolidShape(Color outlineColor, Color fillColor);
	explicit SolidShape(Color outlineColor);
	explicit SolidShape();

	[[nodiscard]] Color GetFillColor() const;
	void SetFillColor(Color color);

private:
	Color m_fillColor;
};

#endif // SOLID_SHAPE_H
