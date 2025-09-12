#ifndef ISHAPE_H
#define ISHAPE_H

#include "../GFX/ICanvas.h"
#include "./Models/IShapeModel.h"
#include "Shapes/Geometry.h"
#include <memory>

namespace shapes
{

class IShape
{
public:
	virtual ~IShape() = default;
	virtual void Draw(gfx::ICanvas& canvas) = 0;
	// TODO: навести красоту со константыми методами
	virtual void SetFillColor(const Color& color) = 0;
	virtual Bounds GetBounds() = 0;
	virtual void ChangeShape(std::unique_ptr<IShapeModel> model) = 0;
	virtual void Move(double dx, double dy) = 0;
	virtual std::string GetId() const = 0;
	virtual std::string ToString() const = 0;
};

} // namespace shapes

#endif /* ISHAPE_H */
