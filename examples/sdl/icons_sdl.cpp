/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Icons

This shows some of the icons provided by GLV that can be used to decorate 
widgets.
*/

#include "glv_sdl.h"

struct MyGLV : public GLV{

	float pad, dim;

	MyGLV(): pad(8), dim(24)
	{}

	void onDraw(GLV& g){
		using namespace glv;

		float x = pad;
		float y = pad;

		draw::color(0);
		draw::lineWidth(2);

		// The function signatures for the icons take the left, top, right, and
		// bottom positions of the rectangular containing region of the icon.

		draw::triangleL(x,y, x+dim, y+dim);

		y += dim+pad;
		draw::triangleR(x,y, x+dim, y+dim);

		y += dim+pad;
		draw::triangleU(x,y, x+dim, y+dim);

		y += dim+pad;
		draw::triangleD(x,y, x+dim, y+dim);


		y = pad;
		x += dim+pad;
			// Rectangular outline
		draw::frame(x,y, x+dim, y+dim);

		y += dim+pad;
		draw::rectangle(x,y, x+dim, y+dim);

		y += dim+pad;
			// Frame with truncated corners
		draw::frameTrunc<4,4,4,4>(x,y, x+dim, y+dim);

		y += dim+pad;
			// Rectangle with truncated corners
		draw::rectTrunc<4,4,4,4>(x,y, x+dim, y+dim);


		y = pad;
		x += dim+pad;
			// Circle with a specified number of edges
		draw::circle<16>(x,y, x+dim, y+dim);

		y += dim+pad;
			// Disc with a specified number of edges
		draw::disc<16>(x,y, x+dim, y+dim);

		y += dim+pad;
			// Polygon with a specified number of vertices, winding, and angle
		draw::polygon<5,2,18>(x,y, x+dim, y+dim);

		y += dim+pad;
			// Polygon with a specified number of vertices, winding, and angle
		draw::polygon<4,1,0>(x,y, x+dim, y+dim);


		y = pad;
		x += dim+pad;
		draw::check(x,y, x+dim, y+dim);

		y += dim+pad;
		draw::x(x,y, x+dim, y+dim);

		y += dim+pad;
		draw::minus(x,y, x+dim, y+dim);

		y += dim+pad;
		draw::plus(x,y, x+dim, y+dim);


		// Crosshatches: horizontal and vertical lines
		y = pad;
		x += dim+pad;
		draw::crosshatch<2,2>(x,y, x+dim, y+dim);

		y += dim+pad;
		draw::crosshatch<3,3>(x,y, x+dim, y+dim);

		y += dim+pad;
		draw::crosshatch<1,4>(x,y, x+dim, y+dim);

		y += dim+pad;
		draw::crosshatch<4,0>(x,y, x+dim, y+dim);


		// Rose curves with specified number of edges, harmonic numbers, and
		// start angles.
		y = pad;
		x += dim+pad;
		draw::rose<36, -1, 2, 0>(x,y, x+dim, y+dim);

		y += dim+pad;
		draw::rose<36,  1, 2, 0>(x,y, x+dim, y+dim);

		// Wheel spokes
		y += dim+pad;
		draw::spokes<3, 0>(x,y, x+dim, y+dim);

		y += dim+pad;
		draw::spokes<6, 0>(x,y, x+dim, y+dim);


		y = pad;
		x += dim+pad;
		draw::fileLoad(x,y, x+dim, y+dim);

		y += dim+pad;
		draw::fileSave(x,y, x+dim, y+dim);

		y += dim+pad;
		draw::magnifier(x,y, x+dim, y+dim);

		y += dim+pad;
		draw::question(x,y, x+dim, y+dim);


		y = pad;
		x += dim+pad;
			// Symbol of parent-child relationship
		draw::viewChild(x,y, x+dim, y+dim);

		y += dim+pad;
			// Symbol of sibling relationship
		draw::viewSibling(x,y, x+dim, y+dim);

		y += dim+pad;
		

		y += dim+pad;
		
	}
};

int main(){
	MyGLV myGLV;			
	if(!SDL_Init(SDL_INIT_VIDEO)) return 1;
	glv_sdl::windowHints();
	SDL_Window * win = SDL_CreateWindow("GLV Example: Icons (sdl)", myGLV.pad*9 + myGLV.dim*8, myGLV.pad*5 + myGLV.dim*4, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if(!win){ SDL_Quit(); return 1; }
	SDL_GLContext ctx = SDL_GL_CreateContext(win);
	SDL_GL_MakeCurrent(win, ctx);
	SDL_GL_SetSwapInterval(1);
	glv_sdl::attach(win, myGLV);
	Uint64 prev = SDL_GetTicksNS();
	bool running = true;
	while(running){
		SDL_Event ev;
		while(SDL_PollEvent(&ev)){
			if(!glv_sdl::processEvent(win, myGLV, ev)) running = false;
		}
		Uint64 now = SDL_GetTicksNS();
		glv_sdl::draw(win, myGLV, (now - prev) * 1e-9);
		prev = now;
		SDL_GL_SwapWindow(win);
	}
	glv_sdl::detach(win, myGLV);
	SDL_GL_DestroyContext(ctx);
	SDL_DestroyWindow(win);
	SDL_Quit();
}

