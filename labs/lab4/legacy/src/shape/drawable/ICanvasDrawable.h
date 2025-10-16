//
// Created by smmm on 06.05.2025.
//

#ifndef I_CANVAS_DRAWABLE_H
#define I_CANVAS_DRAWABLE_H
#include "canvas/ICanvas.h"

class ICanvasDrawable
{
public:
	virtual ~ICanvasDrawable() = default;
	virtual void Draw(ICanvas& canvas) = 0;
};

#endif // I_CANVAS_DRAWABLE_H
