/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Using stretch and anchor

This demonstrates how the stretch and acnhor factors can be used to control
the resizing behavior of child Views.

*/

#include "glv_sfml.h"

int main(int argc, char ** argv){

	GLV glv;
	View v1(Rect(100));
	View v2(Rect( 40));
	
	glv.colors().set(StyleColor::WhiteOnBlack);
	glv.cloneStyle();
	v1.colors().set(StyleColor::SmokyGray);
	
	glv << v1;
	v1 << v2;
		
	sf::Window win(sf::VideoMode({200u, 200u}), "stretchAnchor (sfml)",
		sf::Style::Default, sf::State::Windowed, glv_sfml::contextSettings());
	win.setVerticalSyncEnabled(true);
	glv_sfml::attach(win, glv, "stretchAnchor (sfml)");

	v1.stretch(1,1);
	v1.anchor(0,0);
	
	v2.anchor(1,1);

	sf::Clock clock;
	while(win.isOpen()){
		while(const std::optional<sf::Event> ev = win.pollEvent()){
			if(!glv_sfml::processEvent(win, glv, *ev)){
				glv_sfml::detach(win, glv);	// while the GL context still exists
				win.close();
				break;
			}
		}
		if(!win.isOpen()) break;
		glv_sfml::draw(win, glv, clock.restart().asSeconds());
		win.display();
	}
}

