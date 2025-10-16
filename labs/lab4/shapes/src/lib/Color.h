#ifndef COLOR_H
#define COLOR_H

#include <cstdint>
#include <string>
struct Color
{
	Color(const std::string& hexColor)
	{
		if (hexColor.size() == 9 && hexColor[0] == '#')
		{
			r = std::stoi(hexColor.substr(1, 2), nullptr, 16);
			g = std::stoi(hexColor.substr(3, 2), nullptr, 16);
			b = std::stoi(hexColor.substr(5, 2), nullptr, 16);
			a = std::stoi(hexColor.substr(7, 2), nullptr, 16);
		}
	}

	std::string ToHexString() const
	{
		char buffer[10];
		snprintf(buffer, sizeof(buffer), "#%02x%02x%02x%02x", r, g, b, a);
		return std::string(buffer);
	}

	bool operator==(const Color& other) const
	{
		return r == other.r && g == other.g && b == other.b && a == other.a;
	}

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	uint8_t a = 255;
};

#endif /* COLOR_H */
