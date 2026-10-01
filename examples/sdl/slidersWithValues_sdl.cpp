/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Sliders With Values

This demonstrates how to make individual value labels for each slider of a
Sliders object.
*/

#include "glv_sdl.h"

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
	if(!SDL_Init(SDL_INIT_VIDEO)) return 1;
	glv_sdl::windowHints();
	SDL_Window * win = SDL_CreateWindow("slidersWithValues (sdl)", (int)top.w, (int)top.h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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
}

