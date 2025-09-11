#ifndef ISHAPEMODEL_H
#define ISHAPEMODEL_H

#include "../../GFX/ICanvas.h"
#include "../Geometry.h"

namespace shapes
{

class IShapeModel
{
public:
	virtual ~IShapeModel() = default;
	virtual Bounds GetBounds() = 0;
	virtual void Draw(gfx::ICanvas& canvas, const Color& fillColor) = 0;
	virtual void Move(double dx, double dy) = 0;
};

}; // namespace shapes

#endif /* ISHAPEMODEL_H */
