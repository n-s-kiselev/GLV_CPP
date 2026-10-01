/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_sdl.h"

const int N = 3600;

struct Scene : View3D{

	Scene()
	:	spin(0),
		gui("<<<, >--, >--"),
		freqX(4,0, N/2,-N/2),
		freqY(4,0, N/2,-N/2),
		freqZ(4,0, N/2,-N/2)
	{
		freqX.setValue(1); freqY.setValue(2); freqZ.setValue(3);
		stretch(1,1);
		disable(DrawBorder);
		freqX.fontSize(20);
		freqY.fontSize(20);
		freqZ.fontSize(20);
		gui << freqX << freqY << freqZ;
		gui.arrange();
		
		tz.extent(gui.width() - 2*gui.paddingX(), 20);

		prim.addItem("Points").addItem("LineLoop").addItem("Triangles").addItem("TriangleStrip");

		gui << tz << prim;
		gui.arrange();
		
		(*this) << gui;
	}

	virtual void onDraw3D(GLV& g){
		using namespace glv::draw;
		
		float fx = freqX.getValue();
		float fy = freqY.getValue();
		float fz = freqZ.getValue();
		
		for(int i=0; i<N; ++i){
			float f = float(i)/N;
			float p = f * 2*M_PI;
			
			float x = cos(fx*p);
			float y = sin(fy*p);
			float z = sin(fz*p);
			
			vertices[i](x, y, z);
			colors[i] = HSV(0.6, f, z*0.45+0.55);
		}

		if((spin+=0.5) > 360) spin -= 360;

		translateZ(tz.getValue()*4-4);
		rotateY(spin);
		stroke(2);
		
		int prims[] = {Points, LineLoop, Triangles, TriangleStrip};
		paint(prims[prim.selectedItem()], vertices, colors, N);
		//paint(draw::Triangles, vertices, colors, N);
	}
	
	float spin;
	Point3 vertices[N];
	Color colors[N];
		
	Table gui;
	NumberDialer freqX, freqY, freqZ;
	Slider tz;
	DropDown prim;
};

Scene scene;

int main (int argc, char ** argv){
	GLV top;
	top.colors().set(Color(HSV(0.6,0.5,0.5), 0.9), 0.4);
	top << scene;

	if(!SDL_Init(SDL_INIT_VIDEO)) return 1;
	glv_sdl::windowHints();
	SDL_Window * win = SDL_CreateWindow("Space Curve (sdl)", 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

