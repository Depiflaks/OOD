//
// Created by smmm on 06.05.2025.
//

#ifndef LINE_SEGMENT_H
#define LINE_SEGMENT_H
#include "shape/Shape.h"

class LineSegment final : public Shape
{
public:
	LineSegment() = default;
	LineSegment(Point start, Point end);
	LineSegment(Point start, Point end, Color outlineColor);

	[[nodiscard]] double GetArea() const override;
	[[nodiscard]] double GetPerimeter() const override;
	[[nodiscard]] std::string ToString() const override;

	void Draw(ICanvas& canvas) override;

	void Print(std::ostream &os) const override;
	friend std::istream& operator>>(std::istream& is, LineSegment& lineSegment);

	[[nodiscard]] Point GetStartPoint() const;
	[[nodiscard]] Point GetEndPoint() const;

private:
	Point m_startPoint;
	Point m_endPoint;
};

#endif // LINE_SEGMENT_H
