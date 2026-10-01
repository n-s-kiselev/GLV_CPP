/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */
/*
Example: Chladni Pattern
*/

#include "glv_sfml.h"
using namespace glv;

struct Canvas : public Plot{
	Canvas(const Rect& r): Plot(r), plotDensity(HSV(0.3,0.5,1), 1./16){
		int N = 256;
		data().resize(Data::FLOAT, 1, N,N);
		//plotDensity.interpolate(true).drawUnderGrid(true);
		add(plotDensity);
		equalizeAxes(true);
		showAxis(false);
		showGrid(false);
		disable(DrawBorder);
	}

	void onAnimate(double dt){
		
		plotDensity.plotRegion(interval(0), interval(1));
		
		Indexer i(data().size(1), data().size(2));

		while(i()){
			int ix = i[0];
			int iy = i[1];
			
			double px = interval(0).fromUnit(i.fracClosed(0)) * M_PI;
			double py = interval(1).fromUnit(i.fracClosed(1)) * M_PI;
			
			// plate vibration frequencies
			double f1 = 7;
			double f2 = 17;
			
			double val = (cos(f1*px) * cos(f2*py) - cos(f2*px) * cos(f1*py))/2;
			
			data().elem<float>(0, ix, iy) = val;
		}

	}

	PlotDensity plotDensity;
};


int main(){
	GLV top;
	Canvas v1(Rect(0));
	sf::Window win(sf::VideoMode({600u, 600u}), "Chladni Pattern (sfml)",
		sf::Style::Default, sf::State::Windowed, glv_sfml::contextSettings());
	win.setVerticalSyncEnabled(true);
	glv_sfml::attach(win, top, "Chladni Pattern (sfml)");

	top.colors().set(StyleColor::WhiteOnBlack);
	v1.stretch(1,1);
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
	return 0;
}

