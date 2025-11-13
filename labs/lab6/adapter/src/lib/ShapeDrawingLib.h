#ifndef SHAPEDRAWINGLIB_H
#define SHAPEDRAWINGLIB_H

#include "GraphicsLib.h"
namespace shape_drawing_lib
{

struct Point
{
	int x;
	int y;
};

class ICanvasDrawable
{
public:
	virtual void Draw(graphics_lib::ICanvas& canvas) const = 0;
	virtual ~ICanvasDrawable() = default;
};

class Triangle : public ICanvasDrawable
{
public:
	Triangle(const Point& p1, const Point& p2, const Point& p3)
	{
		// TODO: написать код конструктора
	}

	void Draw(graphics_lib::ICanvas& canvas) const override
	{
		// TODO: написать код рисования треугольника на холсте
	}

private:
	// TODO: дописать приватную часть
};

class Rectangle : public ICanvasDrawable
{
public:
	Rectangle(const Point& leftTop, int width, int height)
	{
		// TODO: написать код конструктора
	}
	void Draw(graphics_lib::ICanvas& canvas) const override
	{
		// TODO: написать код рисования прямоугольника на холсте
	}

private:
	// TODO: дописать приватную часть
};

// Художник, способный рисовать ICanvasDrawable-объекты на ICanvas
class CanvasPainter
{
public:
	CanvasPainter(graphics_lib::ICanvas& canvas)
	{
		// TODO: дописать конструктор класса
	}
	void Draw(const ICanvasDrawable& drawable)
	{
		// TODO: дописать код рисования ICanvasDrawable на canvas, переданном в
		// конструктор
	}

private:
	// TODO: дописать приватную часть
};
} // namespace shape_drawing_lib

#endif /* SHAPEDRAWINGLIB_H */
