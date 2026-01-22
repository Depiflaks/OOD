//
// Created by smmm on 05.12.2025.
//

#ifndef OOD_GEOM_H
#define OOD_GEOM_H
#include <cstdint>

using Color = uint32_t;

struct Point
{
	int x = 0;
	int y = 0;
};

struct Size
{
	int width = 0;
	int height = 0;
};

// Точка передаётся в локальных координатах.
inline bool IsPointInSize(Point p, Size size) noexcept
{
	return p.x >= 0 && p.y >= 0 && p.x < size.width && p.y < size.height;
}

#endif // OOD_GEOM_H
