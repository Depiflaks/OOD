//
// Created by smmm on 10.05.2025.
//

#ifndef COLOR_TYPE_H
#define COLOR_TYPE_H

#include <cstdint>
#include <ostream>


class Color
{
public:
	Color();
	Color(uint8_t r, uint8_t g, uint8_t b);
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	explicit Color(uint32_t rgba);

	[[nodiscard]] uint8_t GetRed() const;
	[[nodiscard]] uint8_t GetGreen() const;
	[[nodiscard]] uint8_t GetBlue() const;
	[[nodiscard]] uint8_t GetAlpha() const;
	[[nodiscard]] uint32_t GetRGBA() const;

	void SetRed(uint8_t r);
	void SetGreen(uint8_t g);
	void SetBlue(uint8_t b);
	void SetAlpha(uint8_t a);
	void SetRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	void SetRGBA(uint32_t rgba);

	friend std::ostream& operator<<(std::ostream& os, const Color& color);
	friend std::istream& operator>>(std::istream& is, Color& color);
	bool operator==(const Color& other) const;

private:
	uint32_t m_color{};
};

const Color k_black{};
const Color k_white{ 0xff, 0xff, 0xff, 0xff };
const Color k_empty{ 0xff, 0xff, 0xff, 0x00 };

#endif // COLOR_TYPE_H
