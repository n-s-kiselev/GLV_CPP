/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Using a Notification to set a Label

A Notification is used to set a label to a slider's value.

*/

#include "glv_glfw.h"

void ntSetLabel(const Notification& n){
	Label& l = *n.receiver<Label>();
	Slider& s = *n.sender<Slider>();
	l.setValue(s.data().toString());
}

int main(int argc, char ** argv){

	GLV glv;
	Slider slider(Rect(100,20));
	Label label;
	
	slider.attach(ntSetLabel, Update::Value, &label);

	Placer p(glv, Direction::E, Place::TL, 10, 10);
	
	p << slider << label;

	if(!glfwInit()) return 1;
	glv_glfw::windowHints();
	GLFWwindow * win = glfwCreateWindow(200, 40, "notification (glfw)", NULL, NULL);
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

