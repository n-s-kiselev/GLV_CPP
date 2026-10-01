/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */
/*
Example:
Demonstration of a simple painting application using the mouse as a brush.
The left mouse button controls the brush up and down and the right button
controls the eraser up and down.
*/

#include "glv_sfml.h"
using namespace glv;

struct Canvas : public Plot{
	Canvas(const Rect& r): Plot(r){
		data().resize(Data::FLOAT, 1,64,64);
	}

	bool onEvent(Event::t e, GLV& g){

		float gx = pixToGrid(0, g.mouse().xRel());	// convert mouse to grid position
		float gy = pixToGrid(1, g.mouse().yRel());	// convert mouse to grid position
		int cx = (gx*0.5+0.5) * data().size(1);		// convert grid to cell position
		int cy = (gy*0.5+0.5) * data().size(2);		// convert grid to cell position

		switch(e){
		case Event::MouseDown:
		case Event::MouseDrag:
			if(data().inBounds(0, cx, cy)){
				data().assign(g.mouse().button() ? 0:1, 0,cx,cy);
			}
			break;
		case Event::KeyDown:
			switch(g.keyboard().key()){
			case 'c': data().assignAll(0); break;
			}
			break;
		default:;
		}
		
		return true;
	}
};


int main(){
	GLV top;
	Canvas v1(Rect(000,0, 00,00));
	v1.add(*new PlotDensity(Color(1)));
	
	v1.stretch(1,1);
	top << v1;

	sf::Window win(sf::VideoMode({600u, 600u}), "Simple Paint (sfml)",
		sf::Style::Default, sf::State::Windowed, glv_sfml::contextSettings());
	win.setVerticalSyncEnabled(true);
	glv_sfml::attach(win, top, "Simple Paint (sfml)");
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

