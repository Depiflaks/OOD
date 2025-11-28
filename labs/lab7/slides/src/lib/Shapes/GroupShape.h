//
// Created by smmm on 28.11.2025.
//

#ifndef OOD_GROUPSHAPE_H
#define OOD_GROUPSHAPE_H
#include "IShape.h"

#include <algorithm>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

class GroupShape final : public IGroupShape
{
	class GroupStyle final : public IStyle
	{
	public:
		using StyleEnumerator
			= std::function<void(const std::function<void(IStyle&)>&)>;

		explicit GroupStyle(StyleEnumerator enumerator)
			: m_enumerator(std::move(enumerator))
		{
		}

		std::optional<bool> IsEnabled() const override
		{
			return m_enabled;
		}

		void Enable(bool enable) override
		{
			m_enabled = enable;
			m_enumerator([enable](IStyle& style) { style.Enable(enable); });
		}

		std::optional<RGBAColor> GetColor() const override
		{
			return m_color;
		}

		void SetColor(RGBAColor color) override
		{
			m_color = color;
			m_enumerator([color](IStyle& style) { style.SetColor(color); });
		}

		std::unique_ptr<IStyle> Clone() const override
		{
			return nullptr;
		}

		void SetCachedColor(std::optional<RGBAColor> color)
		{
			m_color = color;
		}

		void SetCachedEnabled(std::optional<bool> enabled)
		{
			m_enabled = enabled;
		}

	private:
		StyleEnumerator m_enumerator;
		std::optional<bool> m_enabled;
		std::optional<RGBAColor> m_color;
	};

public:
	GroupShape()
	{
		m_outlineStyle = std::make_shared<GroupStyle>(
			[this](const std::function<void(IStyle&)>& fn) {
				for (const auto& shape : m_shapes)
				{
					fn(shape->GetOutlineStyle());
				}
			});

		m_fillStyle = std::make_shared<GroupStyle>(
			[this](const std::function<void(IStyle&)>& fn) {
				for (const auto& shape : m_shapes)
				{
					fn(shape->GetFillStyle());
				}
			});
	}

	void Draw(ICanvas& canvas) const override
	{
		for (const auto& shape : m_shapes)
		{
			shape->Draw(canvas);
		}
	}

	RectD GetFrame() const override
	{
		return m_frame;
	}

	void SetFrame(const RectD& rect) override
	{
		if (m_shapes.empty())
		{
			m_frame = rect;
			return;
		}

		double scaleX = m_frame.width == 0 ? 0 : rect.width / m_frame.width;
		double scaleY = m_frame.height == 0 ? 0 : rect.height / m_frame.height;

		for (auto& shape : m_shapes)
		{
			RectD oldShapeFrame = shape->GetFrame();
			double newX
				= rect.left + (oldShapeFrame.left - m_frame.left) * scaleX;
			double newY = rect.top + (oldShapeFrame.top - m_frame.top) * scaleY;
			double newWidth = oldShapeFrame.width * scaleX;
			double newHeight = oldShapeFrame.height * scaleY;

			shape->SetFrame({ newX, newY, newWidth, newHeight });
		}

		m_frame = rect;
	}

	IStyle& GetOutlineStyle() override
	{
		return *m_outlineStyle;
	}

	const IStyle& GetOutlineStyle() const override
	{
		return *m_outlineStyle;
	}

	IStyle& GetFillStyle() override
	{
		return *m_fillStyle;
	}

	const IStyle& GetFillStyle() const override
	{
		return *m_fillStyle;
	}

	void SetOutlineColor(RGBAColor color) override
	{
		m_outlineStyle->SetColor(color);
	}

	void SetFillColor(RGBAColor color) override
	{
		m_fillStyle->SetColor(color);
	}

	void EnableOutline(bool enable) override
	{
		m_outlineStyle->Enable(enable);
	}

	void EnableFill(bool enable) override
	{
		m_fillStyle->Enable(enable);
	}

	void SetOutlineThickness(int thickness) override
	{
		for (auto& shape : m_shapes)
		{
			shape->SetOutlineThickness(thickness);
		}
		m_outlineThickness = thickness;
	}

	std::optional<int> GetOutlineThickness() const override
	{
		return m_outlineThickness;
	}

	std::shared_ptr<IGroupShape> GetGroup() override
	{
		return std::make_shared<GroupShape>(*this);
	}

