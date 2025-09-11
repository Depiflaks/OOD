#ifndef PICTURE_H
#define PICTURE_H

#include "../GFX/ICanvas.h"
#include "./Models/IShapeModel.h"
#include "./Shape.h"
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

namespace shapes
{
class Picture
{
public:
	Picture(gfx::ICanvas& canvas)
		: m_canvas(&canvas)
	{
	}

	void DrawPicture()
	{
		for (auto& [id, shape] : m_shapes)
		{
			shape->Draw(*m_canvas);
		}
	}

	void DrawShape(const std::string& id)
	{
		if (m_shapes.find(id) != m_shapes.end())
		{
			m_shapes[id]->Draw(*m_canvas);
		}
	}

	void AddShape(const std::string& id, const Color& color, std::unique_ptr<IShapeModel> model)
	{
		auto shape = std::make_unique<Shape>(std::move(model), id, color);
		m_shapes[id] = std::move(shape);
		UpdateBounds();
	}

	void ChangeShape(const std::string& id, std::unique_ptr<IShapeModel> model)
	{
		if (m_shapes.find(id) != m_shapes.end())
		{
			m_shapes[id]->ChangeShape(std::move(model));
			UpdateBounds();
		}
	}

	void ChangeColor(const std::string& id, const Color& color)
	{
		if (m_shapes.find(id) != m_shapes.end())
		{
			m_shapes[id]->SetFillColor(color);
		}
	}

	void List()
	{
		for (const auto& [id, shape] : m_shapes)
		{
			std::cout << shape->ToString() << std::endl;
		}
	}

	void DeleteShape(const std::string& id)
	{
		m_shapes.erase(id);
		UpdateBounds();
	}

	void MovePicture(double dx, double dy)
	{
		for (auto& [id, shape] : m_shapes)
		{
			shape->Move(dx, dy);
		}
		UpdateBounds();
	}

	void MoveShape(const std::string& id, double dx, double dy)
	{
		if (m_shapes.find(id) != m_shapes.end())
		{
			m_shapes[id]->Move(dx, dy);
			UpdateBounds();
		}
	}

	Bounds GetBounds() const
	{
		return m_bounds;
	}

private:
	void UpdateBounds()
	{
		if (m_shapes.empty())
		{
			m_bounds = Bounds();
			return;
		}

		double minX = std::numeric_limits<double>::max();
		double minY = std::numeric_limits<double>::max();
		double maxX = std::numeric_limits<double>::lowest();
		double maxY = std::numeric_limits<double>::lowest();

		for (const auto& [id, shape] : m_shapes)
		{
			auto bounds = shape->GetBounds();
			minX = std::min(minX, bounds.position.x);
			minY = std::min(minY, bounds.position.y);
			maxX = std::max(maxX, bounds.position.x + bounds.size.w);
			maxY = std::max(maxY, bounds.position.y + bounds.size.h);
		}

		m_bounds = Bounds{ Position{ minX, minY }, Size{ maxX - minX, maxY - minY } };
	}

	Bounds m_bounds;
	gfx::ICanvas* m_canvas;
	std::unordered_map<std::string, std::unique_ptr<IShape>> m_shapes;
};
} // namespace shapes

#endif /* PICTURE_H */
