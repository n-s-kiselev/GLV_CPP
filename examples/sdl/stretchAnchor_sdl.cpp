/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Using stretch and anchor

This demonstrates how the stretch and acnhor factors can be used to control
the resizing behavior of child Views.

*/

#include "glv_sdl.h"

int main(int argc, char ** argv){

	GLV glv;
	View v1(Rect(100));
	View v2(Rect( 40));
	
	glv.colors().set(StyleColor::WhiteOnBlack);
	glv.cloneStyle();
	v1.colors().set(StyleColor::SmokyGray);
	
	glv << v1;
	v1 << v2;
		
	if(!SDL_Init(SDL_INIT_VIDEO)) return 1;
	glv_sdl::windowHints();
	SDL_Window * win = SDL_CreateWindow("stretchAnchor (sdl)", 200, 200, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if(!win){ SDL_Quit(); return 1; }
	SDL_GLContext ctx = SDL_GL_CreateContext(win);
	SDL_GL_MakeCurrent(win, ctx);
	SDL_GL_SetSwapInterval(1);
	glv_sdl::attach(win, glv);

	v1.stretch(1,1);
	v1.anchor(0,0);
	
	v2.anchor(1,1);

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

