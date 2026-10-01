/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: GraphicsData

This demonstrates how to use GraphicsData to draw a heptagram shape.
*/

#include "glv_sdl.h"

struct Scene : View3D{

	virtual void onDraw3D(GLV& g){

		GraphicsData& gd = g.graphicsData();
		
		draw::lineWidth(2);

		// generate rainbow heptagon
		for(int i=0; i<7; ++i){
			float p = float(i)/7;
			gd.addColor(HSV(p));
			gd.addVertex(cos(p*2*M_PI), sin(p*2*M_PI), -3);
		}

		// draw heptagon
		draw::paint(draw::LineLoop, gd);

		// generate indices 0, 3, 6, 2, 5, 1, 4
		for(int i=0; i<7; ++i){
			gd.addIndex(i*3 % 7);
		}

		// draw star heptagon
		draw::paint(draw::LineLoop, gd);
	}
};


int main (int argc, char ** argv){

	GLV top;
	Scene scene;
	
	scene.stretch(1,1);
	
	top << scene;

	if(!SDL_Init(SDL_INIT_VIDEO)) return 1;
	glv_sdl::windowHints();
	SDL_Window * win = SDL_CreateWindow("Example: GraphicsData (sdl)", 600, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

