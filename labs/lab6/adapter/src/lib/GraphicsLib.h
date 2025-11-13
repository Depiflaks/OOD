#ifndef GRAPHICSLIB_H
#define GRAPHICSLIB_H

#include <cstdint>
namespace graphics_lib
{
class ICanvas
{
public:
	// Установка цвета в формате 0xRRGGBB
	virtual void SetColor(uint32_t rgbColor) = 0;
	virtual void MoveTo(int x, int y) = 0;
	virtual void LineTo(int x, int y) = 0;
	virtual ~ICanvas() = default;
};

class Canvas : public ICanvas
{
public:
	void SetColor(uint32_t rgbColor) override
	{
		// TODO: вывести в output цвет в виде строки SetColor (#RRGGBB)
	}

	void MoveTo(int x, int y) override
	{
		// Реализация остается без изменения
	}

	void LineTo(int x, int y) override
	{
		// Реализация остается без изменения
	}
};
} // namespace graphics_lib

#endif /* GRAPHICSLIB_H */
