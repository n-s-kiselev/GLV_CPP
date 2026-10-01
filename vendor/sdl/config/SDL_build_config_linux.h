// GLV SDL3 vendoring: trimmed Linux/X11 build config.
//
// Project-authored, like its macOS and Windows siblings in this folder -
// upstream ships no non-CMake Linux config header to copy, so this one is
// derived from include/build_config/SDL_build_config.h.cmake and settled by
// the compile spike recorded in docs/plans/linux-sdl-sfml-examples.md.
//
// It enables only what GLV's examples need (X11 video, GLX context
// creation, events, threads, timers, filesystem) and disables every
// subsystem they do not (audio, camera, joystick, haptic, sensor, GPU,
// dialog, process, power) via SDL's own SDL_*_DISABLED macros.
//
// SDL_VIDEO_RENDER_OGL and SDL_TRAY_DUMMY are not optional despite being
// unused here, for the same reasons as on macOS - see the comment at the
// top of SDL_build_config_macos.h.
//
// Every X11 extension is loaded through SDL_x11dyn.c at run time
// (SDL_VIDEO_DRIVER_X11_DYNAMIC_*), so only the extension *headers* are a
// build requirement and the examples link just -lX11. A machine missing,
// say, libXcursor at run time loses that feature instead of failing to
// start. The IME (ibus/fcitx/dbus), udev and evdev integrations are left
// out entirely: they only matter to the disabled subsystems and to text
// input GLV does not use.

#ifndef SDL_build_config_h_
#define SDL_build_config_h_

#include <SDL3/SDL_platform_defines.h>

/* Useful headers */
#define HAVE_ALLOCA_H 1
#define HAVE_FLOAT_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_LIMITS_H 1
#define HAVE_MATH_H 1
#define HAVE_SIGNAL_H 1
#define HAVE_STDARG_H 1
#define HAVE_STDDEF_H 1
#define HAVE_STDINT_H 1
#define HAVE_STDIO_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_WCHAR_H 1
#define HAVE_LIBC 1
#define HAVE_DLOPEN 1
#define HAVE_MALLOC 1
#define HAVE_GETENV 1
#define HAVE_GETHOSTNAME 1
#define HAVE_GETRESUID 1
#define HAVE_GETRESGID 1
#define HAVE_SETENV 1
#define HAVE_PUTENV 1
#define HAVE_UNSETENV 1
#define HAVE_ABS 1
#define HAVE_BCOPY 1
#define HAVE_MEMSET 1
#define HAVE_MEMCPY 1
#define HAVE_MEMMOVE 1
#define HAVE_MEMCMP 1
#define HAVE_STRLEN 1
#define HAVE_STRPBRK 1
#define HAVE_STRCHR 1
#define HAVE_STRRCHR 1
#define HAVE_STRSTR 1
#define HAVE_STRTOK_R 1
#define HAVE_STRTOL 1
#define HAVE_STRTOUL 1
#define HAVE_STRTOLL 1
#define HAVE_STRTOULL 1
#define HAVE_STRTOD 1
#define HAVE_ATOI 1
#define HAVE_ATOF 1
#define HAVE_STRCMP 1
#define HAVE_STRNCMP 1
#define HAVE_VSSCANF 1
#define HAVE_VSNPRINTF 1
#define HAVE_ACOS 1
#define HAVE_ACOSF 1
#define HAVE_ASIN 1
#define HAVE_ASINF 1
#define HAVE_ATAN 1
#define HAVE_ATANF 1
#define HAVE_ATAN2 1
#define HAVE_ATAN2F 1
#define HAVE_CEIL 1
#define HAVE_CEILF 1
#define HAVE_COPYSIGN 1
#define HAVE_COPYSIGNF 1
#define HAVE_COS 1
#define HAVE_COSF 1
#define HAVE_EXP 1
#define HAVE_EXPF 1
#define HAVE_FABS 1
#define HAVE_FABSF 1
#define HAVE_FLOOR 1
#define HAVE_FLOORF 1
#define HAVE_FMOD 1
#define HAVE_FMODF 1
#define HAVE_ISINF 1
#define HAVE_ISINF_FLOAT_MACRO 1
#define HAVE_ISNAN 1
#define HAVE_ISNAN_FLOAT_MACRO 1
#define HAVE_LOG 1
#define HAVE_LOGF 1
#define HAVE_LOG10 1
#define HAVE_LOG10F 1
#define HAVE_LROUND 1
#define HAVE_LROUNDF 1
#define HAVE_MODF 1
#define HAVE_MODFF 1
#define HAVE_POW 1
#define HAVE_POWF 1
#define HAVE_ROUND 1
#define HAVE_ROUNDF 1
#define HAVE_SCALBN 1
#define HAVE_SCALBNF 1
#define HAVE_SIN 1
#define HAVE_SINF 1
#define HAVE_SQRT 1
#define HAVE_SQRTF 1
#define HAVE_TAN 1
#define HAVE_TANF 1
#define HAVE_TRUNC 1
#define HAVE_TRUNCF 1
#define HAVE_SIGACTION 1
#define HAVE_SETJMP 1
#define HAVE_NANOSLEEP 1
#define HAVE_GMTIME_R 1
#define HAVE_LOCALTIME_R 1
#define HAVE_NL_LANGINFO 1
#define HAVE_SYSCONF 1
#define HAVE_GCC_ATOMICS 1

