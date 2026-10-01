/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */
/*
Example: 
*/

#include "glv_glfw.h"

int main(){

	GLV top;
	if(!glfwInit()) return 1;
	glv_glfw::windowHints();
	GLFWwindow * win = glfwCreateWindow(800, 800, "Grid (glfw)", NULL, NULL);
	if(!win){ glfwTerminate(); return 1; }
	glfwMakeContextCurrent(win);
	glfwSwapInterval(1);
	glv_glfw::attach(win, top);
	
	top.colors().set(StyleColor::WhiteOnBlack);
	//top.colors().set(StyleColor::SmokeyGray);
	
	Grid v1(Rect(0,0));
	
	v1.range(1);			// set plot region
	v1.major(1);			// set major tick mark placement
	v1.minor(2);			// number of divisions per major ticks
	v1.equalizeAxes(true);
	
	v1.stretch(1,1);
	//v1.disable(CropSelf);
	top << v1;

	top.addHandler(Event::KeyDown, *new glv_glfw::FullScreenToggler(win));

	double prev = glfwGetTime();
	while(!glfwWindowShouldClose(win)){
		glfwPollEvents();
		double now = glfwGetTime();
		glv_glfw::draw(win, top, now - prev);
		prev = now;
		glfwSwapBuffers(win);
	}
	glv_glfw::detach(win, top);
	glfwDestroyWindow(win);
	glfwTerminate();
}

