//
// Created by smmm on 27.11.2025.
//

#ifndef OOD_SHAPES_H
#define OOD_SHAPES_H
#include "IShape.h"

#include <functional>

using DrawingStrategy
	= std::function<void(ICanvas& canvas, const IShape& shape)>;

class SimpleShape : public IShape
{
public:
	explicit SimpleShape(const DrawingStrategy& drawingStrategy)
	{
		(void)&drawingStrategy;
	}
};


#endif // OOD_SHAPES_H
