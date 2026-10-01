// sfml_unity_linux.cpp - Linux/X11 counterpart of sfml_unity.mm (see that
// file's header comment for the general rationale: single-translation-unit
// build of the vendored SFML 3.1.0 System+Window module sources).
//
// A separate .cpp file rather than a shared one selected by extension, for
// the same reason sfml_unity_windows.cpp exists: sfml_unity.mm is
// Objective-C++, which only has meaning on macOS.
//
// Platform selection inside SFML's own headers: include/SFML/Config.hpp
// detects the platform purely from compiler-predefined macros (__linux__),
// same as the macOS and Windows builds - no -D flags or project-authored
// config header needed here either.
//
// The X11 (GLX) backend is the only one built. SFML's DRM/EGL backend is a
// separate upstream CMake option (SFML_USE_DRM) for console-only systems
// and has no use in GLV's examples.

// -- System module (platform-independent + Unix) --
#include "src/SFML/System/Clock.cpp"
#include "src/SFML/System/Err.cpp"
#include "src/SFML/System/Sleep.cpp"
#include "src/SFML/System/String.cpp"
#include "src/SFML/System/TimeoutWithPredicate.cpp"
#include "src/SFML/System/Utils.cpp"
#include "src/SFML/System/Version.cpp"
#include "src/SFML/System/FileInputStream.cpp"
#include "src/SFML/System/MemoryInputStream.cpp"
#include "src/SFML/System/Unix/SleepImpl.cpp"

// -- Window module: common sources (every platform) --
#include "src/SFML/Window/Clipboard.cpp"
#include "src/SFML/Window/Context.cpp"
#include "src/SFML/Window/Cursor.cpp"
#include "src/SFML/Window/GlContext.cpp"
#include "src/SFML/Window/GlResource.cpp"
#include "src/SFML/Window/Joystick.cpp"
#include "src/SFML/Window/JoystickManager.cpp"
#include "src/SFML/Window/Keyboard.cpp"
#include "src/SFML/Window/Mouse.cpp"
#include "src/SFML/Window/Touch.cpp"
#include "src/SFML/Window/Sensor.cpp"
#include "src/SFML/Window/SensorManager.cpp"
#include "src/SFML/Window/VideoMode.cpp"
#include "src/SFML/Window/Vulkan.cpp"
#include "src/SFML/Window/Window.cpp"
#include "src/SFML/Window/WindowBase.cpp"
#include "src/SFML/Window/WindowImpl.cpp"

// -- Window module: Unix/X11 backend --
#include "src/SFML/Window/Unix/CursorImpl.cpp"
#include "src/SFML/Window/Unix/Display.cpp"
#include "src/SFML/Window/Unix/GlxContext.cpp"
#include "src/SFML/Window/Unix/InputImpl.cpp"
#include "src/SFML/Window/Unix/JoystickImpl.cpp"
#include "src/SFML/Window/Unix/KeyboardImpl.cpp"
#include "src/SFML/Window/Unix/KeySymToKeyMapping.cpp"
#include "src/SFML/Window/Unix/KeySymToUnicodeMapping.cpp"
#include "src/SFML/Window/Unix/SensorImpl.cpp"
#include "src/SFML/Window/Unix/VideoModeImpl.cpp"
// As on Windows (and unlike macOS), Vulkan.cpp takes a branch that
// #includes <SFML/Window/VulkanImpl.hpp>, so the backend's implementation
// must be linked in even though GLV never uses Vulkan.
#include "src/SFML/Window/Unix/VulkanImplX11.cpp"
#include "src/SFML/Window/Unix/WindowImplX11.cpp"
// ClipboardImpl.cpp must come after WindowImplX11.cpp, not in alphabetical
// order with the rest: both files define an unnamed-namespace checkEvent()
// X11 event predicate, and WindowImplX11.cpp's is reached through a
// function-local `using namespace WindowImplX11Impl;`. A block-scope
// using-directive nominates the name into the enclosing (global) namespace
// for lookup, so with ClipboardImpl.cpp included first its checkEvent() is
// already declared there and every call in WindowImplX11.cpp becomes
// ambiguous. In this order neither file ever sees the other's.
#include "src/SFML/Window/Unix/ClipboardImpl.cpp"
