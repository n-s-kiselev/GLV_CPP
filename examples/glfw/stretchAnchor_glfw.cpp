/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Using stretch and anchor

This demonstrates how the stretch and acnhor factors can be used to control
the resizing behavior of child Views.

*/

#include "glv_glfw.h"

int main(int argc, char ** argv){

	GLV glv;
	View v1(Rect(100));
	View v2(Rect( 40));
	
	glv.colors().set(StyleColor::WhiteOnBlack);
	glv.cloneStyle();
	v1.colors().set(StyleColor::SmokyGray);
	
	glv << v1;
	v1 << v2;
		
	if(!glfwInit()) return 1;
	glv_glfw::windowHints();
	GLFWwindow * win = glfwCreateWindow(200, 200, "stretchAnchor (glfw)", NULL, NULL);
	if(!win){ glfwTerminate(); return 1; }
	glfwMakeContextCurrent(win);
	glfwSwapInterval(1);
	glv_glfw::attach(win, glv);

	v1.stretch(1,1);
	v1.anchor(0,0);
	
	v2.anchor(1,1);

	double prev = glfwGetTime();
	while(!glfwWindowShouldClose(win)){
		glfwPollEvents();
		double now = glfwGetTime();
		glv_glfw::draw(win, glv, now - prev);
		prev = now;
		glfwSwapBuffers(win);
	}
	glv_glfw::detach(win, glv);
	glfwDestroyWindow(win);
	glfwTerminate();
}

