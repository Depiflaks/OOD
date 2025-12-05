//
// Created by smmm on 05.12.2025.
//

#ifndef OOD_DRAWER_H
#define OOD_DRAWER_H
#include "Image.h"

/**
 * Рисует отрезок прямой линии между точками from и to цветом color на
 * изображении Image.
 *
 * Для рисования используется алгоритм Брезенхэма.
 * (https://ru.wikipedia.org/wiki/Алгоритм_Брезенхэма)
 */
void DrawLine(Image& image, Point from, Point to, Color color);
void DrawCircle(Image& image, Point center, int radius, Color color);
void FillCircle(Image& image, Point center, int radius, Color color);

#endif // OOD_DRAWER_H
