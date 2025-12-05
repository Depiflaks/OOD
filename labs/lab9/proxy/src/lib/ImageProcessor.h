//
// Created by smmm on 05.12.2025.
//

#ifndef OOD_IMAGEPROCESSOR_H
#define OOD_IMAGEPROCESSOR_H
#include "Image.h"

#include <fstream>

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
		for (Color ch : line)
		{
			img.SetPixel({ x++, y }, ch);
		}
	}

	return img;
};

inline void SaveImage(const Image& image, const std::string& dst)
{
	std::ofstream out(dst, std::ios::binary);
	const auto size = image.GetSize();
	out << "P3\n" << size.width << " " << size.height << "\n255\n";
	for (int y = 0; y < size.height; ++y)
		for (int x = 0; x < size.width; ++x)
		{
			Color c = static_cast<unsigned char>(image.GetPixel({ x, y }));
			unsigned r = (c >> 16) & 0xFF;
			unsigned g = (c >> 8) & 0xFF;
			unsigned b = c & 0xFF;
			out << r << " " << g << " " << b << "\n";
		}
}

inline Image ImportImage(const std::string& source)
{
	std::ifstream in(source, std::ios::binary);
	std::string magic;
	in >> magic;
	int w, h, maxv;
	in >> w >> h >> maxv;
	Image img({ w, h });
	for (int y = 0; y < h; ++y)
		for (int x = 0; x < w; ++x)
		{
			int r, g, b;
			in >> r >> g >> b;
			Color c = (r << 16) | (g << 8) | b;
			img.SetPixel({ x, y }, static_cast<char>(c));
		}
	return img;
}
#endif // OOD_IMAGEPROCESSOR_H
