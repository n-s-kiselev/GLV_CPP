/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */
/*
Example: 
*/

#include "glv_sdl.h"
using namespace glv;

struct Canvas : public Plot{
	Canvas(const Rect& r): Plot(r){
		disable(DrawBorder);
		P.resize(Data::FLOAT, 64,64,2);
		iz=0;
	}

	void onAnimate(double dt){

		// set plot data to current z plane
		data() = P.slice(iz*P.size(0,1)).shape(1, P.size(0), P.size(1));
		
		Indexer i(P.size(0), P.size(1));

		while(i()){

			int x1 = i[0];			
			int x0 = x1-1; if(x0<0) x0 = i.size(0)-1;
			int x2 = x1+1; if(x2==i.size(0)) x2 = 0;

			int y1 = i[1];
			int y0 = y1-1; if(y0<0) y0 = i.size(1)-1;
			int y2 = y1+1; if(y2==i.size(1)) y2 = 0;			

			P.elem<float>(x1,y1,1-iz) =
				P.elem<float>(x1,y1,iz)*0.5 +
				( P.elem<float>(x0,y1,iz)
				+ P.elem<float>(x2,y1,iz)
				+ P.elem<float>(x1,y0,iz)
				+ P.elem<float>(x1,y2,iz) )*0.25*0.5;
		}

		// "swap" buffers
		iz = 1-iz;
	}

	bool onEvent(Event::t e, GLV& g){

		//Plot::onEvent(e,g);	// "inherit" mouse/keyboard controls

		float gx = pixToGrid(0, g.mouse().xRel());	// convert mouse to grid position
		float gy = pixToGrid(1, g.mouse().yRel());	// convert mouse to grid position
		int cx = (gx*0.5+0.5) * P.size(0);			// convert grid to cell position
		int cy = (gy*0.5+0.5) * P.size(1);			// convert grid to cell position

		switch(e){
		case Event::MouseDown:
		case Event::MouseDrag:
			if(P.inBounds(cx, cy)){
				P.elem<float>(cx,cy,iz) = g.mouse().button() ? -1:1;
			}
			break;
		case Event::KeyDown:
			switch(g.keyboard().key()){
			case 'c': P.assignAll(0); break;
			}
			break;
		default:;
		}
		
		return true;
	}

	Data P;
	int iz;
};


int main(){
	GLV top;
	Canvas v1(Rect(0));
	v1.add(*new PlotDensity(HSV(0.5, 1, 1), 1./16));
	v1.stretch(1,1);
	top << v1;

	if(!SDL_Init(SDL_INIT_VIDEO)) return 1;
	glv_sdl::windowHints();
	SDL_Window * win = SDL_CreateWindow("Diffusion Paint (sdl)", 600, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

