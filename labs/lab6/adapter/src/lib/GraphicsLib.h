#ifndef GRAPHICSLIB_H
#define GRAPHICSLIB_H

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>

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
	explicit Canvas(std::ostream& output)
		: m_output(output)
	{
	}

	void SetColor(uint32_t rgbColor) override
	{
		std::ostringstream colorStream;
		colorStream << "#" << std::hex << std::uppercase << std::setfill('0')
					<< std::setw(6) << (rgbColor & 0xFFFFFF);
		m_output << "SetColor (" << colorStream.str() << ")" << std::endl;
	}

	void MoveTo(int x, int y) override
	{
		m_output << "MoveTo (" << x << ", " << y << ")" << std::endl;
	}

	void LineTo(int x, int y) override
	{
		m_output << "LineTo (" << x << ", " << y << ")" << std::endl;
	}

private:
	std::ostream& m_output;
};
} // namespace graphics_lib

#endif /* GRAPHICSLIB_H */