/* Subsystems this project does not need */
#define SDL_AUDIO_DISABLED 1
#define SDL_CAMERA_DISABLED 1
#define SDL_JOYSTICK_DISABLED 1
#define SDL_HAPTIC_DISABLED 1
#define SDL_SENSOR_DISABLED 1
#define SDL_GPU_DISABLED 1
#define SDL_DIALOG_DISABLED 1
#define SDL_POWER_DISABLED 1

// The process subsystem is NOT disabled here, unlike on macOS and Windows.
// SDL_PROCESS_DISABLED is a no-op macro in SDL 3.4 (nothing in src/ reads
// it), and on Unix both misc/unix/SDL_sysurl.c (SDL_OpenURL) and
// dialog/unix/SDL_zenitymessagebox.c - which X11_ShowMessageBox() calls
// unconditionally - spawn a helper process. Keeping SDL_OpenURL and the
// message box working is worth the two extra source files.
#define SDL_PROCESS_POSIX 1

#define SDL_LOADSO_DLOPEN 1
#define SDL_THREAD_PTHREAD 1
#define SDL_THREAD_PTHREAD_RECURSIVE_MUTEX 1
#define SDL_TIME_UNIX 1
#define SDL_TIMER_UNIX 1

#define SDL_VIDEO_DRIVER_X11 1
#define SDL_VIDEO_DRIVER_X11_DYNAMIC "libX11.so.6"
#define SDL_VIDEO_DRIVER_X11_DYNAMIC_XCURSOR "libXcursor.so.1"
#define SDL_VIDEO_DRIVER_X11_DYNAMIC_XEXT "libXext.so.6"
#define SDL_VIDEO_DRIVER_X11_DYNAMIC_XFIXES "libXfixes.so.3"
#define SDL_VIDEO_DRIVER_X11_DYNAMIC_XINPUT2 "libXi.so.6"
#define SDL_VIDEO_DRIVER_X11_DYNAMIC_XRANDR "libXrandr.so.2"
#define SDL_VIDEO_DRIVER_X11_DYNAMIC_XSS "libXss.so.1"
#define SDL_VIDEO_DRIVER_X11_HAS_XKBLIB 1
#define SDL_VIDEO_DRIVER_X11_SUPPORTS_GENERIC_EVENTS 1
#define SDL_VIDEO_DRIVER_X11_XCURSOR 1
#define SDL_VIDEO_DRIVER_X11_XDBE 1
#define SDL_VIDEO_DRIVER_X11_XFIXES 1
#define SDL_VIDEO_DRIVER_X11_XINPUT2 1
#define SDL_VIDEO_DRIVER_X11_XINPUT2_SUPPORTS_MULTITOUCH 1
#define SDL_VIDEO_DRIVER_X11_XRANDR 1
#define SDL_VIDEO_DRIVER_X11_XSCRNSAVER 1
#define SDL_VIDEO_DRIVER_X11_XSHAPE 1
#define SDL_VIDEO_DRIVER_X11_XSYNC 1
// SDL_VIDEO_DRIVER_X11_XTEST is deliberately left undefined: upstream's
// X11_InitXTest() is itself `#if 0`-ed out ("doesn't appear to work on
// XWayland"), so X11_XTestIsInitialized() is always false and every
// XTEST-guarded path is dead. Defining it would only add libxtst-dev to
// the build requirements.
#define SDL_VIDEO_OPENGL 1
#define SDL_VIDEO_OPENGL_GLX 1
#define SDL_VIDEO_RENDER_OGL 1

#define SDL_TRAY_DUMMY 1

#define SDL_FILESYSTEM_UNIX 1
#define SDL_FSOPS_POSIX 1

#endif
