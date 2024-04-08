#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

//After multiple attempts, I could not properly run OpenGL due to denied access (see screenshot attach in submission)
//Spoke with TA Scaramal and I have included New_OpenGL folder that corresponds with main.cpp (downloaded as x64 via Canvas)
//If OpenGL was accessible, following code would represent a sinusoidal wave (electromagnetic waves used in aerospace and physics)
//Sinusoidal wave eqn is y(x,t)=Asin(kx-wt+phase)

int main() {
	if (!glfwInit())
	{
		//initalize GLFW check
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	//window for sinusodial wave
	GLFWwindow* window = glfwCreateWindow(500, 500, "Sinusidal Wave using OpenGL with GLEW and GLFW", NULL, NULL);
	if (!window)
	{
		//window check
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK)
	{
		//initalize GLEW check
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return -1;
	}
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glBegin(GL_LINES);

		//Draw X and Y axis
		glColor3f(1.0, 1.0, 1.0);
		glVertex2f(-1.0, 0.0);
		glVertex2f(1.0, 0.0);
		glVertex2f(0.0, 1.0);
		glVertex2f(0.0, -1.0);

		//Plot sinusodial sine wave y(x,t)=Asin(kx-wt+phase)
		glColor3f(0.0, 1.0, 0.0);
		for (float i = 0; i < 2 * 3.14159; i += 0.001)
		{
			float x = (2 * i - 3.14159) / 3.14159;
			float y = -sin(2 * i); //change period by factor of 2
			glVertex2f(x, y - 0.001);
			glVertex2f(x, y);
		}
		glEnd();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}