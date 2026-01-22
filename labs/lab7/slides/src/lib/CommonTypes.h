#ifndef COMMONTYPES_H
#define COMMONTYPES_H
#include <cstdint>
#include <memory>
#include <optional>

using RGBAColor = uint32_t;
constexpr RGBAColor k_black = 0;

template <typename T>
struct Rect
{
	T left;
	T top;
	T width;
	T height;
};

using RectD = Rect<double>;

class IStyle
{
public:
	virtual std::optional<bool> IsEnabled() const = 0;
	virtual void Enable(bool enable) = 0;

	virtual std::optional<RGBAColor> GetColor() const = 0;
	virtual void SetColor(RGBAColor color) = 0;

	virtual std::unique_ptr<IStyle> Clone() const = 0;

	virtual ~IStyle() = default;
};

class Style final : public IStyle
{
public:
	Style(bool enabled, RGBAColor color)
		: m_enabled(enabled)
		, m_color(color)
	{
	}

	std::optional<bool> IsEnabled() const override
	{
		return m_enabled;
	}

	void Enable(bool enable) override
	{
		m_enabled = enable;
	}

	std::optional<RGBAColor> GetColor() const override
	{
		if (m_enabled.value_or(false))
		{
			return m_color;
		}
		if (m_color.has_value())
		{
			return m_color.value() & 0x00FFFFFF;
		}
		return m_color;
	}

	void SetColor(RGBAColor color) override
	{
		m_color = color;
	}

	std::unique_ptr<IStyle> Clone() const override
	{
		return std::make_unique<Style>(
			m_enabled.value_or(false), m_color.value_or(0));
	}

private:
	std::optional<bool> m_enabled;
	std::optional<RGBAColor> m_color;
};

#endif // COMMONTYPES_H