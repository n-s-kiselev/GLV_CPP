/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Attaching variables to numerical widgets

A single variable is attached to two different sliders. This demonstrates how
a slider can automatically update a program variable and how a slider is
automatically synchronized to changes in its attached variable. When one slider
is moved it sets the value of the variable which then causes the other slider
to synchronize to the changed variable. If working correctly, moving one slider
will move the other slider.
*/

#include "glv_sdl.h"

int main(int argc, char ** argv){

	GLV glv;
	Slider sl1(Rect(10,10,200,20)), sl2(Rect(10,40,200,20));
	
	float var = 0;
	
	sl1.attachVariable(var);
	sl2.attachVariable(var);
	
	glv << sl1 << sl2;

	if(!SDL_Init(SDL_INIT_VIDEO)) return 1;
	glv_sdl::windowHints();
	SDL_Window * win = SDL_CreateWindow("attachVariable (sdl)", 220, 70, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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
