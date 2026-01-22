#include "lib/CommonTypes.h"
#include "lib/Shapes/IShape.h"

#include <cmath>
#include <functional>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

using namespace testing;

bool operator==(const RectD& a, const RectD& b)
{
	const double EPSILON = 1e-9;
	return std::abs(a.left - b.left) < EPSILON
		&& std::abs(a.top - b.top) < EPSILON
		&& std::abs(a.width - b.width) < EPSILON
		&& std::abs(a.height - b.height) < EPSILON;
}

std::ostream& operator<<(std::ostream& os, const RectD& r)
{
	return os << "RectD{l:" << r.left << ", t:" << r.top << ", w:" << r.width
			  << ", h:" << r.height << "}";
}

class MockStyle : public IStyle
{
public:
	MOCK_CONST_METHOD0(IsEnabled, std::optional<bool>());
	MOCK_METHOD1(Enable, void(bool enable));
	MOCK_CONST_METHOD0(GetColor, std::optional<RGBAColor>());
	MOCK_METHOD1(SetColor, void(RGBAColor color));
	MOCK_CONST_METHOD0(Clone, std::unique_ptr<IStyle>());
};

class MockShape : public IShape
{
public:
	MockShape()
		: m_outlineStyle(std::make_unique<MockStyle>())
		, m_fillStyle(std::make_unique<MockStyle>())
	{
		ON_CALL(*this, Clone()).WillByDefault(Invoke([this]() {
			auto clone = std::make_shared<MockShape>();
			clone->SetFrame(GetFrame());
			return clone;
		}));
		ON_CALL(*this, GetGroup()).WillByDefault(Return(nullptr));
		ON_CALL(*this, GetGroup()).WillByDefault(Return(nullptr));
	}

	MOCK_CONST_METHOD0(GetFrame, RectD());
	MOCK_METHOD1(SetFrame, void(const RectD& rect));

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

	MOCK_METHOD1(SetOutlineColor, void(RGBAColor color));
	MOCK_METHOD1(SetFillColor, void(RGBAColor color));
	MOCK_METHOD1(EnableOutline, void(bool enable));
	MOCK_METHOD1(EnableFill, void(bool enable));
	MOCK_METHOD1(SetOutlineThickness, void(int thickness));
	MOCK_CONST_METHOD0(GetOutlineThickness, std::optional<int>());

	MOCK_CONST_METHOD1(Draw, void(ICanvas& canvas));
	MOCK_METHOD0(GetGroup, std::shared_ptr<IGroupShape>());
	MOCK_CONST_METHOD0(GetGroup, std::shared_ptr<const IGroupShape>());
	MOCK_METHOD0(Clone, std::shared_ptr<IShape>());

	MockStyle& GetMockOutlineStyle()
	{
		return *m_outlineStyle;
	}
	MockStyle& GetMockFillStyle()
	{
		return *m_fillStyle;
	}

private:
	std::unique_ptr<MockStyle> m_outlineStyle;
	std::unique_ptr<MockStyle> m_fillStyle;
};

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

	GroupShape(const GroupShape& other)
		: m_shapes(other.m_shapes)
		, m_frame(other.m_frame)
		, m_outlineThickness(other.m_outlineThickness)
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
		if (position > m_shapes.size())
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

TEST(GroupShapeTest, RecalculatesFrameOnInsert)
{
	auto group = std::make_shared<GroupShape>();
	ASSERT_EQ(group->GetShapesCount(), 0);
	ASSERT_EQ(group->GetFrame(), (RectD{ 0, 0, 0, 0 }));

	auto shape1 = std::make_shared<MockShape>();
	RectD frame1 = { 10.0, 10.0, 50.0, 50.0 };
	EXPECT_CALL(*shape1, GetFrame()).WillRepeatedly(Return(frame1));

	group->InsertShape(shape1);
	EXPECT_EQ(group->GetFrame(), frame1);
	EXPECT_EQ(group->GetShapesCount(), 1);

	auto shape2 = std::make_shared<MockShape>();
	RectD frame2 = { 90.0, 20.0, 30.0, 40.0 };
	EXPECT_CALL(*shape2, GetFrame()).WillRepeatedly(Return(frame2));

	group->InsertShape(shape2);
	RectD expectedFrame = { 10.0, 10.0, 110.0, 50.0 };
	EXPECT_EQ(group->GetFrame(), expectedFrame);
	EXPECT_EQ(group->GetShapesCount(), 2);
}