	std::shared_ptr<const IGroupShape> GetGroup() const override
	{
		return std::make_shared<GroupShape>(*this);
	}

	size_t GetShapesCount() const override
	{
		return m_shapes.size();
	}

	void InsertShape(const std::shared_ptr<IShape>& shape,
		size_t position = std::numeric_limits<size_t>::max()) override
	{
		if (position >= m_shapes.size())
		{
			m_shapes.push_back(shape);
		}
		else
		{
			m_shapes.insert(m_shapes.begin() + position, shape);
		}

		RecalculateFrame();
		UpdateStyles();
		UpdateOutlineThickness();
	}

	std::shared_ptr<IShape> GetShapeAtIndex(size_t index) override
	{
		ValidateIndex(index);
		return m_shapes[index];
	}

	void RemoveShapeAtIndex(size_t index) override
	{
		ValidateIndex(index);
		m_shapes.erase(m_shapes.begin() + index);
		RecalculateFrame();
		UpdateStyles();
		UpdateOutlineThickness();
	}

	std::shared_ptr<IShape> Clone() override
	{
		auto newGroup = std::make_shared<GroupShape>();
		for (const auto& shape : m_shapes)
		{
			newGroup->InsertShape(shape->Clone());
		}
		return newGroup;
	}

private:
	void ValidateIndex(size_t index) const
	{
		if (index >= m_shapes.size())
		{
			throw std::out_of_range("Index out of range");
		}
	}

	void RecalculateFrame()
	{
		if (m_shapes.empty())
		{
			m_frame = { 0, 0, 0, 0 };
			return;
		}

		double minX = std::numeric_limits<double>::max();
		double minY = std::numeric_limits<double>::max();
		double maxX = std::numeric_limits<double>::lowest();
		double maxY = std::numeric_limits<double>::lowest();

		for (const auto& shape : m_shapes)
		{
			RectD frame = shape->GetFrame();
			if (frame.left < minX)
				minX = frame.left;
			if (frame.top < minY)
				minY = frame.top;
			if (frame.left + frame.width > maxX)
				maxX = frame.left + frame.width;
			if (frame.top + frame.height > maxY)
				maxY = frame.top + frame.height;
		}

		m_frame = { minX, minY, maxX - minX, maxY - minY };
	}

	void UpdateStyles()
	{
		auto updateStyle = [&](std::shared_ptr<GroupStyle>& groupStyle,
							   bool isOutline) {
			if (m_shapes.empty())
			{
				groupStyle->SetCachedColor(std::nullopt);
				groupStyle->SetCachedEnabled(std::nullopt);
				return;
			}

			auto getStyle
				= [&](const std::shared_ptr<IShape>& s) -> const IStyle& {
				return isOutline ? s->GetOutlineStyle() : s->GetFillStyle();
			};

			const IStyle& firstStyle = getStyle(m_shapes[0]);
			std::optional<RGBAColor> commonColor = firstStyle.GetColor();
			std::optional<bool> commonEnabled = firstStyle.IsEnabled();

			for (size_t i = 1; i < m_shapes.size(); ++i)
			{
				const IStyle& style = getStyle(m_shapes[i]);
				if (style.GetColor() != commonColor)
				{
					commonColor = std::nullopt;
				}
				if (style.IsEnabled() != commonEnabled)
				{
					commonEnabled = std::nullopt;
				}
			}

			groupStyle->SetCachedColor(commonColor);
			groupStyle->SetCachedEnabled(commonEnabled);
		};

		updateStyle(m_outlineStyle, true);
		updateStyle(m_fillStyle, false);
	}

	void UpdateOutlineThickness()
	{
		if (m_shapes.empty())
		{
			m_outlineThickness = std::nullopt;
			return;
		}

		std::optional<int> commonThickness = m_shapes[0]->GetOutlineThickness();

		for (size_t i = 1; i < m_shapes.size(); ++i)
		{
			if (m_shapes[i]->GetOutlineThickness() != commonThickness)
			{
				commonThickness = std::nullopt;
				break;
			}
		}

		m_outlineThickness = commonThickness;
	}

	std::vector<std::shared_ptr<IShape>> m_shapes;
	RectD m_frame = { 0, 0, 0, 0 };
	std::shared_ptr<GroupStyle> m_outlineStyle;
	std::shared_ptr<GroupStyle> m_fillStyle;
	std::optional<int> m_outlineThickness;
};
#endif // OOD_GROUPSHAPE_H
