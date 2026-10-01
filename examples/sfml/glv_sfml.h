#ifndef INC_GLV_EXAMPLES_SFML_H
#define INC_GLV_EXAMPLES_SFML_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*	Header-only glue between SFML3 and a GLV top view, shared by the examples
	in this folder. It replaces examples/glut/example.h: each example creates
	its own sf::Window and runs its own event loop, and uses these helpers only
	to hand the GL context and the input events to GLV. The translation follows
	src/glv_binding_glut.cpp so that the examples behave as their GLUT
	counterparts do. Not part of GLV's public API. SFML requires C++17. */

#include <math.h>
#include <optional>
#include <string>
#include "glv.h"		// first: provides the OpenGL declarations (GLAD or system)
#include "glv_util.h"

#include <SFML/Window.hpp>

using namespace glv;

namespace glv_sfml{

typedef sf::Keyboard::Key K;

// Character last sent as a key-down for each scancode, so that the matching
// key-up reports the same character (SFML reports releases by key only).
inline int& lastChar(sf::Keyboard::Scancode sc){
	static int chars[sf::Keyboard::ScancodeCount] = {0};
	static int invalid = 0;
	int i = static_cast<int>(sc);
	return (i >= 0 && i < (int)sf::Keyboard::ScancodeCount) ? chars[i] : invalid;
}

// Printable key waiting for its TextEntered event
inline sf::Keyboard::Scancode& pendingKey(){
	static sf::Keyboard::Scancode sc = sf::Keyboard::Scancode::Unknown;
	return sc;
}

// Modifier state as of the last key event. Tracked from events rather than
// sf::Keyboard::isKeyPressed(), which on macOS needs input-monitoring rights.
struct Mods{ bool shift, alt, ctrl, system; };
inline Mods& mods(){ static Mods m = {false, false, false, false}; return m; }

template <class KeyEvent>
inline void storeMods(const KeyEvent& e){
	Mods& m = mods();
	m.shift = e.shift; m.alt = e.alt; m.ctrl = e.control; m.system = e.system;
}

inline void modsToGLV(GLV& g){
	const Mods& m = mods();
	g.setKeyModifiers(m.shift, m.alt, m.ctrl, false /* no caps state */, m.system);
}

inline void keyToGLV(GLV& g, int key, bool down){
	down ? g.setKeyDown(key) : g.setKeyUp(key);
	modsToGLV(g);
	g.propagateEvent();
}

// Maps non-printable SFML keys to GLV key codes; returns 0 for other keys.
inline int specialKey(K key){
	switch(key){
		case K::Enter:		return Key::Return;
		case K::Backspace:	return Key::Backspace;
		case K::Tab:		return Key::Tab;
		case K::Escape:		return Key::Escape;
		case K::Delete:		return Key::Delete;
		case K::Left:		return Key::Left;
		case K::Up:			return Key::Up;
		case K::Right:		return Key::Right;
		case K::Down:		return Key::Down;
		case K::PageUp:		return Key::PageUp;
		case K::PageDown:	return Key::PageDown;
		case K::Home:		return Key::Home;
		case K::End:		return Key::End;
		case K::Insert:		return Key::Insert;
		default:;
	}
	if(key >= K::F1 && key <= K::F12) return Key::F1 + (static_cast<int>(key) - static_cast<int>(K::F1));
	return 0;
}

// Lowercase ASCII of a printable key (US layout), or 0; used with ctrl/system
// held, when SFML sends no TextEntered event.
inline int asciiKey(K key){
	if(key >= K::A && key <= K::Z) return 'a' + (static_cast<int>(key) - static_cast<int>(K::A));
	if(key >= K::Num0 && key <= K::Num9) return '0' + (static_cast<int>(key) - static_cast<int>(K::Num0));
	switch(key){
		case K::Space:		return ' ';
		case K::LBracket:	return '[';
		case K::RBracket:	return ']';
		case K::Semicolon:	return ';';
		case K::Comma:		return ',';
		case K::Period:		return '.';
		case K::Apostrophe:	return '\'';
		case K::Slash:		return '/';
		case K::Backslash:	return '\\';
		case K::Grave:		return '`';
		case K::Equal:		return '=';
		case K::Hyphen:		return '-';
		default:			return 0;
	}
}

template <class KeyEvent>
inline void keyEvent(GLV& g, const KeyEvent& e, bool down){
	storeMods(e);
	int k = specialKey(e.code);
	if(k){ keyToGLV(g, k, down); return; }

	int c = asciiKey(e.code);
	if(down){
		if(c && (e.control || e.system)){
			// Send the lowercase key as the GLUT binding does for ctrl keys.
			lastChar(e.scancode) = c;
			keyToGLV(g, c, true);
		}
		else{
			pendingKey() = e.scancode;	// character arrives as TextEntered
		}
	}
	else if(lastChar(e.scancode)){
		keyToGLV(g, lastChar(e.scancode), false);
		lastChar(e.scancode) = 0;
	}
}

inline void textEvent(GLV& g, char32_t unicode){
	// GLV key codes above 255 are special keys; below 32 are control
	// characters already sent as key events (Backspace, Tab, Return, ...).
	if(unicode < 32 || unicode >= 256 || unicode == 127) return;
	sf::Keyboard::Scancode sc = pendingKey();
	pendingKey() = sf::Keyboard::Scancode::Unknown;
	if(sf::Keyboard::Scancode::Unknown != sc) lastChar(sc) = (int)unicode;
	keyToGLV(g, (int)unicode, true);
}

inline void mouseButtonEvent(GLV& g, sf::Mouse::Button button, sf::Vector2i pos, bool down){
	space_t x = (space_t)pos.x, y = (space_t)pos.y;
	space_t relx = x, rely = y;

	int btn;
	switch(button){
		case sf::Mouse::Button::Left:	btn = Mouse::Left; break;
		case sf::Mouse::Button::Middle:	btn = Mouse::Middle; break;
		case sf::Mouse::Button::Right:	btn = Mouse::Right; break;
		default:						btn = Mouse::Extra;
	}

	if(down)	g.setMouseDown(relx, rely, btn, 0);
	else		g.setMouseUp  (relx, rely, btn, 0);
	g.setMousePos((int)x, (int)y, relx, rely);
	modsToGLV(g);
	g.propagateEvent();
}

inline void mouseMotionEvent(GLV& g, sf::Vector2i pos){
	space_t x = (space_t)pos.x, y = (space_t)pos.y;
	space_t relx = x, rely = y;
	g.setMouseMotion(relx, rely, g.mouse().isDownAny() ? Event::MouseDrag : Event::MouseMove);
	g.setMousePos((int)x, (int)y, relx, rely);
	g.propagateEvent();
}

inline void sizeToGLV(GLV& g, sf::Vector2u size){
	if(size.x > 0 && size.y > 0 && (g.w != size.x || g.h != size.y)){
		g.extent(size.x, size.y);
		g.broadcastEvent(Event::WindowResize);
	}
}

/// Context settings matching GLV's default display mode (GLUT_DOUBLE|ALPHA|DEPTH).
inline sf::ContextSettings contextSettings(){
	sf::ContextSettings s;
	s.depthBits = 24;
	return s;
}

// Title of the window, needed to re-create it for fullscreen (sf::Window has
// no title getter).
inline std::string& windowTitle(){ static std::string t; return t; }

inline void initWindow(sf::Window& w, GLV& g){
	w.setKeyRepeatEnabled(false);	// GLUT binding ignores key repeat too
	GLV_PLATFORM_INIT_CONTEXT
	g.broadcastEvent(Event::WindowCreate);
	sizeToGLV(g, w.getSize());
}

/// Hands the window's (active) GL context and input to GLV: loads the GL
/// functions, sends Event::WindowCreate and sizes the GLV to the window, as
/// Window::setGLV() does in the GLUT binding.
inline void attach(sf::Window& w, GLV& g, const std::string& title = ""){
	windowTitle() = title;
	initWindow(w, g);
}

/// Translates one SFML event for the GLV shown in window w. Returns false when
/// the window should close.
inline bool processEvent(sf::Window& w, GLV& g, const sf::Event& e){
	if(e.is<sf::Event::Closed>()) return false;
	if(const auto * r = e.getIf<sf::Event::Resized>())					sizeToGLV(g, r->size);
	else if(const auto * k = e.getIf<sf::Event::KeyPressed>())			keyEvent(g, *k, true);
	else if(const auto * k = e.getIf<sf::Event::KeyReleased>())			keyEvent(g, *k, false);
	else if(const auto * t = e.getIf<sf::Event::TextEntered>())			textEvent(g, t->unicode);
	else if(const auto * b = e.getIf<sf::Event::MouseButtonPressed>())	mouseButtonEvent(g, b->button, b->position, true);
	else if(const auto * b = e.getIf<sf::Event::MouseButtonReleased>())	mouseButtonEvent(g, b->button, b->position, false);
	else if(const auto * m = e.getIf<sf::Event::MouseMoved>())			mouseMotionEvent(g, m->position);
	else if(const auto * s = e.getIf<sf::Event::MouseWheelScrolled>()){
		int dy = (s->delta > 0) - (s->delta < 0);
		if(sf::Mouse::Wheel::Vertical == s->wheel && dy){
			g.setMouseWheel(dy);
			g.propagateEvent();
		}
	}
	return true;
}

/// Resizes the window to the GLV's own fitted size (Window::fit()).
inline void fit(sf::Window& w, GLV& g){
	g.fit();
	if(g.w > 0 && g.h > 0) w.setSize(sf::Vector2u((unsigned)g.w, (unsigned)g.h));
}

/// Draws one frame into the window's framebuffer (Window::Impl::draw()).
inline void draw(sf::Window& w, GLV& g, double dsec){
	sf::Vector2u size = w.getSize();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	g.drawGLV(size.x, size.y, dsec);
}

/// Toggles fullscreen. SFML has to re-create the window (and its GL context)
/// for that, so GLV gets WindowDestroy/WindowCreate around it, as
/// Window::gameMode() does in the GLUT binding.
inline void fullScreenToggle(sf::Window& w, GLV& g){
	static bool full = false;
	static sf::Vector2u windowedSize(800, 600);
	g.broadcastEvent(Event::WindowDestroy);
	if(!full){
		windowedSize = w.getSize();
		w.create(sf::VideoMode::getDesktopMode(), windowTitle(), sf::State::Fullscreen, contextSettings());
	}
	else{
		w.create(sf::VideoMode(windowedSize), windowTitle(), sf::Style::Default, sf::State::Windowed, contextSettings());
	}
	full = !full;
	w.setVerticalSyncEnabled(true);
	initWindow(w, g);
}

/// Key event handler to toggle fullscreen (glv::FullScreenToggler).
struct FullScreenToggler : EventHandler {

	FullScreenToggler(sf::Window& w, int key_ = Key::Escape): win(w), key(key_){}

	bool onEvent(View& v, GLV& g) override {
		if(g.keyboard().key() == key){
			fullScreenToggle(win, g);
			return false;
		}
		return true;
	}

	sf::Window& win;
	int key;
};

/// Sends Event::Quit and Event::WindowDestroy while the GL context still
/// exists (Application::quit() and ~Window() in the GLUT binding).
inline void detach(sf::Window& w, GLV& g){
	g.broadcastEvent(Event::Quit);
	g.broadcastEvent(Event::WindowDestroy);
}

} // glv_sfml::

#endif