TEST(GroupShapeTest, ScalesNestedShapesCorrectly)
{
	auto group = std::make_shared<GroupShape>();

	auto shape1 = std::make_shared<MockShape>();
	RectD frame1 = { 10.0, 10.0, 50.0, 50.0 };
	EXPECT_CALL(*shape1, GetFrame()).WillRepeatedly(Return(frame1));

	auto shape2 = std::make_shared<MockShape>();
	RectD frame2 = { 70.0, 70.0, 40.0, 40.0 };
	EXPECT_CALL(*shape2, GetFrame()).WillRepeatedly(Return(frame2));

	group->InsertShape(shape1);
	group->InsertShape(shape2);

	RectD initialGroupFrame = { 10.0, 10.0, 100.0, 100.0 };
	EXPECT_EQ(group->GetFrame(), initialGroupFrame);

	RectD newGroupFrame = { 10.0, 10.0, 200.0, 200.0 };

	RectD expectedFrame1 = { 10.0, 10.0, 100.0, 100.0 };
	EXPECT_CALL(*shape1, SetFrame(expectedFrame1)).Times(1);

	RectD expectedFrame2 = { 130.0, 130.0, 80.0, 80.0 };
	EXPECT_CALL(*shape2, SetFrame(expectedFrame2)).Times(1);

	group->SetFrame(newGroupFrame);

	EXPECT_EQ(group->GetFrame(), newGroupFrame);
}

TEST(GroupShapeTest, FillStyleColorIsNullWhenNestedStylesDiffer)
{
	auto group = std::make_shared<GroupShape>();

	auto shape1 = std::make_shared<MockShape>();
	EXPECT_CALL(shape1->GetMockFillStyle(), GetColor())
		.WillRepeatedly(Return(0xFF0000FF));
	EXPECT_CALL(*shape1, GetFrame())
		.WillRepeatedly(Return(RectD{ 0, 0, 10, 10 }));

	auto shape2 = std::make_shared<MockShape>();
	EXPECT_CALL(shape2->GetMockFillStyle(), GetColor())
		.WillRepeatedly(Return(0x00FF00FF));
	EXPECT_CALL(*shape2, GetFrame())
		.WillRepeatedly(Return(RectD{ 20, 20, 10, 10 }));

	group->InsertShape(shape1);
	group->InsertShape(shape2);

	EXPECT_FALSE(group->GetFillStyle().GetColor().has_value());
}

TEST(GroupShapeTest, OutlineStyleInconsistencyChecks)
{
	auto group = std::make_shared<GroupShape>();

	auto shape1 = std::make_shared<MockShape>();
	EXPECT_CALL(shape1->GetMockOutlineStyle(), IsEnabled())
		.WillRepeatedly(Return(true));
	EXPECT_CALL(*shape1, GetFrame())
		.WillRepeatedly(Return(RectD{ 0, 0, 10, 10 }));

	auto shape2 = std::make_shared<MockShape>();
	EXPECT_CALL(shape2->GetMockOutlineStyle(), IsEnabled())
		.WillRepeatedly(Return(false));
	EXPECT_CALL(*shape2, GetFrame())
		.WillRepeatedly(Return(RectD{ 20, 20, 10, 10 }));

	group->InsertShape(shape1);
	group->InsertShape(shape2);

	EXPECT_FALSE(group->GetOutlineStyle().IsEnabled().has_value());

	EXPECT_CALL(*shape1, GetOutlineThickness()).WillRepeatedly(Return(5));
	EXPECT_CALL(*shape2, GetOutlineThickness()).WillRepeatedly(Return(10));

	group->InsertShape(std::make_shared<MockShape>());
	EXPECT_FALSE(group->GetOutlineThickness().has_value());
}

