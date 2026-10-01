/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */
/*
Example: 
*/

#include "glv_sfml.h"
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

	sf::Window win(sf::VideoMode({600u, 600u}), "Diffusion Paint (sfml)",
		sf::Style::Default, sf::State::Windowed, glv_sfml::contextSettings());
	win.setVerticalSyncEnabled(true);
	glv_sfml::attach(win, top, "Diffusion Paint (sfml)");
	sf::Clock clock;
	while(win.isOpen()){
		while(const std::optional<sf::Event> ev = win.pollEvent()){
			if(!glv_sfml::processEvent(win, top, *ev)){
				glv_sfml::detach(win, top);	// while the GL context still exists
				win.close();
				break;
			}
		}
		if(!win.isOpen()) break;
		glv_sfml::draw(win, top, clock.restart().asSeconds());
		win.display();
	}
	return 0;
}

