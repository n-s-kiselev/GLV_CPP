/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */
/*
Example: 
*/

#include "glv_sfml.h"

int main(){

	GLV top;
	sf::Window win(sf::VideoMode({800u, 800u}), "Grid (sfml)",
		sf::Style::Default, sf::State::Windowed, glv_sfml::contextSettings());
	win.setVerticalSyncEnabled(true);
	glv_sfml::attach(win, top, "Grid (sfml)");
	
	top.colors().set(StyleColor::WhiteOnBlack);
	//top.colors().set(StyleColor::SmokeyGray);
	
	Grid v1(Rect(0,0));
	
	v1.range(1);			// set plot region
	v1.major(1);			// set major tick mark placement
	v1.minor(2);			// number of divisions per major ticks
	v1.equalizeAxes(true);
	
	v1.stretch(1,1);
	//v1.disable(CropSelf);
	top << v1;

	top.addHandler(Event::KeyDown, *new glv_sfml::FullScreenToggler(win));

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
}

