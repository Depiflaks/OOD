//
// Created by smmm on 12.05.2025.
//

#include "GlCanvas.h"
#include <cmath>

void GlCanvas::SetColor(const Color c) {
	const auto r = static_cast<float>(c.GetRed()) / 255.0f;
	const auto g = static_cast<float>(c.GetGreen()) / 255.0f;
	const auto b = static_cast<float>(c.GetBlue()) / 255.0f;
	const auto a = static_cast<float>(c.GetAlpha()) / 255.0f;
    glColor4f(r, g, b, a);
}

void GlCanvas::DrawLine(const Point from, const Point to, const Color lineColor) {
    SetColor(lineColor);
    glBegin(GL_LINES);
        glVertex2d(from.x, from.y);
        glVertex2d(to.x,   to.y);
    glEnd();
}

void GlCanvas::DrawPolygon(std::vector<Point> points, const Color lineColor) {
    SetColor(lineColor);
    glBegin(GL_LINE_LOOP);
    for (auto& [x, y] : points) {
        glVertex2d(x, y);
    }
    glEnd();
}

void GlCanvas::FillPolygon(std::vector<Point> points, Color fillColor) {
    SetColor(fillColor);
    glBegin(GL_POLYGON);
    for (auto& [x, y] : points) {
        glVertex2d(x, y);
    }
    glEnd();
}

void GlCanvas::DrawCircle(const Point center, const double radius, const Color lineColor) {
    SetColor(lineColor);
	constexpr int segments = 64;
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; ++i) {
		const double theta = 2.0 * M_PI * static_cast<double>(i) / static_cast<double>(segments);
		const double x = center.x + radius * cos(theta);
		const double y = center.y + radius * sin(theta);
        glVertex2d(x, y);
    }
    glEnd();
}

void GlCanvas::FillCircle(const Point center, const double radius, const Color fillColor) {
    SetColor(fillColor);
	constexpr int segments = 64;
    glBegin(GL_TRIANGLE_FAN);
        glVertex2d(center.x, center.y);
        for (int i = 0; i <= segments; ++i) {
			const double theta = 2.0 * M_PI * static_cast<double>(i) / static_cast<double>(segments);
			const double x = center.x + radius * cos(theta);
			const double y = center.y + radius * sin(theta);
            glVertex2d(x, y);
        }
    glEnd();
}
