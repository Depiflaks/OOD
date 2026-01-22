#ifndef GRAPHICSADAPTER_H
#define GRAPHICSADAPTER_H

#include "GraphicsLib.h"
#include "ModernGraphicsLib.h"

class ObjectModernCanvasAdapter : public graphics_lib::ICanvas
{
public:
	ObjectModernCanvasAdapter(
		std::shared_ptr<modern_graphics_lib::ModernGraphicsRenderer> renderer)
		: m_renderer(renderer)
		, m_currentColor(0x000000)
		, m_currentPointerPosition(0, 0)
	{
		m_renderer->BeginDraw();
	}

	~ObjectModernCanvasAdapter()
	{
		m_renderer->EndDraw();
	}

	void SetColor(uint32_t rgbColor) override
	{
		m_currentColor = rgbColor;
	}

	void MoveTo(int x, int y) override
	{
		m_currentPointerPosition = modern_graphics_lib::Point(x, y);
	}

	void LineTo(int x, int y) override
	{
		modern_graphics_lib::Point endPoint(x, y);
		m_renderer->DrawLine(
			m_currentPointerPosition, endPoint, m_currentColor);
		m_currentPointerPosition = endPoint;
	}

private:
	std::shared_ptr<modern_graphics_lib::ModernGraphicsRenderer> m_renderer;
	uint32_t m_currentColor;
	modern_graphics_lib::Point m_currentPointerPosition;
};

class ClassModernCanvasAdapter
	: public graphics_lib::ICanvas
	, private modern_graphics_lib::ModernGraphicsRenderer
{
public:
	ClassModernCanvasAdapter(std::ostream& strm)
		: modern_graphics_lib::ModernGraphicsRenderer(strm)
		, m_currentColor(0x000000)
		, m_currentPointerPosition(0, 0)
	{
		BeginDraw();
	}

	~ClassModernCanvasAdapter()
	{
		EndDraw();
	}

	void SetColor(uint32_t rgbColor) override
	{
		m_currentColor = rgbColor;
	}

	void MoveTo(int x, int y) override
	{
		m_currentPointerPosition = modern_graphics_lib::Point(x, y);
	}

	void LineTo(int x, int y) override
	{
		modern_graphics_lib::Point endPoint(x, y);
		DrawLine(m_currentPointerPosition, endPoint, m_currentColor);
		m_currentPointerPosition = endPoint;
	}

private:
	uint32_t m_currentColor;
	modern_graphics_lib::Point m_currentPointerPosition;
};

#endif /* GRAPHICSADAPTER_H */
