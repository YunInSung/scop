#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

const int width = WINDOW_WIDTH;
const int height = WINDOW_HEIGHT;

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
	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.1f, 0.2f, 0.0f);
	// glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
	// glfwSetKeyCallback(window, OnKeyEvent);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}
