#ifndef ISHAPEMODEL_H
#define ISHAPEMODEL_H

#include "../../GFX/ICanvas.h"
#include "../Geometry.h"

class IShapeModel
{
public:
	virtual ~IShapeModel() = default;
	virtual Bounds GetBounds() = 0;
	virtual void Draw(ICanvas& canvas, const Color& fillColor) = 0;
};

#endif /* ISHAPEMODEL_H */
