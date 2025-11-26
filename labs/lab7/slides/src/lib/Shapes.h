//
// Created by smmm on 27.11.2025.
//

#ifndef OOD_SHAPES_H
#define OOD_SHAPES_H
#include <optional>

#include "Canvas.h"

#include <functional>
#include <limits>
#include <memory>

using std::optional;

class IDrawable
{
public:
	virtual void Draw(ICanvas& canvas) const = 0;

	virtual ~IDrawable() = default;
};

class IStyle
{
public:
	virtual optional<bool> IsEnabled() const = 0;
	virtual void Enable(bool enable) = 0;

	virtual optional<RGBAColor> GetColor() const = 0;
	virtual void SetColor(RGBAColor color) = 0;

	virtual ~IStyle() = default;
};

class IGroupShape;

class IShape : public IDrawable
{
public:
	virtual RectD GetFrame() = 0;
	virtual void SetFrame(const RectD& rect) = 0;

	virtual IStyle& GetOutlineStyle() = 0;
	virtual const IStyle& GetOutlineStyle() const = 0;

	virtual IStyle& GetFillStyle() = 0;
	virtual const IStyle& GetFillStyle() const = 0;

	virtual std::shared_ptr<IGroupShape> GetGroup() = 0;
	virtual std::shared_ptr<const IGroupShape> GetGroup() const = 0;
};

class IShapes
{
public:
	virtual size_t GetShapesCount() const = 0;
	virtual void InsertShape(const std::shared_ptr<IShape>& shape,
		size_t position = std::numeric_limits<size_t>::max())
		= 0;
	virtual std::shared_ptr<IShape> GetShapeAtIndex(size_t index) = 0;
	virtual void RemoveShapeAtIndex(size_t index) = 0;

	virtual ~IShapes() = default;
};

class IGroupShape
	: public IShape
	, public IShapes
{
};

typedef std::function<void(ICanvas& canvas, const IShape& shape)>
	DrawingStrategy;

class SimpleShape : public IShape
{
public:
	explicit SimpleShape(const DrawingStrategy& drawingStrategy)
	{
		(void)&drawingStrategy;
	}
};

class GroupShape : public IGroupShape
{
};

class ISlide : public IDrawable
{
public:
	virtual double GetWidth() const = 0;
	virtual double GetHeight() const = 0;

	virtual IShapes& GetShapes() const = 0;

	~ISlide() override = default;
};

class Slide : public ISlide
{
};

#endif // OOD_SHAPES_H
