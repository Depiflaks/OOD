#ifndef PAINTER_H
#define PAINTER_H
#include "lib/Shape.h"
class Painter
{
public:
	void DrawPicture(PictureDraft& draft, ICanvas& canvas)
	{
		for (size_t i = 0; i < draft.GetShapeCount(); ++i)
		{
			Shape& shape = draft.GetShape(i);
			shape.Draw(canvas);
		}
		canvas.Render();
	}
};
#endif /* PAINTER_H */
