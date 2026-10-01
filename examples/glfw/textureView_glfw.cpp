/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Textured View

This demonstrates how to create a View subclass that draws a texture.

*/

#include "glv_glfw.h"

class TextureView : public View{
public:
	TextureView(const Rect& r = Rect(100,100))
	:	View(r), mTex(256,256,GL_RGB,GL_UNSIGNED_BYTE)
	{
		// Generate a pattern for the texture...
		unsigned char * pixs = mTex.buffer<unsigned char>();
		for(int j=0; j<mTex.height(); ++j){
		for(int i=0; i<mTex.width(); ++i){
			int k = j*mTex.width()+i;
			pixs[k*3  ] = i*13 ^ j*3; 
			pixs[k*3+1] = i* 1 ^ j*2;
			pixs[k*3+2] = 0;
		}}
		mTex.magFilter(GL_NEAREST);
	}


	bool onEvent(Event::t e, GLV& g){
		switch(e){
		case Event::WindowCreate:
			mTex.recreate();				// Create texture on GPU
			mTex.send();					// Send over texel data
			break;
		case Event::WindowDestroy:
			mTex.destroy();					// Destroy texture on GPU
			break;
		default:;
		}
		return true;
	}


	virtual void onDraw(GLV& g){
		draw::enable(draw::Texture2D);		// Enable texture mapping
		mTex.begin();						// Bind texture
		draw::color(1,1,1,1);				// Set current color
		mTex.draw(0,0, width(),height());	// Draw a textured quad filling the view's area
		mTex.end();							// Unbind texture
		draw::disable(draw::Texture2D);		// Disable texture mapping
	}

private:
	Texture2 mTex;
};


int main(){
	GLV glv;
	TextureView v(Rect(20,20,-40,-40));
	v.stretch(1,1);
	glv << v;
	if(!glfwInit()) return 1;
	glv_glfw::windowHints();
	GLFWwindow * win = glfwCreateWindow(600, 600, "Textured View (glfw)", NULL, NULL);
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

