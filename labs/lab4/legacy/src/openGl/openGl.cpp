//
// Created by smmm on 13.05.2025.
//
#include "openGl.h"
#include <GLFW/glfw3.h>
#include <canvas/openGL/GlCanvas.h>
#include <functional>

void InitOpenGl()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
}

void DrawUntilExitWindow(GLFWwindow* window, const std::function<void()>& Draw)
{
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		Draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void StopDrawing(GLFWwindow* window)
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void CheckOpenGlAvailable()
{
	if (!glfwInit())
		throw OpenGlException();
}

void CheckWindowCreationSuccessful(const GLFWwindow* window)
{
	if (!window)
	{
		glfwTerminate();
		throw OpenGlException();
	}
}

void DrawPicture(const std::vector<std::shared_ptr<Shape>>& shapes)
{
	CheckOpenGlAvailable();
	InitOpenGl();

	GLFWwindow* window = glfwCreateWindow(k_windowWidth, k_windowHeight, k_windowCapture, nullptr, nullptr);
	CheckWindowCreationSuccessful(window);
	glfwMakeContextCurrent(window);

	GlCanvas canvas;

	const std::function DrawShapes{ [&shapes, &canvas] {
		for (const auto& shape : shapes)
			shape->Draw(canvas);
	} };

	DrawUntilExitWindow(window, DrawShapes);
	StopDrawing(window);
}
