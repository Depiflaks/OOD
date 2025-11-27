#ifndef COMMONTYPES_H
#define COMMONTYPES_H
#include <cstdint>
#include <optional>

using RGBAColor = uint32_t;

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

	virtual ~IStyle() = default;
};

#endif // COMMONTYPES_H