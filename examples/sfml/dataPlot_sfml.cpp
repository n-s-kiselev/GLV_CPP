/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */
/*
Example: 
*/

#include "glv_sfml.h"

namespace glv{

struct PlotVector : public Plottable{

	PlotVector(): Plottable(draw::Triangles){}

	void onMap(GraphicsData& b, const Data& d, const Indexer& i){
		double dx = 2./d.size(1);
		double dy = 2./d.size(2);
		while(i()){
			double x = i.frac(0)*2 - 1 + dx/2;
			double y = i.frac(1)*2 - 1 + dy/2;
			double vx=d.at<double>(0,i[0],i[1],i[2])*dx*2;
			double vy=d.at<double>(1,i[0],i[1],i[2])*dy*2;

			double r = 0.2/hypot(vx, vy);
			double Nx = vy*r*dx;
			double Ny =-vx*r*dy;

//			b.addVertex2(
//				x, y,
//				x + d.at<double>(0,i[0],i[1],i[2])*dx,
//				y + d.at<double>(1,i[0],i[1],i[2])*dy
//			);
			b.addVertex2(
				x+Nx, y+Ny,
				x-Nx, y-Ny,
				x + vx,
				y + vy
			);
			b.addColor(
				Color(0,0),
				Color(0,0),
				Color(0)
			);
		}
	}
};


struct MyGLV : GLV {

	MyGLV(): phase(0){
		data.resize(Data::DOUBLE, 2,16,16);
	}

	void onAnimate(double dt){
		phase += 0.013;
		Data& d = data;
		
		Indexer i(d.size(1), d.size(2));

		while(i()){
			float x = i.frac(0);
			float y = i.frac(1);
			x = (cos(x*7 + phase) + cos(x*-2 + phase*1.01))/2;
			y = (sin(y*5 + phase) + sin(y*-3 + phase*1.01))/2;
			d.assign(x, 0,i[0],i[1]);
			d.assign(y, 1,i[0],i[1]);
		}
	}

	double phase;
	Data data;
};
}

int main(){

	// Create the Views
	MyGLV top;
	Plot v1(Rect(000,0, 400,400), *new PlotDensity);
	Plot v2(Rect(400,0, 400,400), *new PlotVector);
	Plot v3(Rect(800,0, 400,400), (new PlotFunction2D)->prim(draw::Points).stroke(2));

	v1.data() = top.data;
	v2.data() = top.data;
	v3.data() = top.data;
	
	top << v1 << v2 << v3;

	top.fit();
	sf::Window win(sf::VideoMode({(unsigned)top.w, (unsigned)top.h}), "Data Plots (sfml)",
		sf::Style::Default, sf::State::Windowed, glv_sfml::contextSettings());
	win.setVerticalSyncEnabled(true);
	glv_sfml::attach(win, top, "Data Plots (sfml)");
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

