//
// Created by smmm on 05.12.2025.
//

#ifndef OOD_IMAGE_H
#define OOD_IMAGE_H
#include "Geom.h"

#include <iosfwd>
#include <string>

#include "Geom.h"

#include <sstream>

class Image
{
public:
	/**
	 * Конструирует изображение заданного размера. Если размеры не являются
	 * положительными, выбрасывает исключение std::out_of_range.
	 */
	explicit Image(Size size, Color color = ' ')
	{
	}

	// Возвращает размер изображения в пикселях.
	Size GetSize() const noexcept
	{
	}

	/**
	 * Возвращает «цвет» пикселя в указанных координатах.Если координаты выходят
	 * за пределы изображения, возвращает «пробел».
	 */
	Color GetPixel(Point p) const noexcept
	{
	}

	/**
	 * Задаёт «цвет» пикселя в указанных координатах. Если координаты выходят за
	 * пределы изображения действие игнорируется.
	 */
	void SetPixel(Point p, Color color)
	{
	}

private:
};

#endif // OOD_IMAGE_H
