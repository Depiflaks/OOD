#ifndef ICANVAS_H
#define ICANVAS_H

#include "./Color.h"

class ICanvas {
public:
    virtual ~ICanvas() = default;
    virtual void MoveTo(double x, double y) = 0;
    virtual void SetColor(const Color& color) = 0;
    virtual void LineTo(double x, double y) = 0;
    virtual void DrawEllipse(double cx, double cy, double rx, double ry) = 0;
    virtual void DrawText(double left, double top, double fontSize, const std::string& text) = 0;
};

#endif /* ICANVAS_H */

