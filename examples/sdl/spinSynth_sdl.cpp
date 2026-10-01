/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_sdl.h"

struct Scene : View3D{

	Scene(const Rect& r): View3D(r){}

	virtual void onDraw3D(GLV& g){
		using namespace glv::draw;

		if((ax+=vx) >= 360) ax-=360;
		if((ay+=vy) >= 360) ay-=360;
		if((az+=vz) >= 360) az-=360;

		Point3 p[] = {
			Point3(-0.5,-0.5,0), Point3(+0.5,-0.5,0), Point3(0,+1,0)
		};
		
		Color c[] = { HSV(0.1), HSV(0.2), Color(0) };

		translateZ(-3);
		rotate(ax, ay, az);
		paint(Triangles, p,c,3);
	}
	
	float vx, vy, vz;
	float ax, ay, az;
};

Slider slider1(Rect(200, 40)), slider2(slider1.rect()), slider3(slider1.rect());
View toolBar(Rect(0,60));
View statusBar(Rect(0,-20,0,20));
Label statusString("", 10, 4);
Scene scene(Rect(0,toolBar.h, 0,-(toolBar.h+statusBar.h)));

void ntStatusString(const Notification& n){
	View& v = *n.sender<View>();
	statusString.setValue(v.className() + std::string(": ") + v.descriptor() + " = " + v.getDataAsString());
}

int main (int argc, char ** argv){

	slider1.descriptor("Angular Velocity x");
	slider2.descriptor("Angular Velocity y");
	slider3.descriptor("Angular Velocity z");

	slider1.interval(30,-30);
	slider2.interval(30,-30);
	slider3.interval(30,-30);

	slider1.attachVariable(scene.vx);
	slider2.attachVariable(scene.vy);
	slider3.attachVariable(scene.vz);
	
	slider1.attach(ntStatusString, Update::Value);
	slider2.attach(ntStatusString, Update::Value);
	slider3.attach(ntStatusString, Update::Value);

	GLV top(0,0);
	top.colors().set(Color(HSV(0.1,0.5,0.8), 1), 0.7);
	
	scene.stretch(1,1).anchor(0,0);
	toolBar.stretch(1,0).anchor(0,0);
	statusBar.stretch(1,0).anchor(0,1);
	statusString.align(0,1).anchor(Place::CL);
	
	top << scene << toolBar << (statusBar << statusString);

	Placer p(toolBar, Direction::E, Place::TL, 10, 10, 10);

	p << slider1 << slider2 << slider3;

	if(!SDL_Init(SDL_INIT_VIDEO)) return 1;
	glv_sdl::windowHints();
	SDL_Window * win = SDL_CreateWindow("spinSynth (sdl)", 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

