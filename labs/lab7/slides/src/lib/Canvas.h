#pragma once
#include "CommonTypes.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <vector>

class ICanvas
{
public:
	virtual void SetLineColor(RGBAColor color) = 0;
	virtual void BeginFill(RGBAColor color) = 0;
	virtual void EndFill() = 0;
	virtual void MoveTo(double x, double y) = 0;
	virtual void LineTo(double x, double y) = 0;
	virtual void DrawEllipse(
		double left, double top, double width, double height)
		= 0;
	virtual void SetLineWidth(int width) = 0;

	virtual ~ICanvas() = default;
};

inline sf::Color toSfColor(RGBAColor color)
{
	return sf::Color((color >> 16) & 0xFF, // R
		(color >> 8) & 0xFF, // G
		color & 0xFF, // B
		(color >> 24) & 0xFF // A
	);
}

class SFMLCanvas final : public ICanvas
{
public:
	explicit SFMLCanvas(sf::RenderWindow& window)
		: m_window(window)
		, m_currentFillColor(sf::Color::Transparent)
		, m_currentLineColor(sf::Color::Transparent)
		, m_currentLineWidth(1)
	{
	}

	void SetLineColor(RGBAColor color) override
	{
		m_currentLineColor = toSfColor(color);
	}

	void BeginFill(RGBAColor color) override
	{
		m_currentFillColor = toSfColor(color);
	}

	void EndFill() override
	{
		if (m_pathVertices.size() >= 3 && m_currentFillColor.a > 0)
		{
			sf::ConvexShape convex;
			convex.setPointCount(m_pathVertices.size());
			for (size_t i = 0; i < m_pathVertices.size(); ++i)
			{
				convex.setPoint(i, m_pathVertices[i]);
			}
			convex.setFillColor(m_currentFillColor);
			m_window.draw(convex);
		}

		if (m_pathVertices.size() >= 2 && m_currentLineColor.a > 0)
		{
			sf::VertexArray outline(sf::LineStrip, m_pathVertices.size() + 1);
			for (size_t i = 0; i < m_pathVertices.size(); ++i)
			{
				outline[i].position = m_pathVertices[i];
				outline[i].color = m_currentLineColor;
			}
			outline[m_pathVertices.size()].position = m_pathVertices[0];
			outline[m_pathVertices.size()].color = m_currentLineColor;

			m_window.draw(outline);
		}

		m_pathVertices.clear();
	}

	void MoveTo(double x, double y) override
	{
		m_pathVertices.clear();
		m_pathVertices.emplace_back(
			static_cast<float>(x), static_cast<float>(y));
	}

	void LineTo(double x, double y) override
	{
		m_pathVertices.emplace_back(
			static_cast<float>(x), static_cast<float>(y));
	}

	void DrawEllipse(
		double left, double top, double width, double height) override
	{
		float radiusX = (float)width / 2.0f;
		float radiusY = (float)height / 2.0f;

		sf::CircleShape ellipse(radiusX);
		ellipse.setPosition((float)left, (float)top);
		ellipse.setScale(1.0f, radiusY / radiusX);

		ellipse.setFillColor(m_currentFillColor);
		ellipse.setOutlineThickness(0.0f);
		m_window.draw(ellipse);

		if (m_currentLineColor.a > 0)
		{
			ellipse.setFillColor(sf::Color::Transparent);
			ellipse.setOutlineThickness((float)m_currentLineWidth);
			ellipse.setOutlineColor(m_currentLineColor);

			m_window.draw(ellipse);
		}
	}

	void SetLineWidth(int width) override
	{
		m_currentLineWidth = std::max(1, width);
	}

private:
	sf::RenderWindow& m_window;
	sf::Color m_currentFillColor;
	sf::Color m_currentLineColor;
	int m_currentLineWidth;
	std::vector<sf::Vector2f> m_pathVertices;
};
