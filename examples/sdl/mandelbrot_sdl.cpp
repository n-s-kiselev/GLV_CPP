/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */
/*
Example: Mandelbrot set
*/

#include <complex>
#include "glv_sdl.h"
using namespace glv;

struct Canvas : public Plot{
	Canvas(const Rect& r): Plot(r), plotDensity(HSV(0.3,0.5,1), 1./16){
		int N = 384;
		data().resize(Data::FLOAT, 1, N,N);
		plotDensity.interpolate(true).drawUnderGrid(true);
		add(plotDensity);
		equalizeAxes(true);
		showGrid(false);
		showAxis(false);
		disable(DrawBorder);
	}

	void onAnimate(double dt){
		
//		major(interval(1).diameter()/4);
//		minor(1);

		//printf("%g\n", interval(1).diameter());
		
		// set plot region to current grid region
		plotDensity.plotRegion(interval(0), interval(1));
		
		Indexer i(data().size(1), data().size(2));

		while(i()){
			int ix = i[0];
			int iy = i[1];
			
			double posx = interval(0).fromUnit(i.fracClosed(0));
			double posy = interval(1).fromUnit(i.fracClosed(1));
			
			std::complex<double> c(posx, posy);
			std::complex<double> z(c);
			
			for(int i=0; i<40; ++i){
				z = z*z + c;
			}

			double val = z.real() * z.real() + z.imag() * z.imag();
			
			//val = (val > 0) ? 1./val : 0;
			val = (val > 0) ? log(val)/4. : 1;
			//val = (val > 0) ? val/4. : 1;

			val = glv::clip(val, 1., -1.);			
			data().elem<float>(0, ix, iy) = val;
		}

	}

	PlotDensity plotDensity;
};


int main(){
	GLV top;
	Canvas v1(Rect(0));
	if(!SDL_Init(SDL_INIT_VIDEO)) return 1;
	glv_sdl::windowHints();
	SDL_Window * win = SDL_CreateWindow("Mandelbrot Set (sdl)", 800, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if(!win){ SDL_Quit(); return 1; }
	SDL_GLContext ctx = SDL_GL_CreateContext(win);
	SDL_GL_MakeCurrent(win, ctx);
	SDL_GL_SetSwapInterval(1);
	glv_sdl::attach(win, top);
	
	top.colors().set(StyleColor::WhiteOnBlack);
	v1.stretch(1,1);
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
	return 0;
}

