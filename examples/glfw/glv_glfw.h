#ifndef INC_GLV_EXAMPLES_GLFW_H
#define INC_GLV_EXAMPLES_GLFW_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*	Header-only glue between GLFW3 and a GLV top view, shared by the examples
	in this folder. It replaces examples/glut/example.h: each example creates
	its own GLFW window and runs its own main loop, and uses these helpers only
	to hand the GL context and the input events to GLV. The translation follows
	src/glv_binding_glut.cpp so that the examples behave as their GLUT
	counterparts do. Not part of GLV's public API. */

#include <math.h>
#include "glv.h"		// first: provides the OpenGL declarations (GLAD or system)
#include "glv_util.h"

#define GLFW_INCLUDE_NONE	// GL headers already come from glv.h
#include <GLFW/glfw3.h>

using namespace glv;

namespace glv_glfw{

inline GLV * glvOf(GLFWwindow * w){ return static_cast<GLV *>(glfwGetWindowUserPointer(w)); }

// Character last sent as a key-down for each GLFW key, so that the matching
// key-up reports the same character (GLFW reports releases by key only).
inline int& lastChar(int key){
	static int chars[GLFW_KEY_LAST + 1] = {0};
	static int invalid = 0;
	return (key >= 0 && key <= GLFW_KEY_LAST) ? chars[key] : invalid;
}

// Printable key waiting for its character callback (see keyCB)
inline int& pendingKey(){ static int key = GLFW_KEY_UNKNOWN; return key; }
inline int& currentMods(){ static int mods = 0; return mods; }

inline void modsToGLV(GLV& g, int mods){
	currentMods() = mods;
	g.setKeyModifiers(
		mods & GLFW_MOD_SHIFT,
		mods & GLFW_MOD_ALT,
		mods & GLFW_MOD_CONTROL,
		mods & GLFW_MOD_CAPS_LOCK,
		mods & GLFW_MOD_SUPER
	);
}

inline void keyToGLV(GLV& g, int key, bool down){
	down ? g.setKeyDown(key) : g.setKeyUp(key);
	modsToGLV(g, currentMods());
	g.propagateEvent();
}

// Maps non-printable GLFW keys to GLV key codes; returns 0 for other keys.
inline int specialKey(int key){
	switch(key){
		case GLFW_KEY_ENTER:
		case GLFW_KEY_KP_ENTER:		return Key::Return;
		case GLFW_KEY_BACKSPACE:	return Key::Backspace;
		case GLFW_KEY_TAB:			return Key::Tab;
		case GLFW_KEY_ESCAPE:		return Key::Escape;
		case GLFW_KEY_DELETE:		return Key::Delete;
		case GLFW_KEY_LEFT:			return Key::Left;
		case GLFW_KEY_UP:			return Key::Up;
		case GLFW_KEY_RIGHT:		return Key::Right;
		case GLFW_KEY_DOWN:			return Key::Down;
		case GLFW_KEY_PAGE_UP:		return Key::PageUp;
		case GLFW_KEY_PAGE_DOWN:	return Key::PageDown;
		case GLFW_KEY_HOME:			return Key::Home;
		case GLFW_KEY_END:			return Key::End;
		case GLFW_KEY_INSERT:		return Key::Insert;
		default:;
	}
	if(key >= GLFW_KEY_F1 && key <= GLFW_KEY_F12) return Key::F1 + (key - GLFW_KEY_F1);
	return 0;
}

inline void keyCB(GLFWwindow * w, int key, int scancode, int action, int mods){
	GLV * g = glvOf(w);
	if(!g || GLFW_REPEAT == action) return;	// GLUT binding ignores key repeat too
	currentMods() = mods;
	bool down = GLFW_PRESS == action;

	int k = specialKey(key);
	if(k){ keyToGLV(*g, k, down); return; }

	// Printable key (GLFW key codes are US-layout ASCII in this range)
	if(key >= GLFW_KEY_SPACE && key <= GLFW_KEY_GRAVE_ACCENT){
		if(down){
			if(mods & (GLFW_MOD_CONTROL | GLFW_MOD_SUPER)){
				// No character callback with ctrl/super held; send the
				// lowercase ASCII key as the GLUT binding does for ctrl keys.
				int c = (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) ? key + 32 : key;
				lastChar(key) = c;
				keyToGLV(*g, c, true);
			}
			else{
				pendingKey() = key;	// character arrives in charCB
			}
		}
		else if(lastChar(key)){
			keyToGLV(*g, lastChar(key), false);
			lastChar(key) = 0;
		}
	}
}

inline void charCB(GLFWwindow * w, unsigned int codepoint){
	GLV * g = glvOf(w);
	if(!g || codepoint >= 256) return;	// GLV key codes above 255 are special keys
	int key = pendingKey();
	pendingKey() = GLFW_KEY_UNKNOWN;
	if(GLFW_KEY_UNKNOWN != key) lastChar(key) = (int)codepoint;
	keyToGLV(*g, (int)codepoint, true);
}

inline void mouseButtonCB(GLFWwindow * w, int button, int action, int mods){
	GLV * g = glvOf(w);
	if(!g) return;
	double ax, ay;
	glfwGetCursorPos(w, &ax, &ay);
	space_t x = (space_t)ax, y = (space_t)ay;
	space_t relx = x, rely = y;

	int btn;
	switch(button){
		case GLFW_MOUSE_BUTTON_LEFT:	btn = Mouse::Left; break;
		case GLFW_MOUSE_BUTTON_MIDDLE:	btn = Mouse::Middle; break;
		case GLFW_MOUSE_BUTTON_RIGHT:	btn = Mouse::Right; break;
		default:						btn = Mouse::Extra;
	}

	if(GLFW_PRESS == action)	g->setMouseDown(relx, rely, btn, 0);
	else						g->setMouseUp  (relx, rely, btn, 0);
	g->setMousePos((int)x, (int)y, relx, rely);
	modsToGLV(*g, mods);
	g->propagateEvent();
}

inline void cursorPosCB(GLFWwindow * w, double ax, double ay){
	GLV * g = glvOf(w);
	if(!g) return;
	space_t x = (space_t)ax, y = (space_t)ay;
	space_t relx = x, rely = y;
	g->setMouseMotion(relx, rely, g->mouse().isDownAny() ? Event::MouseDrag : Event::MouseMove);
	g->setMousePos((int)x, (int)y, relx, rely);
	g->propagateEvent();
}

inline void scrollCB(GLFWwindow * w, double dx, double dy){
	GLV * g = glvOf(w);
	if(!g || 0 == (int)dy) return;
	g->setMouseWheel((int)dy);
	g->propagateEvent();
}

inline void sizeToGLV(GLV& g, int w, int h){
	if(w > 0 && h > 0 && (g.w != w || g.h != h)){
		g.extent(w, h);
		g.broadcastEvent(Event::WindowResize);
	}
}

inline void windowSizeCB(GLFWwindow * w, int width, int height){
	GLV * g = glvOf(w);
	if(g) sizeToGLV(*g, width, height);
}

// Window hints matching GLV's default display mode (GLUT_DOUBLE|ALPHA|DEPTH).
// No retina framebuffer: like the GLUT examples, GLV draws at window
// (point) resolution.
inline void windowHints(){
	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
}

/// Hands the window's (current) GL context and input to GLV: loads the GL
/// functions, sends Event::WindowCreate and sizes the GLV to the window, as
/// Window::setGLV() does in the GLUT binding.
inline void attach(GLFWwindow * w, GLV& g){
	glfwSetWindowUserPointer(w, &g);
	glfwSetKeyCallback(w, keyCB);
	glfwSetCharCallback(w, charCB);
	glfwSetMouseButtonCallback(w, mouseButtonCB);
	glfwSetCursorPosCallback(w, cursorPosCB);
	glfwSetScrollCallback(w, scrollCB);
	glfwSetWindowSizeCallback(w, windowSizeCB);

	GLV_PLATFORM_INIT_CONTEXT
	g.broadcastEvent(Event::WindowCreate);
	int width, height;
	glfwGetWindowSize(w, &width, &height);
	sizeToGLV(g, width, height);
}

/// Resizes the window to the GLV's own fitted size (Window::fit()).
inline void fit(GLFWwindow * w, GLV& g){
	g.fit();
	if(g.w > 0 && g.h > 0) glfwSetWindowSize(w, (int)g.w, (int)g.h);
}

/// Draws one frame into the window's framebuffer (Window::Impl::draw()).
inline void draw(GLFWwindow * w, GLV& g, double dsec){
	int fbw, fbh;
	glfwGetFramebufferSize(w, &fbw, &fbh);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	g.drawGLV((unsigned)fbw, (unsigned)fbh, dsec);
}

/// Toggles fullscreen on the window's current monitor, keeping the GL context.
inline void fullScreenToggle(GLFWwindow * w){
	static int wx = 0, wy = 0, ww = 800, wh = 600;
	if(glfwGetWindowMonitor(w)){
		glfwSetWindowMonitor(w, NULL, wx, wy, ww, wh, GLFW_DONT_CARE);
	}
	else{
		glfwGetWindowPos(w, &wx, &wy);
		glfwGetWindowSize(w, &ww, &wh);
		GLFWmonitor * m = glfwGetPrimaryMonitor();
		const GLFWvidmode * mode = glfwGetVideoMode(m);
		glfwSetWindowMonitor(w, m, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
}

/// Key event handler to toggle fullscreen (glv::FullScreenToggler).
struct FullScreenToggler : EventHandler {

	FullScreenToggler(GLFWwindow * w, int key_ = Key::Escape): win(w), key(key_){}

	bool onEvent(View& v, GLV& g) override {
		if(g.keyboard().key() == key){
			fullScreenToggle(win);
			return false;
		}
		return true;
	}

	GLFWwindow * win;
	int key;
};

/// Sends Event::Quit and Event::WindowDestroy while the GL context still
/// exists (Application::quit() and ~Window() in the GLUT binding).
inline void detach(GLFWwindow * w, GLV& g){
	g.broadcastEvent(Event::Quit);
	g.broadcastEvent(Event::WindowDestroy);
	glfwSetWindowUserPointer(w, NULL);
}

} // glv_glfw::

#endif
