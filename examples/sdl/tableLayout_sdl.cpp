/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Table layout

This demonstrates how to construct a layout using a Table.

*/

#include "glv_sdl.h"

int main(){

	GLV glv;
	View v1(Rect(80,40));
	View v2(Rect(40,40));
	View v3(Rect(40,80));
	View v4(Rect(40,40));
	View v5(Rect(80,80));
	View v6(Rect(40,40));

	const char * layout = 
		". x - x,"
		"x x x -,"
		"| x | . ";

	Table table(layout);

	glv << (table << v1 << v2 << v3 << v4 << v5 << v6);

	table.arrange();
			
	if(!SDL_Init(SDL_INIT_VIDEO)) return 1;
	glv_sdl::windowHints();
	SDL_Window * win = SDL_CreateWindow("Example: Table layout (sdl)", 300, 200, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

