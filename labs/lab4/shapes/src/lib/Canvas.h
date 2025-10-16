#ifndef CANVAS_H
#define CANVAS_H

#include "Color.h"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class ICanvas
{
public:
	virtual ~ICanvas() = default;
	virtual void MoveTo(double x, double y) = 0;
	virtual void SetColor(const Color& color) = 0;
	virtual void LineTo(double x, double y) = 0;
	virtual void DrawEllipse(double cx, double cy, double rx, double ry) = 0;
	virtual void DrawFilledRectangle(double x1, double y1, double x2, double y2) = 0;
	virtual void DrawFilledTriangle(double x1, double y1, double x2, double y2, double x3, double y3) = 0;
	virtual void DrawFilledRegularPolygon(const std::vector<std::pair<double, double>>& points) = 0;
	virtual void DrawText(double left, double top, double fontSize, const std::string& text) = 0;
	virtual void Render() = 0;
};

class SFMLCanvas : public ICanvas
{
public:
	SFMLCanvas(sf::RenderWindow& window)
		: m_window(window)
	{
	}

	void MoveTo(double x, double y) override
	{
		m_currentPosition.x = x;
		m_currentPosition.y = y;
	}

	void SetColor(const Color& color) override
	{
		m_currentColor = sf::Color(color.r, color.g, color.b, color.a);
	}

	void LineTo(double x, double y) override
	{
		sf::Vertex line[] = {
			sf::Vertex(sf::Vector2f(m_currentPosition.x, m_currentPosition.y), m_currentColor),
			sf::Vertex(sf::Vector2f(x, y), m_currentColor)
		};
		m_window.draw(line, 2, sf::Lines);
		m_currentPosition.x = x;
		m_currentPosition.y = y;
	}

	void DrawEllipse(double cx, double cy, double rx, double ry) override
	{
		sf::CircleShape ellipse(rx);
		ellipse.setScale(1.0f, ry / rx);
		ellipse.setPosition(cx - rx, cy - ry);
		ellipse.setFillColor(m_currentColor);
		ellipse.setOutlineColor(m_currentColor);
		ellipse.setOutlineThickness(1.0f);
		m_window.draw(ellipse);
	}

	void DrawFilledRectangle(double x1, double y1, double x2, double y2) override
	{
		sf::RectangleShape rectangle(sf::Vector2f(x2 - x1, y2 - y1));
		rectangle.setPosition(x1, y1);
		rectangle.setFillColor(m_currentColor);
		m_window.draw(rectangle);
	}

	void DrawFilledTriangle(double x1, double y1, double x2, double y2, double x3, double y3) override
	{
		sf::ConvexShape triangle;
		triangle.setPointCount(3);
		triangle.setPoint(0, sf::Vector2f(x1, y1));
		triangle.setPoint(1, sf::Vector2f(x2, y2));
		triangle.setPoint(2, sf::Vector2f(x3, y3));
		triangle.setFillColor(m_currentColor);
		m_window.draw(triangle);
	}

	void DrawFilledRegularPolygon(const std::vector<std::pair<double, double>>& points) override
	{
		sf::ConvexShape polygon;
		polygon.setPointCount(points.size());
		for (size_t i = 0; i < points.size(); ++i)
		{
			polygon.setPoint(i, sf::Vector2f(points[i].first, points[i].second));
		}
		polygon.setFillColor(m_currentColor);
		m_window.draw(polygon);
	}

	void DrawText(double left, double top, double fontSize, const std::string& text) override
	{
	}

	void Render() override
	{
		m_window.display();
	}

private:
	sf::RenderWindow& m_window;
	sf::Vector2f m_currentPosition;
	sf::Color m_currentColor;
};

#endif /* CANVAS_H */
