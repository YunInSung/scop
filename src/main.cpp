#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "context.h"

const int width = WINDOW_WIDTH;
const int height = WINDOW_HEIGHT;

void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
	SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
	glViewport(0, 0, width, height);
}

void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	SPDLOG_INFO("key: {}, scancode: {}, mods: {}{}{}", \
		key, scancode, \
		action == GLFW_PRESS ? "Pressed" : \
		action == GLFW_RELEASE ?  "Released" : \
		action == GLFW_REPEAT ? "Repeat" : "Unknown",
		mods & GLFW_MOD_CONTROL ? "C" : "-",
		mods & GLFW_MOD_SHIFT ? "S" : "_",
		mods & GLFW_MOD_ALT ? "A" : "-");
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

int main(int ac, char **av)
{
	std::cout << "Initialize glfw" << std::endl;
	if (!glfwInit())
	{
		const char *description = NULL;
		glfwGetError(&description);
		std::cout << "Failed to initialize glfw: " << description << std::endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
	if (!window)
	{
		std::cout << "failed to create glfw window" << std::endl;
		glfwTerminate();

		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "failed to initialize glad" << std::endl;
		glfwTerminate();
		return -1;
	}

	auto context = Context::Create();
	if (!context)
	{
		SPDLOG_ERROR("failed to create context");
		glfwTerminate();
		return -1;
	}

	// OnFramebufferSizeChange(window, width, height);
	glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
	glfwSetKeyCallback(window, OnKeyEvent);

	while (!glfwWindowShouldClose(window))
	{
		context->Render();
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	context.reset();
	// context = nullptr;

	glfwTerminate();
}
