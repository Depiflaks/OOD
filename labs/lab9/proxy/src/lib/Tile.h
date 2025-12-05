//
// Created by smmm on 05.12.2025.
//

#ifndef OOD_TILE_H
#define OOD_TILE_H
#include <array>
#include <cassert>

#include "Geom.h"

#include <vector>

class Tile
{
public:
	constexpr static int SIZE = 8;

	Tile(Color color = static_cast<Color>(' ')) noexcept
		: m_pixels(SIZE * SIZE, color)
	{
		assert(m_instanceCount >= 0);
		++m_instanceCount;
	}

	Tile(const Tile& other)
		: m_pixels(other.m_pixels)
	{
		assert(m_instanceCount >= 0);
		++m_instanceCount;
	}

	~Tile()
	{
		--m_instanceCount;
		assert(m_instanceCount >= 0);
	}

	void SetPixel(Point p, Color color) noexcept
	{
		if (!IsPointInSize(p, { SIZE, SIZE }))
		{
			return;
		}
		m_pixels[p.y * SIZE + p.x] = color;
	}

	Color GetPixel(Point p) const noexcept
	{
		if (!IsPointInSize(p, { SIZE, SIZE }))
		{
			return static_cast<Color>(' ');
		}
		return m_pixels[p.y * SIZE + p.x];
	}

	static int GetInstanceCount() noexcept
	{
		return m_instanceCount;
	}

private:
	inline static int m_instanceCount = 0;
	std::vector<Color> m_pixels;
};
#endif // OOD_TILE_H
