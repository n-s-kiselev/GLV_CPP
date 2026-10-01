/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Sliders With Values

This demonstrates how to make individual value labels for each slider of a
Sliders object.
*/

#include "glv_sfml.h"

// Notification callback to set label string to slider value
void ntSetLabel(const Notification& n){
	Label * l = n.receiver<Label>();
	Sliders& s = *n.sender<Sliders>();
	
	// Get currently selected slider
	int idx = s.selected();
	
	// Set string of corresponding label to slider value
	l[idx].setValue(glv::toString(s.getValue(idx)));
}


int main(){

	const int numSliders = 4;

	GLV top;
	Sliders sliders(Rect(200,80), 1, numSliders);
	Label labels[numSliders];
	
	for(int i=0; i<numSliders; ++i){
		labels[i].pos(2,4);
		
		// Set anchor factor so labels lie on top of each slider
		labels[i].anchor(0, float(i)/numSliders);

		sliders << labels[i];
	}
	
	// Set notification callback for whenever a slider changes value
	sliders.attach(ntSetLabel, Update::Value, labels);
	sliders.colors().set(StyleColor::SmokyGray);
	
	top << sliders;

	top.fit();
	sf::Window win(sf::VideoMode({(unsigned)top.w, (unsigned)top.h}), "slidersWithValues (sfml)",
		sf::Style::Default, sf::State::Windowed, glv_sfml::contextSettings());
	win.setVerticalSyncEnabled(true);
	glv_sfml::attach(win, top, "slidersWithValues (sfml)");
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

