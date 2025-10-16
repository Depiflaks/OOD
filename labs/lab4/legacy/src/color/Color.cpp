//
// Created by smmm on 10.05.2025.
//

#include "Color.h"

#include <iomanip>
Color::Color()
	: m_color(0x000000FF)
{
}

Color::Color(const uint8_t r, const uint8_t g, const uint8_t b)
{
	SetRGBA(r, g, b, 0xFF);
}

Color::Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
{
	SetRGBA(r, g, b, a);
}

Color::Color(const uint32_t rgba)
	: m_color(rgba)
{
}

uint8_t Color::GetRed() const { return (m_color >> 24) & 0xFF; }
uint8_t Color::GetGreen() const { return (m_color >> 16) & 0xFF; }
uint8_t Color::GetBlue() const { return (m_color >> 8) & 0xFF; }
uint8_t Color::GetAlpha() const { return m_color & 0xFF; }
uint32_t Color::GetRGBA() const { return m_color; }

void Color::SetRed(const uint8_t r) { m_color = (m_color & 0x00FFFFFF) | (r << 24); }
void Color::SetGreen(const uint8_t g) { m_color = (m_color & 0xFF00FFFF) | (g << 16); }
void Color::SetBlue(const uint8_t b) { m_color = (m_color & 0xFFFF00FF) | (b << 8); }
void Color::SetAlpha(const uint8_t a) { m_color = (m_color & 0xFFFFFF00) | a; }

void Color::SetRGBA(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
{
	m_color = (r << 24) | (g << 16) | (b << 8) | a;
}

void Color::SetRGBA(const uint32_t rgba)
{
	m_color = rgba;
}

bool Color::operator==(const Color& other) const
{
	return GetRGBA() == other.GetRGBA();
}

std::ostream& operator<<(std::ostream& os, const Color& color)
{
	const auto f = os.flags();
	os << std::hex << std::setfill('0') << std::setw(8) << color.m_color;
	os.flags(f);
	return os;
}

std::istream& operator>>(std::istream& is, Color& color)
{
	char buffer[9] = {};
	is >> std::ws;
	is.read(buffer, 8);

	if (is.gcount() != 8)
	{
		is.setstate(std::ios::failbit);
		return is;
	}

	uint32_t value;
	std::istringstream(std::string(buffer, 8)) >> std::hex >> value;
	color.SetRGBA(value);
	return is;
}