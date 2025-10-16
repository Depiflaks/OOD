//
// Created by smmm on 08.05.2025.
//

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "shape/solidShape/SolidShape.h"

#include <string>

class Triangle final : public SolidShape
{
public:
	explicit Triangle() = default;
	explicit Triangle(Point vertex1, Point vertex2, Point vertex3);
	explicit Triangle(Point vertex1, Point vertex2, Point vertex3, Color outlineColor);
	explicit Triangle(Point vertex1, Point vertex2, Point vertex3, Color outlineColor, Color fillColor);

	void Draw(ICanvas& canvas) override;

	[[nodiscard]] double GetArea() const override;
	[[nodiscard]] double GetPerimeter() const override;
	[[nodiscard]] std::string ToString() const override;

	void Print(std::ostream& os) const override;
	friend std::istream& operator>>(std::istream& is, Triangle& triangle);

	[[nodiscard]] Point GetVertex1() const;
	[[nodiscard]] Point GetVertex2() const;
	[[nodiscard]] Point GetVertex3() const;

private:
	Point m_vertex1;
	Point m_vertex2;
	Point m_vertex3;
};

#endif // TRIANGLE_H
