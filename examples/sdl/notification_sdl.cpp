/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Using a Notification to set a Label

A Notification is used to set a label to a slider's value.

*/

#include "glv_sdl.h"

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

	if(!SDL_Init(SDL_INIT_VIDEO)) return 1;
	glv_sdl::windowHints();
	SDL_Window * win = SDL_CreateWindow("notification (sdl)", 200, 40, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

