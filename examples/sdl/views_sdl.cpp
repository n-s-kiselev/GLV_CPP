/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Views

This demonstrates how the hierarchical structure of Views works and some of the
basic View properties.

*/

#include "glv_sdl.h"

int main(){

	// Create the Views
	GLV glv;
	View v(Rect(100,100, 600,400));
	View v1(Rect(10,10, 300,200)), v2(Rect(v1.right()+10,10, 100,200));
	View v11(Rect(20,20, 80,100)), v12(Rect(80,80,100,80));

	// Create the tree hierarchy 
	glv << v;
	v << v1 << v2;
	v1 << v11 << v12;
	
	// Set properties of Views	
	View* views[] = {&v, &v1, &v2, &v11, &v12};
	for(int i=0; i<5; ++i){
		views[i]->addHandler(Event::MouseDrag, Behavior::mouseMove);
		//views[i]->enable(KeepWithinParent);
	}
	
	// Disable some of the default View properties
	v.disable(DrawBack);
	v2.disable(DrawBorder);
	v12.disable(FocusHighlight);
	
	// Set color styles
	glv.cloneStyle().colors().set(StyleColor::WhiteOnBlack);
	v1.colors().set(Color(0.2,0.4,1,0.8), 0.7);
			
	if(!SDL_Init(SDL_INIT_VIDEO)) return 1;
	glv_sdl::windowHints();
	SDL_Window * win = SDL_CreateWindow("Views (sdl)", 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

