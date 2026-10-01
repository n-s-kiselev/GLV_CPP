/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Using a Notification to set a Label

A Notification is used to set a label to a slider's value.

*/

#include "glv_sfml.h"

void ntSetLabel(const Notification& n){
	Label& l = *n.receiver<Label>();
	Slider& s = *n.sender<Slider>();
	l.setValue(s.data().toString());
}

int main(int argc, char ** argv){

	GLV glv;
	Slider slider(Rect(100,20));
	Label label;
	
	slider.attach(ntSetLabel, Update::Value, &label);

	Placer p(glv, Direction::E, Place::TL, 10, 10);
	
	p << slider << label;

	sf::Window win(sf::VideoMode({200u, 40u}), "notification (sfml)",
		sf::Style::Default, sf::State::Windowed, glv_sfml::contextSettings());
	win.setVerticalSyncEnabled(true);
	glv_sfml::attach(win, glv, "notification (sfml)");
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

