/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */
/*
Example:
Demonstration of a simple painting application using the mouse as a brush.
The left mouse button controls the brush up and down and the right button
controls the eraser up and down.
*/

#include "glv_sdl.h"
using namespace glv;

struct Canvas : public Plot{
	Canvas(const Rect& r): Plot(r){
		data().resize(Data::FLOAT, 1,64,64);
	}

	bool onEvent(Event::t e, GLV& g){

		float gx = pixToGrid(0, g.mouse().xRel());	// convert mouse to grid position
		float gy = pixToGrid(1, g.mouse().yRel());	// convert mouse to grid position
		int cx = (gx*0.5+0.5) * data().size(1);		// convert grid to cell position
		int cy = (gy*0.5+0.5) * data().size(2);		// convert grid to cell position

		switch(e){
		case Event::MouseDown:
		case Event::MouseDrag:
			if(data().inBounds(0, cx, cy)){
				data().assign(g.mouse().button() ? 0:1, 0,cx,cy);
			}
			break;
		case Event::KeyDown:
			switch(g.keyboard().key()){
			case 'c': data().assignAll(0); break;
			}
			break;
		default:;
		}
		
		return true;
	}
};


int main(){
	GLV top;
	Canvas v1(Rect(000,0, 00,00));
	v1.add(*new PlotDensity(Color(1)));
	
	v1.stretch(1,1);
	top << v1;

	if(!SDL_Init(SDL_INIT_VIDEO)) return 1;
	glv_sdl::windowHints();
	SDL_Window * win = SDL_CreateWindow("Simple Paint (sdl)", 600, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if(!win){ SDL_Quit(); return 1; }
	SDL_GLContext ctx = SDL_GL_CreateContext(win);
	SDL_GL_MakeCurrent(win, ctx);
	SDL_GL_SetSwapInterval(1);
	glv_sdl::attach(win, top);
	Uint64 prev = SDL_GetTicksNS();
	bool running = true;
	while(running){
		SDL_Event ev;
		while(SDL_PollEvent(&ev)){
			if(!glv_sdl::processEvent(win, top, ev)) running = false;
		}
		Uint64 now = SDL_GetTicksNS();
		glv_sdl::draw(win, top, (now - prev) * 1e-9);
		prev = now;
		SDL_GL_SwapWindow(win);
	}
	glv_sdl::detach(win, top);
	SDL_GL_DestroyContext(ctx);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}

