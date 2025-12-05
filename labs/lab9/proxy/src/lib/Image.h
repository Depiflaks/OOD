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
	explicit Image(Size size, char color = ' ')
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
	char GetPixel(Point p) const noexcept
	{
	}

	/**
	 * Задаёт «цвет» пикселя в указанных координатах. Если координаты выходят за
	 * пределы изображения действие игнорируется.
	 */
	void SetPixel(Point p, char color)
	{
	}

private:
};

/**
 * Выводит в поток out изображение в виде символов.
 */
inline void Print(const Image& img, std::ostream& out)
{
	const auto size = img.GetSize();
	for (int y = 0; y < size.height; ++y)
	{
		for (int x = 0; x < size.width; ++x)
		{
			out.put(img.GetPixel({ x, y }));
		}
		out.put('\n');
	}
};

/**
 * Загружает изображение из pixels. Линии изображения разделяются символами \n.
 * Размеры картинки определяются по количеству переводов строки и самой длинной
 * линии.
 */
inline Image LoadImage(const std::string& pixels)
{
	std::istringstream s(pixels);
	Size size;
	std::string line;
	while (std::getline(s, line))
	{
		size.width = std::max(size.width, static_cast<int>(line.length()));
		++size.height;
	}

	Image img(size);

	s = std::istringstream(pixels);
	for (int y = 0; y < size.height; ++y)
	{
		if (!std::getline(s, line))
			break;

		int x = 0;
		for (char ch : line)
		{
			img.SetPixel({ x++, y }, ch);
		}
	}

	return img;
};
#endif // OOD_IMAGE_H
