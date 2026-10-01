/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Table layout

This demonstrates how to construct a layout using a Table.

*/

#include "glv_sfml.h"

int main(){

	GLV glv;
	View v1(Rect(80,40));
	View v2(Rect(40,40));
	View v3(Rect(40,80));
	View v4(Rect(40,40));
	View v5(Rect(80,80));
	View v6(Rect(40,40));

	const char * layout = 
		". x - x,"
		"x x x -,"
		"| x | . ";

	Table table(layout);

	glv << (table << v1 << v2 << v3 << v4 << v5 << v6);

	table.arrange();
			
	sf::Window win(sf::VideoMode({300u, 200u}), "Example: Table layout (sfml)",
		sf::Style::Default, sf::State::Windowed, glv_sfml::contextSettings());
	win.setVerticalSyncEnabled(true);
	glv_sfml::attach(win, glv, "Example: Table layout (sfml)");

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

