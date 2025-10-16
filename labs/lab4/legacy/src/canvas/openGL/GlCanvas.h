//
// Created by smmm on 12.05.2025.
//

#ifndef CANVAS_H
#define CANVAS_H
#include "../ICanvas.h"

#include <GL/gl.h>
#include <vector>

class GlCanvas final : public ICanvas
{
public:
	GlCanvas() = default;
	~GlCanvas() override = default;

	void DrawLine(Point from, Point to, Color lineColor) override;
	void DrawPolygon(std::vector<Point> points, Color lineColor) override;
	void FillPolygon(std::vector<Point> points, Color fillColor) override;
	void DrawCircle(Point center, double radius, Color lineColor) override;
	void FillCircle(Point center, double radius, Color fillColor) override;

private:
	static void SetColor(Color c);
};

#endif // CANVAS_H
