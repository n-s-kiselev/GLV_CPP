/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Textured View

This demonstrates how to create a View subclass that draws a texture.

*/

#include "glv_sdl.h"

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
	if(!SDL_Init(SDL_INIT_VIDEO)) return 1;
	glv_sdl::windowHints();
	SDL_Window * win = SDL_CreateWindow("Textured View (sdl)", 600, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if(!win){ SDL_Quit(); return 1; }
	SDL_GLContext ctx = SDL_GL_CreateContext(win);
	SDL_GL_MakeCurrent(win, ctx);
	SDL_GL_SetSwapInterval(1);
	glv_sdl::attach(win, glv);
	Uint64 prev = SDL_GetTicksNS();
	bool running = true;
	while(running){
		SDL_Event ev;
		while(SDL_PollEvent(&ev)){
			if(!glv_sdl::processEvent(win, glv, ev)) running = false;
		}
		Uint64 now = SDL_GetTicksNS();
		glv_sdl::draw(win, glv, (now - prev) * 1e-9);
		prev = now;
		SDL_GL_SwapWindow(win);
	}
	glv_sdl::detach(win, glv);
	SDL_GL_DestroyContext(ctx);
	SDL_DestroyWindow(win);
	SDL_Quit();
}

