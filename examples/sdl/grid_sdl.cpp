/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */
/*
Example: 
*/

#include "glv_sdl.h"

int main(){

	GLV top;
	if(!SDL_Init(SDL_INIT_VIDEO)) return 1;
	glv_sdl::windowHints();
	SDL_Window * win = SDL_CreateWindow("Grid (sdl)", 800, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if(!win){ SDL_Quit(); return 1; }
	SDL_GLContext ctx = SDL_GL_CreateContext(win);
	SDL_GL_MakeCurrent(win, ctx);
	SDL_GL_SetSwapInterval(1);
	glv_sdl::attach(win, top);
	
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

	top.addHandler(Event::KeyDown, *new glv_sdl::FullScreenToggler(win));

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
}

