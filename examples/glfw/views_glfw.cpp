/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Views

This demonstrates how the hierarchical structure of Views works and some of the
basic View properties.

*/

#include "glv_glfw.h"

int main(){

	// Create the Views
	GLV glv;
	View v(Rect(100,100, 600,400));
	View v1(Rect(10,10, 300,200)), v2(Rect(v1.right()+10,10, 100,200));
	View v11(Rect(20,20, 80,100)), v12(Rect(80,80,100,80));

	// Create the tree hierarchy 
	glv << v;
	v << v1 << v2;
	v1 << v11 << v12;
	
	// Set properties of Views	
	View* views[] = {&v, &v1, &v2, &v11, &v12};
	for(int i=0; i<5; ++i){
		views[i]->addHandler(Event::MouseDrag, Behavior::mouseMove);
		//views[i]->enable(KeepWithinParent);
	}
	
	// Disable some of the default View properties
	v.disable(DrawBack);
	v2.disable(DrawBorder);
	v12.disable(FocusHighlight);
	
	// Set color styles
	glv.cloneStyle().colors().set(StyleColor::WhiteOnBlack);
	v1.colors().set(Color(0.2,0.4,1,0.8), 0.7);
			
	if(!glfwInit()) return 1;
	glv_glfw::windowHints();
	GLFWwindow * win = glfwCreateWindow(800, 600, "Views (glfw)", NULL, NULL);
	if(!win){ glfwTerminate(); return 1; }
	glfwMakeContextCurrent(win);
	glfwSwapInterval(1);
	glv_glfw::attach(win, glv);

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

