//
// Created by smmm on 27.11.2025.
//

#ifndef OOD_SHAPES_H
#define OOD_SHAPES_H
#include "IShape.h"

#include <functional>
#include <math.h>
#include <utility>

using DrawingStrategy
	= std::function<void(ICanvas& canvas, const IShape& shape)>;

inline DrawingStrategy MakeRectangleStrategy()
{
	return [](ICanvas& canvas, const IShape& shape) {
		RectD r = shape.GetFrame();
		canvas.BeginFill(shape.GetFillStyle().GetColor().value_or(0));
		canvas.SetLineColor(shape.GetOutlineStyle().GetColor().value_or(0));
		canvas.MoveTo(r.left, r.top);
		canvas.LineTo(r.left + r.width, r.top);
		canvas.LineTo(r.left + r.width, r.top + r.height);
		canvas.LineTo(r.left, r.top + r.height);
		canvas.LineTo(r.left, r.top);
		canvas.EndFill();
	};
}

inline DrawingStrategy MakePolygonStrategy(size_t vertexCount)
{
	return [vertexCount](ICanvas& canvas, const IShape& shape) {
		if (vertexCount < 3)
			return;
		RectD r = shape.GetFrame();
		double cx = r.left + r.width * 0.5;
		double cy = r.top + r.height * 0.5;
		double rx = r.width * 0.5;
		double ry = r.height * 0.5;

		canvas.BeginFill(shape.GetFillStyle().GetColor().value_or(0));
		canvas.SetLineColor(shape.GetOutlineStyle().GetColor().value_or(0));

		double angleStep = 2.0 * M_PI / vertexCount;
		double a0 = 0.0;

		double x0 = cx + rx * std::cos(a0);
		double y0 = cy + ry * std::sin(a0);
		canvas.MoveTo(x0, y0);

		for (size_t i = 1; i < vertexCount; ++i)
		{
			double a = i * angleStep;
			double x = cx + rx * std::cos(a);
			double y = cy + ry * std::sin(a);
			canvas.LineTo(x, y);
		}

		canvas.LineTo(x0, y0);
		canvas.EndFill();
	};
}

inline DrawingStrategy MakeEllipseStrategy()
{
	return [](ICanvas& canvas, const IShape& shape) {
		RectD r = shape.GetFrame();
		canvas.BeginFill(shape.GetFillStyle().GetColor().value_or(0));
		canvas.SetLineColor(shape.GetOutlineStyle().GetColor().value_or(0));
		canvas.DrawEllipse(r.left, r.top, r.width, r.height);
		canvas.EndFill();
	};
}

class SimpleShape final : public IShape
{
public:
	explicit SimpleShape(std::shared_ptr<DrawingStrategy> strategy, RectD frame,
		std::unique_ptr<IStyle> fillColor, std::unique_ptr<IStyle> outlineColor,
		int thickness = 1)
		: m_strategy(std::move(strategy))
		, m_frame(frame)
		, m_outline(std::move(outlineColor))
		, m_fill(std::move(fillColor))
		, m_outlineThickness(thickness)
	{
	}

	void Draw(ICanvas& canvas) const override
	{
		(*m_strategy)(canvas, *this);
	}

	RectD GetFrame() const override
	{
		return m_frame;
	}

	void SetFrame(const RectD& rect) override
	{
		m_frame = rect;
	}

	IStyle& GetOutlineStyle() override
	{
		return *m_outline;
	}

	const IStyle& GetOutlineStyle() const override
	{
		return *m_outline;
	}

	IStyle& GetFillStyle() override
	{
		return *m_fill;
	}
	const IStyle& GetFillStyle() const override
	{
		return *m_fill;
	}

	std::shared_ptr<IGroupShape> GetGroup() override
	{
		return nullptr;
	}

	std::shared_ptr<const IGroupShape> GetGroup() const override
	{
		return nullptr;
	}

	void SetOutlineColor(RGBAColor color) override
	{
		m_outline->SetColor(color);
	}
	void SetFillColor(RGBAColor color) override
	{
		m_fill->SetColor(color);
	}
	void EnableOutline(bool enable) override
	{
		m_outline->Enable(enable);
	}
	void EnableFill(bool enable) override
	{
		m_fill->Enable(enable);
	}

	void SetOutlineThickness(int thickness) override
	{
		m_outlineThickness = thickness;
	}
	std::optional<int> GetOutlineThickness() const override
	{
		return m_outlineThickness;
	}

	std::shared_ptr<IShape> Clone() override
	{
		return std::make_shared<SimpleShape>(m_strategy, m_frame,
			m_fill->Clone(), m_outline->Clone(), m_outlineThickness);
	}

private:
	std::shared_ptr<DrawingStrategy> m_strategy;
	RectD m_frame;
	std::unique_ptr<IStyle> m_outline;
	std::unique_ptr<IStyle> m_fill;
	int m_outlineThickness;
};

#endif // OOD_SHAPES_H
