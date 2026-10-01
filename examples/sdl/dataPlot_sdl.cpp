/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */
/*
Example: 
*/

#include "glv_sdl.h"

namespace glv{

struct PlotVector : public Plottable{

	PlotVector(): Plottable(draw::Triangles){}

	void onMap(GraphicsData& b, const Data& d, const Indexer& i){
		double dx = 2./d.size(1);
		double dy = 2./d.size(2);
		while(i()){
			double x = i.frac(0)*2 - 1 + dx/2;
			double y = i.frac(1)*2 - 1 + dy/2;
			double vx=d.at<double>(0,i[0],i[1],i[2])*dx*2;
			double vy=d.at<double>(1,i[0],i[1],i[2])*dy*2;

			double r = 0.2/hypot(vx, vy);
			double Nx = vy*r*dx;
			double Ny =-vx*r*dy;

//			b.addVertex2(
//				x, y,
//				x + d.at<double>(0,i[0],i[1],i[2])*dx,
//				y + d.at<double>(1,i[0],i[1],i[2])*dy
//			);
			b.addVertex2(
				x+Nx, y+Ny,
				x-Nx, y-Ny,
				x + vx,
				y + vy
			);
			b.addColor(
				Color(0,0),
				Color(0,0),
				Color(0)
			);
		}
	}
};


struct MyGLV : GLV {

	MyGLV(): phase(0){
		data.resize(Data::DOUBLE, 2,16,16);
	}

	void onAnimate(double dt){
		phase += 0.013;
		Data& d = data;
		
		Indexer i(d.size(1), d.size(2));

		while(i()){
			float x = i.frac(0);
			float y = i.frac(1);
			x = (cos(x*7 + phase) + cos(x*-2 + phase*1.01))/2;
			y = (sin(y*5 + phase) + sin(y*-3 + phase*1.01))/2;
			d.assign(x, 0,i[0],i[1]);
			d.assign(y, 1,i[0],i[1]);
		}
	}

	double phase;
	Data data;
};
}

int main(){

	// Create the Views
	MyGLV top;
	Plot v1(Rect(000,0, 400,400), *new PlotDensity);
	Plot v2(Rect(400,0, 400,400), *new PlotVector);
	Plot v3(Rect(800,0, 400,400), (new PlotFunction2D)->prim(draw::Points).stroke(2));

	v1.data() = top.data;
	v2.data() = top.data;
	v3.data() = top.data;
	
	top << v1 << v2 << v3;

	top.fit();
	if(!SDL_Init(SDL_INIT_VIDEO)) return 1;
	glv_sdl::windowHints();
	SDL_Window * win = SDL_CreateWindow("Data Plots (sdl)", (int)top.w, (int)top.h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