TEST(GroupShapeTest, StylePropagationSetsNestedStyles)
{
	auto group = std::make_shared<GroupShape>();

	auto shape1 = std::make_shared<MockShape>();
	auto& style1 = shape1->GetMockFillStyle();
	EXPECT_CALL(*shape1, GetFrame())
		.WillRepeatedly(Return(RectD{ 0, 0, 10, 10 }));

	auto shape2 = std::make_shared<MockShape>();
	auto& style2 = shape2->GetMockFillStyle();
	EXPECT_CALL(*shape2, GetFrame())
		.WillRepeatedly(Return(RectD{ 20, 20, 10, 10 }));

	group->InsertShape(shape1);
	group->InsertShape(shape2);

	RGBAColor newColor = 0x112233AA;
	bool newState = false;
	int newThickness = 15;

	EXPECT_CALL(style1, SetColor(newColor)).Times(1);
	EXPECT_CALL(style2, SetColor(newColor)).Times(1);
	group->SetFillColor(newColor);
	EXPECT_EQ(group->GetFillStyle().GetColor().value(), newColor);

	EXPECT_CALL(shape1->GetMockOutlineStyle(), Enable(newState)).Times(1);
	EXPECT_CALL(shape2->GetMockOutlineStyle(), Enable(newState)).Times(1);
	group->EnableOutline(newState);
	EXPECT_EQ(group->GetOutlineStyle().IsEnabled().value(), newState);

	EXPECT_CALL(*shape1, SetOutlineThickness(newThickness)).Times(1);
	EXPECT_CALL(*shape2, SetOutlineThickness(newThickness)).Times(1);
	group->SetOutlineThickness(newThickness);
	EXPECT_EQ(group->GetOutlineThickness().value(), newThickness);
}

TEST(GroupShapeTest, SupportsNestedGroups)
{
	auto innerShape = std::make_shared<MockShape>();
	RectD innerFrame = { 50.0, 50.0, 100.0, 100.0 };
	EXPECT_CALL(*innerShape, GetFrame()).WillRepeatedly(Return(innerFrame));

	auto innerGroup = std::make_shared<GroupShape>();
	innerGroup->InsertShape(innerShape);

	auto outerShape = std::make_shared<MockShape>();
	RectD outerFrame = { 200.0, 200.0, 50.0, 50.0 };
	EXPECT_CALL(*outerShape, GetFrame()).WillRepeatedly(Return(outerFrame));

	auto outerGroup = std::make_shared<GroupShape>();
	outerGroup->InsertShape(innerGroup);
	outerGroup->InsertShape(outerShape);

	RectD expectedFrame = { 50.0, 50.0, 200.0, 200.0 };
	EXPECT_EQ(outerGroup->GetFrame(), expectedFrame);

	RectD newOuterFrame = { 0.0, 0.0, 400.0, 400.0 };

	RectD expectedOuterShapeFrame = { 300.0, 300.0, 100.0, 100.0 };
	EXPECT_CALL(*outerShape, SetFrame(expectedOuterShapeFrame)).Times(1);

	outerGroup->SetFrame(newOuterFrame);
	EXPECT_EQ(outerGroup->GetFrame(), newOuterFrame);
}

TEST(GroupShapeTest, GetShapeAtIndexThrowsOnInvalidIndex)
{
	auto group = std::make_shared<GroupShape>();
	auto shape = std::make_shared<MockShape>();
	EXPECT_CALL(*shape, GetFrame())
		.WillRepeatedly(Return(RectD{ 0, 0, 10, 10 }));
	group->InsertShape(shape);

	ASSERT_NO_THROW(group->GetShapeAtIndex(0));
	ASSERT_THROW(group->GetShapeAtIndex(1), std::out_of_range);

	group->RemoveShapeAtIndex(0);
	ASSERT_THROW(group->GetShapeAtIndex(0), std::out_of_range);
}