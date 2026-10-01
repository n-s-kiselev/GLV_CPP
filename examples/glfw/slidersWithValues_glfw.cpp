/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Sliders With Values

This demonstrates how to make individual value labels for each slider of a
Sliders object.
*/

#include "glv_glfw.h"

// Notification callback to set label string to slider value
void ntSetLabel(const Notification& n){
	Label * l = n.receiver<Label>();
	Sliders& s = *n.sender<Sliders>();
	
	// Get currently selected slider
	int idx = s.selected();
	
	// Set string of corresponding label to slider value
	l[idx].setValue(glv::toString(s.getValue(idx)));
}


int main(){

	const int numSliders = 4;

	GLV top;
	Sliders sliders(Rect(200,80), 1, numSliders);
	Label labels[numSliders];
	
	for(int i=0; i<numSliders; ++i){
		labels[i].pos(2,4);
		
		// Set anchor factor so labels lie on top of each slider
		labels[i].anchor(0, float(i)/numSliders);

		sliders << labels[i];
	}
	
	// Set notification callback for whenever a slider changes value
	sliders.attach(ntSetLabel, Update::Value, labels);
	sliders.colors().set(StyleColor::SmokyGray);
	
	top << sliders;

	top.fit();
	if(!glfwInit()) return 1;
	glv_glfw::windowHints();
	GLFWwindow * win = glfwCreateWindow((int)top.w, (int)top.h, "slidersWithValues (glfw)", NULL, NULL);
	if(!win){ glfwTerminate(); return 1; }
	glfwMakeContextCurrent(win);
	glfwSwapInterval(1);
	glv_glfw::attach(win, top);
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

