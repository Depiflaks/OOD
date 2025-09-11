#ifndef SHAPE_H
#define SHAPE_H

#include "./IShape.h"

namespace shapes
{

class Shape final : public IShape
{
public:
	Shape(std::unique_ptr<IShapeModel> model, const std::string& id, const Color& color = Color("#000000ff"))
		: m_shapeModel(std::move(model))
		, m_id(id)
		, m_fillColor(color)
	{
	}

	void Draw(gfx::ICanvas& canvas) override
	{
		m_shapeModel->Draw(canvas, m_fillColor);
	}

	void SetFillColor(const Color& color) override
	{
		m_fillColor = color;
	}

	void ChangeShape(std::unique_ptr<IShapeModel> model) override
	{
		m_shapeModel = std::move(model);
	}

	void Move(double dx, double dy) override
	{
		m_shapeModel->Move(dx, dy);
	}

	std::string GetId() const override
	{
		return m_id;
	}

	std::string ToString() const override
	{
		// TODO: переделать формат вывода
		return "Shape ID: " + m_id + ", Color: " + m_fillColor.ToHexString();
	}

private:
	std::unique_ptr<IShapeModel> m_shapeModel;
	std::string m_id;
	Color m_fillColor;
};

} // namespace shapes

#endif /* SHAPE_H */
