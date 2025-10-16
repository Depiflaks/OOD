//
// Created by smmm on 06.05.2025.
//

#ifndef SHAPE_H
#define SHAPE_H
#include "canvas/ICanvas.h"
#include "drawable/ICanvasDrawable.h"

#include <algorithm>
#include <bits/ranges_algo.h>
#include <memory>
#include <string>

enum class ShapeType
{
	Triangle,
	Rectangle,
	Circle,
	LineSegment,
	Unknown
};

class Shape : public ICanvasDrawable
{
public:
	explicit Shape();
	explicit Shape(Color outlineColor);

	[[nodiscard]] virtual double GetArea() const = 0;
	[[nodiscard]] virtual double GetPerimeter() const = 0;
	[[nodiscard]] virtual std::string ToString() const = 0;
	virtual void Print(std::ostream& os) const = 0;

	[[nodiscard]] Color GetOutlineColor() const;
	void SetOutlineColor(Color color);

private:
	Color m_outlineColor;
};

inline std::shared_ptr<Shape> GetShapeWithMaxArea(const std::vector<std::shared_ptr<Shape>>& shapes)
{
	const auto maxShape = std::ranges::max_element(shapes,
		[](const std::shared_ptr<Shape>& shape1, const std::shared_ptr<Shape>& shape2) {
			return shape1->GetArea() < shape2->GetArea();
		});
	return maxShape == shapes.end() ? nullptr : *maxShape;
}

inline std::shared_ptr<Shape> GetShapeWithMaxPerimeter(const std::vector<std::shared_ptr<Shape>>& shapes)
{
	const auto maxShape = std::ranges::max_element(shapes,
		[](const std::shared_ptr<Shape>& shape1, const std::shared_ptr<Shape>& shape2) {
			return shape1->GetPerimeter() < shape2->GetPerimeter();
		});

	return maxShape == shapes.end() ? nullptr : *maxShape;
}

#endif // SHAPE_H
