//
// Created by smmm on 05.12.2025.
//

#ifndef OOD_IMAGEPROCESSOR_H
#define OOD_IMAGEPROCESSOR_H
#include "Image.h"

#include <fstream>

inline void Print(const Image& img, std::ostream& out)
{
	const auto size = img.GetSize();
	for (int y = 0; y < size.height; ++y)
	{
		for (int x = 0; x < size.width; ++x)
		{
			Color c = img.GetPixel({ x, y });
			out.put(static_cast<char>(c & 0xFF));
		}
		out.put('\n');
	}
}

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

	Image img(size, 0);

	s.clear();
	s.str(pixels);
	for (int y = 0; y < size.height; ++y)
	{
		if (!std::getline(s, line))
			break;

		int x = 0;
		for (char ch : line)
		{
			img.SetPixel({ x++, y }, static_cast<unsigned char>(ch));
		}
	}

	return img;
}

inline void SaveImage(const Image& image, const std::string& dst)
{
	std::ofstream out(dst, std::ios::binary);
	const auto size = image.GetSize();
	out << "P3\n" << size.width << " " << size.height << "\n255\n";
	for (int y = 0; y < size.height; ++y)
	{
		for (int x = 0; x < size.width; ++x)
		{
			Color c = image.GetPixel({ x, y });
			unsigned r = c >> 16 & 0xFF;
			unsigned g = c >> 8 & 0xFF;
			unsigned b = c & 0xFF;
			out << r << ' ' << g << ' ' << b << '\n';
		}
	}
}

inline Image ImportImage(const std::string& source)
{
	std::ifstream in(source, std::ios::binary);
	std::string magic;
	in >> magic;
	int width = 0;
	int height = 0;
	int maxv = 0;
	in >> width >> height >> maxv;
	Image img({ width, height }, 0);
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int r = 0;
			int g = 0;
			int b = 0;
			if (!(in >> r >> g >> b))
				break;
			if (maxv != 255 && maxv > 0)
			{
				r = r * 255 / maxv;
				g = g * 255 / maxv;
				b = b * 255 / maxv;
			}
			Color c = (static_cast<Color>(r & 0xFF) << 16)
				| (static_cast<Color>(g & 0xFF) << 8)
				| static_cast<Color>(b & 0xFF);
			img.SetPixel({ x, y }, c);
		}
	}
	return img;
}
#endif // OOD_IMAGEPROCESSOR_H
