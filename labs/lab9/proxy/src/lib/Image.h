//
// Created by smmm on 05.12.2025.
//

#ifndef OOD_IMAGE_H
#define OOD_IMAGE_H
#include "CoW.h"
#include "Geom.h"

#include "Tile.h"

#include <sstream>

class Image
{
public:
	explicit Image(Size size, Color color = static_cast<Color>(' '))
		: m_size(size)
	{
		if (size.width <= 0 || size.height <= 0)
		{
			throw std::out_of_range("Image size must be positive");
		}
		m_tilesX = (size.width + Tile::SIZE - 1) / Tile::SIZE;
		m_tilesY = (size.height + Tile::SIZE - 1) / Tile::SIZE;
		m_tiles.reserve(static_cast<size_t>(m_tilesX) * m_tilesY);
		for (int i = 0; i < m_tilesX * m_tilesY; ++i)
		{
			m_tiles.emplace_back(Tile(color));
		}
	}

	Size GetSize() const noexcept
	{
		return m_size;
	}

	Color GetPixel(Point p) const noexcept
	{
		if (!IsPointInSize(p, m_size))
		{
			return 0;
		}
		int tileX = p.x / Tile::SIZE;
		int tileY = p.y / Tile::SIZE;
		int index = tileY * m_tilesX + tileX;
		Point local{ p.x % Tile::SIZE, p.y % Tile::SIZE };
		return m_tiles[static_cast<size_t>(index)]->GetPixel(local);
	}

	void SetPixel(Point p, Color color)
	{
		if (!IsPointInSize(p, m_size))
		{
			return;
		}
		int tileX = p.x / Tile::SIZE;
		int tileY = p.y / Tile::SIZE;
		int index = tileY * m_tilesX + tileX;
		Point local{ p.x % Tile::SIZE, p.y % Tile::SIZE };
		m_tiles[static_cast<size_t>(index)].Write()->SetPixel(local, color);
	}

private:
	Size m_size{};
	int m_tilesX = 0;
	int m_tilesY = 0;
	std::vector<CoW<Tile>> m_tiles;
};

#endif // OOD_IMAGE_H
