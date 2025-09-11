#ifndef MOCKCANVAS_H
#define MOCKCANVAS_H

#include "../src/GFX/ICanvas.h"
#include <gmock/gmock.h>

class MockCanvas : public ICanvas
{
public:
	MOCK_METHOD(void, MoveTo, (double x, double y), (override));
	MOCK_METHOD(void, SetColor, (const Color& color), (override));
	MOCK_METHOD(void, LineTo, (double x, double y), (override));
	MOCK_METHOD(void, DrawEllipse, (double cx, double cy, double rx, double ry), (override));
	MOCK_METHOD(void, DrawText, (double left, double top, double fontSize, const std::string& text), (override));
};

#endif /* MOCKCANVAS_H */
