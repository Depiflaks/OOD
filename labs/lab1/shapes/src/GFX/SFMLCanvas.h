#ifndef SFMLCANVAS_H
#define SFMLCANVAS_H

#include "ICanvas.h"
#include <SFML/Graphics.hpp>

class SFMLCanvas : public ICanvas
{
public:
	SFMLCanvas(sf::RenderTarget& target)
		: m_target(target)
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
		m_target.draw(line, 2, sf::Lines);
		m_currentPosition.x = x;
		m_currentPosition.y = y;
	}

	void DrawEllipse(double cx, double cy, double rx, double ry) override
	{
		sf::CircleShape ellipse(rx);
		ellipse.setScale(1.0f, ry / rx);
		ellipse.setPosition(cx - rx, cy - ry);
		ellipse.setFillColor(sf::Color::Transparent);
		ellipse.setOutlineColor(m_currentColor);
		ellipse.setOutlineThickness(1.0f);
		m_target.draw(ellipse);
	}

	void DrawText(double left, double top, double fontSize, const std::string& text) override
	{
		static sf::Font font;
		if (font.getInfo().family.empty())
		{
			font.loadFromFile("arial.ttf");
		}

		sf::Text sfText(text, font, static_cast<unsigned int>(fontSize));
		sfText.setPosition(left, top);
		sfText.setFillColor(m_currentColor);
		m_target.draw(sfText);
	}

private:
	sf::RenderTarget& m_target;
	sf::Vector2f m_currentPosition;
	sf::Color m_currentColor;
};

#endif /* SFMLCANVAS_H */
