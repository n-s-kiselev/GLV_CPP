#ifndef INC_GLV_EXAMPLES_SDL_H
#define INC_GLV_EXAMPLES_SDL_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*	Header-only glue between SDL3 and a GLV top view, shared by the examples
	in this folder. It replaces examples/glut/example.h: each example creates
	its own SDL window and GL context and runs its own event loop, and uses
	these helpers only to hand the GL context and the input events to GLV. The
	translation follows src/glv_binding_glut.cpp so that the examples behave as
	their GLUT counterparts do. Not part of GLV's public API. */

#include <math.h>
#include "glv.h"		// first: provides the OpenGL declarations (GLAD or system)
#include "glv_util.h"

#include <SDL3/SDL.h>

using namespace glv;

namespace glv_sdl{

// Character last sent as a key-down for each scancode, so that the matching
// key-up reports the same character (SDL reports releases by key only).
inline int& lastChar(SDL_Scancode sc){
	static int chars[SDL_SCANCODE_COUNT] = {0};
	static int invalid = 0;
	return (sc >= 0 && sc < SDL_SCANCODE_COUNT) ? chars[sc] : invalid;
}

// Printable key waiting for its SDL_EVENT_TEXT_INPUT event
inline SDL_Scancode& pendingKey(){ static SDL_Scancode sc = SDL_SCANCODE_UNKNOWN; return sc; }

inline void modsToGLV(GLV& g, SDL_Keymod mods){
	g.setKeyModifiers(
		mods & SDL_KMOD_SHIFT,
		mods & SDL_KMOD_ALT,
		mods & SDL_KMOD_CTRL,
		mods & SDL_KMOD_CAPS,
		mods & SDL_KMOD_GUI
	);
}

inline void keyToGLV(GLV& g, int key, bool down){
	down ? g.setKeyDown(key) : g.setKeyUp(key);
	modsToGLV(g, SDL_GetModState());
	g.propagateEvent();
}

// Maps non-printable SDL keys to GLV key codes; returns 0 for other keys.
inline int specialKey(SDL_Keycode key){
	switch(key){
		case SDLK_RETURN:
		case SDLK_KP_ENTER:		return Key::Return;
		case SDLK_BACKSPACE:	return Key::Backspace;
		case SDLK_TAB:			return Key::Tab;
		case SDLK_ESCAPE:		return Key::Escape;
		case SDLK_DELETE:		return Key::Delete;
		case SDLK_LEFT:			return Key::Left;
		case SDLK_UP:			return Key::Up;
		case SDLK_RIGHT:		return Key::Right;
		case SDLK_DOWN:			return Key::Down;
		case SDLK_PAGEUP:		return Key::PageUp;
		case SDLK_PAGEDOWN:		return Key::PageDown;
		case SDLK_HOME:			return Key::Home;
		case SDLK_END:			return Key::End;
		case SDLK_INSERT:		return Key::Insert;
		case SDLK_F1:  return Key::F1;  case SDLK_F2:  return Key::F2;
		case SDLK_F3:  return Key::F3;  case SDLK_F4:  return Key::F4;
		case SDLK_F5:  return Key::F5;  case SDLK_F6:  return Key::F6;
		case SDLK_F7:  return Key::F7;  case SDLK_F8:  return Key::F8;
		case SDLK_F9:  return Key::F9;  case SDLK_F10: return Key::F10;
		case SDLK_F11: return Key::F11; case SDLK_F12: return Key::F12;
		default: return 0;
	}
}

inline void keyEvent(GLV& g, const SDL_KeyboardEvent& e){
	if(e.repeat) return;	// GLUT binding ignores key repeat too
	int k = specialKey(e.key);
	if(k){ keyToGLV(g, k, e.down); return; }

	// Printable key: SDL keycodes of printable keys are their (lowercase)
	// character in the current layout.
	if(e.key >= 32 && e.key < 127){
		if(e.down){
			if(e.mod & (SDL_KMOD_CTRL | SDL_KMOD_GUI)){
				// No text input event with ctrl/gui held; send the lowercase
				// key as the GLUT binding does for ctrl keys.
				lastChar(e.scancode) = (int)e.key;
				keyToGLV(g, (int)e.key, true);
			}
			else{
				pendingKey() = e.scancode;	// character arrives as text input
			}
		}
		else if(lastChar(e.scancode)){
			keyToGLV(g, lastChar(e.scancode), false);
			lastChar(e.scancode) = 0;
		}
	}
}

// Decodes the first UTF-8 code point of s.
inline unsigned decodeUTF8(const char * s){
	const unsigned char * u = (const unsigned char *)s;
	if(u[0] < 0x80) return u[0];
	if((u[0] & 0xE0) == 0xC0 && u[1]) return ((u[0] & 0x1F) << 6) | (u[1] & 0x3F);
	return 0xFFFF;	// outside GLV's 8-bit character range
}

inline void textEvent(GLV& g, const SDL_TextInputEvent& e){
	unsigned c = decodeUTF8(e.text);
	if(0 == c || c >= 256) return;	// GLV key codes above 255 are special keys
	SDL_Scancode sc = pendingKey();
	pendingKey() = SDL_SCANCODE_UNKNOWN;
	if(SDL_SCANCODE_UNKNOWN != sc) lastChar(sc) = (int)c;
	keyToGLV(g, (int)c, true);
}

inline void mouseButtonEvent(GLV& g, const SDL_MouseButtonEvent& e){
	space_t x = (space_t)e.x, y = (space_t)e.y;
	space_t relx = x, rely = y;

	int btn;
	switch(e.button){
		case SDL_BUTTON_LEFT:	btn = Mouse::Left; break;
		case SDL_BUTTON_MIDDLE:	btn = Mouse::Middle; break;
		case SDL_BUTTON_RIGHT:	btn = Mouse::Right; break;
		default:				btn = Mouse::Extra;
	}

	if(e.down)	g.setMouseDown(relx, rely, btn, 0);
	else		g.setMouseUp  (relx, rely, btn, 0);
	g.setMousePos((int)x, (int)y, relx, rely);
	modsToGLV(g, SDL_GetModState());
	g.propagateEvent();
}

inline void mouseMotionEvent(GLV& g, const SDL_MouseMotionEvent& e){
	space_t x = (space_t)e.x, y = (space_t)e.y;
	space_t relx = x, rely = y;
	g.setMouseMotion(relx, rely, g.mouse().isDownAny() ? Event::MouseDrag : Event::MouseMove);
	g.setMousePos((int)x, (int)y, relx, rely);
	g.propagateEvent();
}

inline void mouseWheelEvent(GLV& g, const SDL_MouseWheelEvent& e){
	int dy = (e.y > 0) - (e.y < 0);
	if(SDL_MOUSEWHEEL_FLIPPED == e.direction) dy = -dy;
	if(0 == dy) return;
	g.setMouseWheel(dy);
	g.propagateEvent();
}

inline void sizeToGLV(GLV& g, int w, int h){
	if(w > 0 && h > 0 && (g.w != w || g.h != h)){
		g.extent(w, h);
		g.broadcastEvent(Event::WindowResize);
	}
}

/// GL attributes matching GLV's default display mode (GLUT_DOUBLE|ALPHA|DEPTH);
/// call after SDL_Init() and before SDL_CreateWindow(). The window is created
/// without SDL_WINDOW_HIGH_PIXEL_DENSITY: like the GLUT examples, GLV draws at
/// window (point) resolution.
inline void windowHints(){
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
}

/// Hands the window's (current) GL context and input to GLV: loads the GL
/// functions, sends Event::WindowCreate and sizes the GLV to the window, as
/// Window::setGLV() does in the GLUT binding.
inline void attach(SDL_Window * w, GLV& g){
	SDL_StartTextInput(w);
	GLV_PLATFORM_INIT_CONTEXT
	g.broadcastEvent(Event::WindowCreate);
	int width, height;
	SDL_GetWindowSize(w, &width, &height);
	sizeToGLV(g, width, height);
}

/// Translates one SDL event for the GLV shown in window w. Returns false when
/// the application should quit (window closed).
inline bool processEvent(SDL_Window * w, GLV& g, const SDL_Event& e){
	switch(e.type){
		case SDL_EVENT_QUIT:
			return false;
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			return e.window.windowID != SDL_GetWindowID(w);
		case SDL_EVENT_WINDOW_RESIZED:
			if(e.window.windowID == SDL_GetWindowID(w)) sizeToGLV(g, e.window.data1, e.window.data2);
			break;
		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_KEY_UP:			keyEvent(g, e.key); break;
		case SDL_EVENT_TEXT_INPUT:		textEvent(g, e.text); break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP:	mouseButtonEvent(g, e.button); break;
		case SDL_EVENT_MOUSE_MOTION:	mouseMotionEvent(g, e.motion); break;
		case SDL_EVENT_MOUSE_WHEEL:		mouseWheelEvent(g, e.wheel); break;
		default:;
	}
	return true;
}

/// Resizes the window to the GLV's own fitted size (Window::fit()).
inline void fit(SDL_Window * w, GLV& g){
	g.fit();
	if(g.w > 0 && g.h > 0) SDL_SetWindowSize(w, (int)g.w, (int)g.h);
}

/// Draws one frame into the window's framebuffer (Window::Impl::draw()).
inline void draw(SDL_Window * w, GLV& g, double dsec){
	int fbw, fbh;
	SDL_GetWindowSizeInPixels(w, &fbw, &fbh);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	g.drawGLV((unsigned)fbw, (unsigned)fbh, dsec);
}

/// Toggles (desktop) fullscreen, keeping the GL context.
inline void fullScreenToggle(SDL_Window * w){
	bool full = (SDL_GetWindowFlags(w) & SDL_WINDOW_FULLSCREEN) != 0;
	SDL_SetWindowFullscreen(w, !full);
}

/// Key event handler to toggle fullscreen (glv::FullScreenToggler).
struct FullScreenToggler : EventHandler {

	FullScreenToggler(SDL_Window * w, int key_ = Key::Escape): win(w), key(key_){}

	bool onEvent(View& v, GLV& g) override {
		if(g.keyboard().key() == key){
			fullScreenToggle(win);
			return false;
		}
		return true;
	}

	SDL_Window * win;
	int key;
};

/// Sends Event::Quit and Event::WindowDestroy while the GL context still
/// exists (Application::quit() and ~Window() in the GLUT binding).
inline void detach(SDL_Window * w, GLV& g){
	g.broadcastEvent(Event::Quit);
	g.broadcastEvent(Event::WindowDestroy);
	SDL_StopTextInput(w);
}

} // glv_sdl::

#endif
