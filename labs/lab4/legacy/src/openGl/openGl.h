//
// Created by smmm on 13.05.2025.
//

#ifndef OPENGL_H
#define OPENGL_H
#include <shape/Shape.h>

constexpr int k_windowWidth = 800;
constexpr int k_windowHeight = 600;
inline auto k_windowCapture = "Picture";

class OpenGlException final : public std::runtime_error
{
public:
	OpenGlException()
		: std::runtime_error("Error while trying to start OpenGl Unit")
	{
	}
};

void DrawPicture(const std::vector<std::shared_ptr<Shape>>& shapes);

#endif //OPENGL_H
