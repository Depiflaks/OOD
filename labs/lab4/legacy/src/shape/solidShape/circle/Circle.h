//
// Created by smmm on 10.05.2025.
//

#ifndef CIRCLE_H
#define CIRCLE_H
#include "shape/solidShape/SolidShape.h"

class Circle final : public SolidShape
{
public:
	Circle();
	Circle(Point center, double radius);
	Circle(Point center, double radius, Color outlineColor);
	Circle(Point center, double radius, Color outlineColor, Color fillColor);

	[[nodiscard]] double GetArea() const override;
	[[nodiscard]] double GetPerimeter() const override;
	[[nodiscard]] std::string ToString() const override;

	void Draw(ICanvas& canvas) override;

	void Print(std::ostream &os) const override;
    friend std::istream& operator>>(std::istream& is, Circle& circle);

	[[nodiscard]] Point GetCenter() const;
	[[nodiscard]] double GetRadius() const;

private:
	Point m_center{};
	double m_radius{};
};

#endif // CIRCLE_H
