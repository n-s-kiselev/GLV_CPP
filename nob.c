#define NOB_IMPLEMENTATION
#include "vendor/nob/nob.h"

// Build script for GLV (Graphics Library of Views). Replaces the former GNU
// Make stack (Makefile/Makefile.common/Makefile.config/Makefile.rules) and
// the Xcode project; structure follows the sibling AntTweakBarC99 nob.c.
// See docs/plans/nob-build-system.md.

#define SRC_FOLDER           "src/"
#define INCLUDE_FOLDER       "GLV/"
#define EXAMPLES_FOLDER      "examples/"
#define EXAMPLES_GLUT_FOLDER EXAMPLES_FOLDER "glut/"
#define EXAMPLES_GLFW_FOLDER EXAMPLES_FOLDER "glfw/"
#define EXAMPLES_SDL_FOLDER  EXAMPLES_FOLDER "sdl/"
#define EXAMPLES_SFML_FOLDER EXAMPLES_FOLDER "sfml/"
#define TESTS_FOLDER         "test/"
#define BUILD_FOLDER         "build/"
#define BUILD_STATIC_FOLDER  BUILD_FOLDER "static/"
#define BUILD_SHARED_FOLDER  BUILD_FOLDER "shared/"
// Every build artifact - including the final libraries and the copy of the
// public headers consuming code would build against - lives under
// BUILD_FOLDER, keeping the repository root free of anything but source.
#define LIB_FOLDER           BUILD_FOLDER "lib/"
#define BUILD_INCLUDE_FOLDER BUILD_FOLDER "include/"
#define BUILD_GLV_INCLUDE    BUILD_INCLUDE_FOLDER "GLV/"
#define EXAMPLES_BUILD_FOLDER BUILD_FOLDER "examples/"
// Split by link mode AND backend (as in AntTweakBarC99's nob.c): every
// combination gets its own folder, so build_needed() - which only compares
// mtimes against a fixed output path - never mistakes another combination's
// binary for an up-to-date one.
#define EXAMPLES_STATIC_GLUT_FOLDER EXAMPLES_BUILD_FOLDER "static-glut/"
#define EXAMPLES_STATIC_GLFW_FOLDER EXAMPLES_BUILD_FOLDER "static-glfw/"
#define EXAMPLES_STATIC_SDL_FOLDER  EXAMPLES_BUILD_FOLDER "static-sdl/"
#define EXAMPLES_STATIC_SFML_FOLDER EXAMPLES_BUILD_FOLDER "static-sfml/"
#define EXAMPLES_SHARED_GLUT_FOLDER EXAMPLES_BUILD_FOLDER "shared-glut/"
#define EXAMPLES_SHARED_GLFW_FOLDER EXAMPLES_BUILD_FOLDER "shared-glfw/"
#define EXAMPLES_SHARED_SDL_FOLDER  EXAMPLES_BUILD_FOLDER "shared-sdl/"
#define EXAMPLES_SHARED_SFML_FOLDER EXAMPLES_BUILD_FOLDER "shared-sfml/"
#define TESTS_BUILD_FOLDER   BUILD_FOLDER "tests/"
#define NOB_HEADER           "vendor/nob/nob.h"

#define LIB_STATIC LIB_FOLDER "libGLV.a"

#if defined(_WIN32)
#define LIB_SHARED LIB_FOLDER "libGLV.dll"
#define LIB_IMPORT LIB_FOLDER "libGLV.dll.a"
#elif defined(__APPLE__)
#define LIB_SHARED LIB_FOLDER "libGLV.dylib"
#else
#define LIB_SHARED        LIB_FOLDER "libGLV.so"
#define LIB_SHARED_SONAME LIB_FOLDER "libGLV.so.1"
#define LIB_SHARED_SONAME_NAME "libGLV.so.1"
#endif

#if defined(_WIN32)
#define EXE_EXT ".exe"
#else
#define EXE_EXT ""
#endif

// Which windowing toolkit an example set is built against. GLUT examples use
// GLV's own Window/Application binding (src/glv_binding_glut.cpp); the other
// three create their own window and main loop, like AntTweakBarC99's
// examples, and translate events through a header-only helper in their own
// folder (examples/<backend>/glv_<backend>.h).
typedef enum {
    BACKEND_GLUT,
    BACKEND_GLFW,
    BACKEND_SDL,
    BACKEND_SFML,
} Backend;

static const char *backend_name(Backend backend)
{
    switch (backend) {
    case BACKEND_GLUT: return "GLUT";
    case BACKEND_GLFW: return "GLFW3";
    case BACKEND_SDL:  return "SDL3";
    case BACKEND_SFML: return "SFML3";
    }
    return "";
}

// GLAD replaces GLEW as GLV's OpenGL loader on Linux and Windows (see
// GLV/glv_conf.h). It is compiled into libGLV itself there, so examples get
// the loader from the static or shared library. macOS uses the system
// OpenGL framework directly and does not need it.
#define GLAD_INCLUDE "vendor/glad/include/"
#define GLAD_SRC     "vendor/glad/src/glad.c"

// The GLFW3, SDL3 and SFML3 toolkits are vendored and built exactly as in
// AntTweakBarC99's nob.c (the vendored trees are byte-identical copies).
// They are only needed by the examples; libGLV links none of them.
#define GLFW_INCLUDE  "vendor/glfw/include/"
#define GLFW_SRC      "vendor/glfw/glfw_unity.c"
#define GLFW_OBJ      EXAMPLES_BUILD_FOLDER "glfw.o"

// SDL3's private headers are not safe to concatenate into one translation
// unit, so sdl_sources_*[] below list the exact upstream files needed and
// each is compiled to its own object, then archived into SDL_LIB.
// vendor/sdl/config/ holds a trimmed SDL_build_config.h (video+events+GL
// only).
#define SDL_INCLUDE        "vendor/sdl/include/"
#define SDL_CONFIG_INCLUDE "vendor/sdl/config/"
#define SDL_SRC_ROOT       "vendor/sdl/src/"
#define SDL_STUB_SRC       "vendor/sdl/sdl_stubs.c"
#define SDL_OBJ_FOLDER     EXAMPLES_BUILD_FOLDER "sdl_obj/"
#define SDL_LIB            EXAMPLES_BUILD_FOLDER "libsdl3_vendored.a"

// SFML3 (System+Window modules only) is unity-build-safe, so it is a single
// object again. MinGW GCC has no Objective-C++ front end, so Windows uses a
// separate plain-C++ unity file.
#define SFML_INCLUDE  "vendor/sfml/include/"
#if defined(_WIN32)
#define SFML_SRC "vendor/sfml/sfml_unity_windows.cpp"
#elif defined(__APPLE__)
#define SFML_SRC "vendor/sfml/sfml_unity.mm"
#else
#define SFML_SRC "vendor/sfml/sfml_unity_linux.cpp"
#endif
#define SFML_OBJ      EXAMPLES_BUILD_FOLDER "sfml.o"

// FreeGLUT is vendored for every supported platform (copied verbatim from
// the sibling AntTweakBar-Legacy repo, including its Cocoa backend), because
// GLV's only window binding, src/glv_binding_glut.cpp, is built into the
// library itself (the old Makefile.config's WINDOW_BINDING = GLUT). Its
// archive is kept in LIB_FOLDER next to libGLV.a: anything linking the
// static library needs it too.
#define FREEGLUT_INCLUDE      "vendor/freeglut/include/"
#define FREEGLUT_SRC_FOLDER   "vendor/freeglut/src/"
#define FREEGLUT_BUILD_FOLDER BUILD_FOLDER "freeglut/"
#define FREEGLUT_LIB          LIB_FOLDER "libfreeglut.a"

// Core list mirrors upstream's own non-CMake build (altbuild/Makefile);
// backend lists mirror CMakeLists.txt's WIN32/Cocoa/X11 source lists.
#if defined(_WIN32)
#define FREEGLUT_CONFIG_INCLUDE FREEGLUT_SRC_FOLDER "mswin/"
#elif defined(__APPLE__)
#define FREEGLUT_CONFIG_INCLUDE FREEGLUT_SRC_FOLDER "cocoa/"
#else
#define FREEGLUT_CONFIG_INCLUDE FREEGLUT_SRC_FOLDER "x11/"
#endif

static const char *freeglut_core_sources[] = {
    FREEGLUT_SRC_FOLDER "fg_callbacks.c",
    FREEGLUT_SRC_FOLDER "fg_cursor.c",
    FREEGLUT_SRC_FOLDER "fg_display.c",
    FREEGLUT_SRC_FOLDER "fg_ext.c",
    FREEGLUT_SRC_FOLDER "fg_font_data.c",
    FREEGLUT_SRC_FOLDER "fg_font.c",
    FREEGLUT_SRC_FOLDER "fg_gamemode.c",
    FREEGLUT_SRC_FOLDER "fg_geometry.c",
    FREEGLUT_SRC_FOLDER "fg_gl2.c",
    FREEGLUT_SRC_FOLDER "fg_init.c",
    FREEGLUT_SRC_FOLDER "fg_input_devices.c",
    FREEGLUT_SRC_FOLDER "fg_joystick.c",
    FREEGLUT_SRC_FOLDER "fg_main.c",
    FREEGLUT_SRC_FOLDER "fg_menu.c",
    FREEGLUT_SRC_FOLDER "fg_misc.c",
    FREEGLUT_SRC_FOLDER "fg_overlay.c",
    FREEGLUT_SRC_FOLDER "fg_spaceball.c",
    FREEGLUT_SRC_FOLDER "fg_state.c",
    FREEGLUT_SRC_FOLDER "fg_stroke_mono_roman.c",
    FREEGLUT_SRC_FOLDER "fg_stroke_roman.c",
    FREEGLUT_SRC_FOLDER "fg_structure.c",
    FREEGLUT_SRC_FOLDER "fg_teapot.c",
    FREEGLUT_SRC_FOLDER "fg_videoresize.c",
    FREEGLUT_SRC_FOLDER "fg_window.c",
};

#if defined(_WIN32)
static const char *freeglut_platform_sources[] = {
    FREEGLUT_SRC_FOLDER "mswin/fg_cursor_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_display_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_ext_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_gamemode_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_init_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_input_devices_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_joystick_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_main_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_menu_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_spaceball_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_state_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_structure_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_window_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_cmap_mswin.c",
    // Windows has no XParseGeometry() (no Xlib); freeglut supplies its own.
    FREEGLUT_SRC_FOLDER "util/xparsegeometry_repl.c",
};
#elif defined(__APPLE__)
static const char *freeglut_platform_sources[] = {
    FREEGLUT_SRC_FOLDER "cocoa/fg_cmap_cocoa.m",
    FREEGLUT_SRC_FOLDER "cocoa/fg_cursor_cocoa.m",
    FREEGLUT_SRC_FOLDER "cocoa/fg_display_cocoa.m",
    FREEGLUT_SRC_FOLDER "cocoa/fg_ext_cocoa.c",
    FREEGLUT_SRC_FOLDER "cocoa/fg_gamemode_cocoa.m",
    FREEGLUT_SRC_FOLDER "cocoa/fg_glut_definitions_cocoa.c",
    FREEGLUT_SRC_FOLDER "cocoa/fg_init_cocoa.m",
    FREEGLUT_SRC_FOLDER "cocoa/fg_input_devices_cocoa.c",
    FREEGLUT_SRC_FOLDER "cocoa/fg_joystick_cocoa.m",
    FREEGLUT_SRC_FOLDER "cocoa/fg_main_cocoa.m",
    FREEGLUT_SRC_FOLDER "cocoa/fg_spaceball_cocoa.m",
    FREEGLUT_SRC_FOLDER "cocoa/fg_state_cocoa.m",
    FREEGLUT_SRC_FOLDER "cocoa/fg_structure_cocoa.c",
    FREEGLUT_SRC_FOLDER "cocoa/fg_window_cocoa.m",
    // Cocoa has no Xlib, so upstream's feature probe selects this fallback.
    FREEGLUT_SRC_FOLDER "util/xparsegeometry_repl.c",
};
#else
static const char *freeglut_platform_sources[] = {
    FREEGLUT_SRC_FOLDER "x11/fg_cursor_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_ext_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_gamemode_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_glutfont_definitions_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_init_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_input_devices_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_joystick_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_main_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_menu_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_spaceball_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_state_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_structure_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_window_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_xinput_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_cmap_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_display_x11_glx.c",
    FREEGLUT_SRC_FOLDER "x11/fg_state_x11_glx.c",
    FREEGLUT_SRC_FOLDER "x11/fg_window_x11_glx.c",
};
#endif

// Library sources: the old Makefile's SRCS plus the GLUT window binding
// (glv_binding.cpp + glv_binding_glut.cpp, added there by WINDOW_BINDING).
static const char *library_sources[] = {
    SRC_FOLDER "glv_buttons.cpp",
    SRC_FOLDER "glv_color.cpp",
    SRC_FOLDER "glv_color_controls.cpp",
    SRC_FOLDER "glv_core.cpp",
    SRC_FOLDER "glv_draw.cpp",
    SRC_FOLDER "glv_font.cpp",
    SRC_FOLDER "glv_glv.cpp",
    SRC_FOLDER "glv_grid.cpp",
    SRC_FOLDER "glv_inputdevice.cpp",
    SRC_FOLDER "glv_layout.cpp",
    SRC_FOLDER "glv_model.cpp",
    SRC_FOLDER "glv_notification.cpp",
    SRC_FOLDER "glv_plots.cpp",
    SRC_FOLDER "glv_preset_controls.cpp",
    SRC_FOLDER "glv_sliders.cpp",
    SRC_FOLDER "glv_sono.cpp",
    SRC_FOLDER "glv_texture.cpp",
    SRC_FOLDER "glv_textview.cpp",
    SRC_FOLDER "glv_view.cpp",
    SRC_FOLDER "glv_view3D.cpp",
    SRC_FOLDER "glv_widget.cpp",
    SRC_FOLDER "glv_binding.cpp",
    SRC_FOLDER "glv_binding_glut.cpp",
#if !defined(__APPLE__)
    GLAD_SRC,
#endif
};

static const char *public_headers[] = {
    "glv.h", "glv_behavior.h", "glv_binding.h", "glv_buttons.h", "glv_color.h",
    "glv_color_controls.h", "glv_conf.h", "glv_core.h", "glv_draw.h", "glv_font.h",
    "glv_grid.h", "glv_icon.h", "glv_layout.h", "glv_model.h", "glv_notification.h",
    "glv_plots.h", "glv_preset_controls.h", "glv_rect.h", "glv_sliders.h", "glv_sono.h",
    "glv_texture.h", "glv_textview.h", "glv_util.h", "glv_view3D.h", "glv_widget.h",
};

// The same 25 programs exist once per backend, as
// examples/<backend>/<name>_<backend>.cpp.
#define GLV_EXAMPLES(X) \
    X(attachVariable) X(chladni) X(dataPlot) X(densityPlot) X(drawText) \
    X(graphicsData) X(grid) X(icons) X(mandelbrot) X(notification) \
    X(paintDiffusion) X(paintSimple) X(paintWave) X(presets) X(scope) \
    X(showcase) X(slidersWithValues) X(soaring) X(spaceCurve) X(spinSynth) \
    X(stretchAnchor) X(tableLayout) X(textureView) X(views) X(widgets)

#define GLUT_EXAMPLE(name) EXAMPLES_GLUT_FOLDER #name "_glut.cpp",
#define GLFW_EXAMPLE(name) EXAMPLES_GLFW_FOLDER #name "_glfw.cpp",
#define SDL_EXAMPLE(name)  EXAMPLES_SDL_FOLDER  #name "_sdl.cpp",
#define SFML_EXAMPLE(name) EXAMPLES_SFML_FOLDER #name "_sfml.cpp",
static const char *glut_examples[] = { GLV_EXAMPLES(GLUT_EXAMPLE) };
static const char *glfw_examples[] = { GLV_EXAMPLES(GLFW_EXAMPLE) };
static const char *sdl_examples[]  = { GLV_EXAMPLES(SDL_EXAMPLE) };
static const char *sfml_examples[] = { GLV_EXAMPLES(SFML_EXAMPLE) };

// The exact upstream vendor/sdl/src/ files needed for a working
// OpenGL+events SDL3 build. The common/macOS/Windows lists come from
// AntTweakBarC99's nob.c, where they were validated on macOS and Windows
// (see its docs/plans/sdl3-backend.md); the Linux/X11 list below was
// settled by the compile spike in docs/plans/linux-sdl-sfml-examples.md.
// Paths are relative to SDL_SRC_ROOT.
static const char *sdl_sources_common[] = {
    "atomic/SDL_atomic.c", "atomic/SDL_spinlock.c",
    "cpuinfo/SDL_cpuinfo.c",
    "dynapi/SDL_dynapi.c",
    "events/imKStoUCS.c", "events/SDL_categories.c", "events/SDL_clipboardevents.c",
    "events/SDL_displayevents.c", "events/SDL_dropevents.c", "events/SDL_events.c",
    "events/SDL_eventwatch.c", "events/SDL_keyboard.c", "events/SDL_keymap.c",
    "events/SDL_keysym_to_keycode.c", "events/SDL_keysym_to_scancode.c", "events/SDL_mouse.c",
    "events/SDL_pen.c", "events/SDL_quit.c", "events/SDL_scancode_tables.c",
    "events/SDL_touch.c", "events/SDL_windowevents.c",
    "filesystem/SDL_filesystem.c",
    "io/generic/SDL_asyncio_generic.c", "io/SDL_asyncio.c", "io/SDL_iostream.c",
    "libm/e_atan2.c", "libm/e_exp.c", "libm/e_fmod.c", "libm/e_log.c", "libm/e_log10.c",
    "libm/e_pow.c", "libm/e_rem_pio2.c", "libm/e_sqrt.c", "libm/k_cos.c", "libm/k_rem_pio2.c",
    "libm/k_sin.c", "libm/k_tan.c", "libm/s_atan.c", "libm/s_copysign.c", "libm/s_cos.c",
    "libm/s_fabs.c", "libm/s_floor.c", "libm/s_isinf.c", "libm/s_isinff.c", "libm/s_isnan.c",
    "libm/s_isnanf.c", "libm/s_modf.c", "libm/s_scalbn.c", "libm/s_sin.c", "libm/s_tan.c",
    "locale/SDL_locale.c",
    "main/SDL_main_callbacks.c",
    "misc/SDL_url.c",
    "render/opengl/SDL_render_gl.c", "render/opengl/SDL_shaders_gl.c",
    "render/SDL_render_unsupported.c", "render/SDL_render.c", "render/SDL_yuv_sw.c",
    "render/software/SDL_blendfillrect.c", "render/software/SDL_blendline.c",
    "render/software/SDL_blendpoint.c", "render/software/SDL_drawline.c",
    "render/software/SDL_drawpoint.c", "render/software/SDL_render_sw.c", "render/software/SDL_triangle.c",
    "SDL_assert.c", "SDL_error.c", "SDL_guid.c", "SDL_hashtable.c", "SDL_hints.c",
    "SDL_list.c", "SDL_log.c", "SDL_properties.c", "SDL_utils.c", "SDL.c",
    "stdlib/SDL_crc16.c", "stdlib/SDL_crc32.c", "stdlib/SDL_getenv.c", "stdlib/SDL_iconv.c",
    "stdlib/SDL_malloc.c", "stdlib/SDL_memcpy.c", "stdlib/SDL_memmove.c", "stdlib/SDL_memset.c",
    "stdlib/SDL_murmur3.c", "stdlib/SDL_qsort.c", "stdlib/SDL_random.c", "stdlib/SDL_stdlib.c",
    "stdlib/SDL_string.c", "stdlib/SDL_strtokr.c",
    "thread/SDL_thread.c",
    "time/SDL_time.c",
    "timer/SDL_timer.c",
    "tray/dummy/SDL_tray.c", "tray/SDL_tray_utils.c",
    "video/SDL_blit_0.c", "video/SDL_blit_1.c", "video/SDL_blit_A.c", "video/SDL_blit_auto.c",
    "video/SDL_blit_copy.c", "video/SDL_blit_N.c", "video/SDL_blit_slow.c", "video/SDL_blit.c",
    "video/SDL_bmp.c", "video/SDL_clipboard.c", "video/SDL_egl.c", "video/SDL_fillrect.c",
    "video/SDL_pixels.c", "video/SDL_rect.c", "video/SDL_RLEaccel.c", "video/SDL_rotate.c",
    "video/SDL_stb.c", "video/SDL_stretch.c", "video/SDL_surface.c", "video/SDL_video.c",
    "video/SDL_vulkan_utils.c", "video/SDL_yuv.c",
    "video/yuv2rgb/yuv_rgb_std.c",
};

static const char *sdl_sources_macos[] = {
    "filesystem/cocoa/SDL_sysfilesystem.m", "filesystem/posix/SDL_sysfsops.c",
    "loadso/dlopen/SDL_sysloadso.c",
    "locale/macos/SDL_syslocale.m",
    "misc/macos/SDL_sysurl.m",
    "thread/pthread/SDL_syscond.c", "thread/pthread/SDL_sysmutex.c", "thread/pthread/SDL_sysrwlock.c",
    "thread/pthread/SDL_syssem.c", "thread/pthread/SDL_systhread.c", "thread/pthread/SDL_systls.c",
    "time/unix/SDL_systime.c",
    "timer/unix/SDL_systimer.c",
    "video/cocoa/SDL_cocoaclipboard.m", "video/cocoa/SDL_cocoaevents.m", "video/cocoa/SDL_cocoakeyboard.m",
    "video/cocoa/SDL_cocoamessagebox.m", "video/cocoa/SDL_cocoamodes.m", "video/cocoa/SDL_cocoamouse.m",
    "video/cocoa/SDL_cocoaopengl.m", "video/cocoa/SDL_cocoapen.m", "video/cocoa/SDL_cocoashape.m",
    "video/cocoa/SDL_cocoavideo.m", "video/cocoa/SDL_cocoawindow.m",
};

static const char *sdl_sources_linux[] = {
    "core/linux/SDL_threadprio.c",
    // SDL.c calls SDL_Gtk_Quit() unconditionally on Unix; GTK itself is
    // dlopen()ed, so this adds no link-time dependency.
    "core/unix/SDL_appid.c", "core/unix/SDL_gtk.c", "core/unix/SDL_poll.c",
    // X11_ShowMessageBox() calls SDL_Zenity_ShowMessageBox() unconditionally,
    // so this one dialog/ file is needed even though SDL_DIALOG_DISABLED is set.
    "dialog/unix/SDL_zenitymessagebox.c",
    "filesystem/posix/SDL_sysfsops.c", "filesystem/unix/SDL_sysfilesystem.c",
    "loadso/dlopen/SDL_sysloadso.c",
    "locale/unix/SDL_syslocale.c",
    "misc/unix/SDL_sysurl.c",
    // Required by SDL_sysurl.c and SDL_zenitymessagebox.c above - see
    // SDL_PROCESS_POSIX in vendor/sdl/config/SDL_build_config_linux.h.
    "process/posix/SDL_posixprocess.c", "process/SDL_process.c",
    "thread/pthread/SDL_syscond.c", "thread/pthread/SDL_sysmutex.c", "thread/pthread/SDL_sysrwlock.c",
    "thread/pthread/SDL_syssem.c", "thread/pthread/SDL_systhread.c", "thread/pthread/SDL_systls.c",
    "time/unix/SDL_systime.c",
    "timer/unix/SDL_systimer.c",
    "video/x11/edid-parse.c", "video/x11/SDL_x11clipboard.c", "video/x11/SDL_x11dyn.c",
    "video/x11/SDL_x11events.c", "video/x11/SDL_x11framebuffer.c", "video/x11/SDL_x11keyboard.c",
    "video/x11/SDL_x11messagebox.c", "video/x11/SDL_x11modes.c", "video/x11/SDL_x11mouse.c",
    "video/x11/SDL_x11opengl.c", "video/x11/SDL_x11opengles.c", "video/x11/SDL_x11pen.c",
    "video/x11/SDL_x11settings.c", "video/x11/SDL_x11shape.c", "video/x11/SDL_x11toolkit.c",
    "video/x11/SDL_x11touch.c", "video/x11/SDL_x11video.c", "video/x11/SDL_x11vulkan.c",
    "video/x11/SDL_x11window.c", "video/x11/SDL_x11xfixes.c", "video/x11/SDL_x11xinput2.c",
    "video/x11/SDL_x11xsync.c", "video/x11/SDL_x11xtest.c", "video/x11/xsettings-client.c",
    // SDL_yuv.c always compiles its SSE2 path on x86/x86_64.
    "video/yuv2rgb/yuv_rgb_sse.c",
};

static const char *sdl_sources_win32[] = {
    "core/windows/SDL_hid.c", "core/windows/SDL_windows.c",
    "filesystem/windows/SDL_sysfilesystem.c", "filesystem/windows/SDL_sysfsops.c",
    "loadso/windows/SDL_sysloadso.c",
    "locale/windows/SDL_syslocale.c",
    "misc/windows/SDL_sysurl.c",
    // The Windows CV/SRW-lock implementations fall back to these generic ones
    // for mutex kinds they don't natively support.
    "thread/generic/SDL_syscond.c", "thread/generic/SDL_sysrwlock.c",
    "thread/windows/SDL_syscond_cv.c", "thread/windows/SDL_sysmutex.c",
    "thread/windows/SDL_sysrwlock_srw.c",
    "thread/windows/SDL_syssem.c", "thread/windows/SDL_systhread.c", "thread/windows/SDL_systls.c",
    "time/windows/SDL_systime.c",
    "timer/windows/SDL_systimer.c",
    "video/windows/SDL_windowsclipboard.c", "video/windows/SDL_windowsevents.c",
    "video/windows/SDL_windowsframebuffer.c", "video/windows/SDL_windowskeyboard.c",
    "video/windows/SDL_windowsmessagebox.c", "video/windows/SDL_windowsmodes.c",
    "video/windows/SDL_windowsmouse.c", "video/windows/SDL_windowsopengl.c",
    "video/windows/SDL_windowsrawinput.c",
    // C++, but with HAVE_GAMEINPUT_H undefined only its no-op stub branch is
    // compiled; SDL_windowsvideo.c calls it unconditionally.
    "video/windows/SDL_windowsgameinput.cpp",
    "video/windows/SDL_windowsshape.c", "video/windows/SDL_windowsvideo.c",
    "video/windows/SDL_windowswindow.c",
    // SDL_yuv.c always compiles its SSE2 path on x86/x86_64.
    "video/yuv2rgb/yuv_rgb_sse.c",
};

// Every test/*.cpp is a standalone program, but only test_units (headless,
// no window) is built and run - exactly what the old `make test` target did.
// The other test/*.cpp programs (test_events, test_glv, test_icon,
// test_multi_window, test_notifications, test_text, test_traversal,
// test_widgets, test_window) are not built: they all include
// test/test_glv.h, whose `char str[2] = {input, '\0'};` (an int narrowed to
// char in a braced initializer) is ill-formed C++11/14 and rejected by
// clang. Building them needs a source fix, which is outside this build-system
// change.
#define TEST_UNITS_SRC TESTS_FOLDER "test_units.cpp"
static const char *tests[] = {
    TEST_UNITS_SRC,
};

// Unlike nob_file_exists() (POSIX access(), which follows symlinks), this reports true for a
// symlink whose target is missing too - access() alone can't see it, which would leave a
// dangling symlink (e.g. a Linux-only libGLV.so.1 synced via Dropbox to a macOS checkout)
// undeletable and silently blocking its parent directory's removal. Deliberately not
// nob_get_file_type(): that logs an [ERROR] on a genuinely-missing path, which
// delete_if_exists is routinely called against as an expected, silent no-op.
static bool path_exists_or_dangling_symlink(const char *path)
{
#if defined(_WIN32)
    return nob_file_exists(path); // this build never creates such symlinks on Windows
#else
    struct stat st;
    return lstat(path, &st) == 0;
#endif
}

static bool delete_if_exists(const char *path)
{
    if (path_exists_or_dangling_symlink(path)) return nob_delete_file(path);
    return true;
}

// Deletes every path in objects - used once a set of intermediate .o files
// has been folded into a final archive/shared library and is no longer
// needed, so they don't linger on disk as stale build output.
static bool delete_objects(Nob_File_Paths *objects)
{
    bool ok = true;
    for (size_t i = 0; i < objects->count; ++i) {
        ok = delete_if_exists(objects->items[i]) && ok;
    }
    return ok;
}

// Deletes every regular file directly inside folder (not recursive), so a
// stale build output left over from a since-renamed/removed source doesn't
// block clean() from removing the folder itself.
static bool clear_directory(const char *folder)
{
    if (!nob_file_exists(folder)) return true;

    Nob_File_Paths children = {0};
    if (!nob_read_entire_dir(folder, &children)) return false;

    bool ok = true;
    for (size_t i = 0; i < children.count; ++i) {
        const char *name = children.items[i];
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;
        ok = delete_if_exists(nob_temp_sprintf("%s%s", folder, name)) && ok;
    }
    return ok;
}

static bool remove_folder(const char *folder)
{
    return clear_directory(folder) && delete_if_exists(folder);
}

static bool build_needed(const char *output, const char **inputs, size_t inputs_count)
{
    int result = nob_needs_rebuild(output, inputs, inputs_count);
    if (result < 0) exit(1);
    return result > 0;
}

static bool collect_regular_file(Nob_Walk_Entry entry)
{
    Nob_File_Paths *paths = (Nob_File_Paths *)entry.data;

    if (entry.type == NOB_FILE_REGULAR) {
        nob_da_append(paths, nob_temp_strdup(entry.path));
    }

    return true;
}

static bool collect_tree_files(Nob_File_Paths *paths, const char *root)
{
    return nob_walk_dir(root, collect_regular_file, .data = paths);
}

static void add_common_build_deps(Nob_File_Paths *paths, const char *nob_exe)
{
    nob_da_append(paths, "nob.c");
    nob_da_append(paths, nob_exe);
    nob_da_append(paths, NOB_HEADER);
}

static const char *object_path(const char *folder, const char *source)
{
    char *base = nob_temp_strdup(nob_path_name(source));
    char *dot = strrchr(base, '.');
    if (dot) *dot = '\0';
    return nob_temp_sprintf("%s%s.o", folder, base);
}

static const char *executable_path(const char *folder, const char *source)
{
    char *base = nob_temp_strdup(nob_path_name(source));
    char *dot = strrchr(base, '.');
    if (dot) *dot = '\0';
    return nob_temp_sprintf("%s%s" EXE_EXT, folder, base);
}

// The vendored FreeGLUT's X11 backend needs the GL and X11 development
// headers (GLV itself loads OpenGL through the vendored GLAD), so check for
// them before compiling anything rather than fail deep inside a compile.
// A development header the build needs, and the Debian/Ubuntu package that
// provides it. Only meaningful on Linux: macOS and Windows get their system
// headers from the SDK.
#if !defined(_WIN32) && !defined(__APPLE__)
typedef struct { const char *header; const char *pkg; } DevHeader;

// Reports every missing header at once (rather than letting the compiler
// stop at the first one) and names the single apt command that installs the
// whole set.
static bool check_dev_headers(const DevHeader *required, size_t count, const char *apt_packages)
{
    bool ok = true;
    for (size_t i = 0; i < count; ++i) {
        if (!nob_file_exists(required[i].header)) {
            nob_log(NOB_ERROR, "Missing development header: %s (package: %s)",
                    required[i].header, required[i].pkg);
            ok = false;
        }
    }
    if (!ok) {
        nob_log(NOB_ERROR, "On Ubuntu/Debian install all of them with:");
        nob_log(NOB_ERROR, "    sudo apt update && sudo apt install %s", apt_packages);
    }
    return ok;
}
#endif

static bool check_linux_deps(void)
{
#if !defined(_WIN32) && !defined(__APPLE__)
    static const DevHeader required[] = {
        { "/usr/include/GL/gl.h",                    "libgl-dev" },
        { "/usr/include/X11/Xlib.h",                 "libx11-dev" },
        { "/usr/include/X11/extensions/Xrandr.h",    "libxrandr-dev" },
        { "/usr/include/X11/extensions/XInput2.h",   "libxi-dev" },
        { "/usr/include/X11/extensions/xf86vmode.h", "libxxf86vm-dev" },
    };
    return check_dev_headers(required, NOB_ARRAY_LEN(required),
                             "libgl-dev libx11-dev libxrandr-dev libxi-dev libxxf86vm-dev");
#else
    return true;
#endif
}

// Compiles the vendored FreeGLUT sources into FREEGLUT_BUILD_FOLDER/*.o and
// archives them into FREEGLUT_LIB (same flags as AntTweakBar-Legacy's
// build_freeglut(), plus -fPIC so the objects can also go into the shared
// libGLV). A single unity build was deliberately not used: FreeGLUT's
// upstream build systems compile every source as a separate translation
// unit and don't document unity-build safety. Unlike the library objects,
// these are kept for incremental rebuilds (they never change unless the
// vendored source does); `./nob -clean` removes them.
static bool build_freeglut(const char *nob_exe)
{
    if (!nob_mkdir_if_not_exists(FREEGLUT_BUILD_FOLDER)) return false;

    Nob_File_Paths sources = {0};
    for (size_t i = 0; i < NOB_ARRAY_LEN(freeglut_core_sources); ++i) {
        nob_da_append(&sources, freeglut_core_sources[i]);
    }
    for (size_t i = 0; i < NOB_ARRAY_LEN(freeglut_platform_sources); ++i) {
        nob_da_append(&sources, freeglut_platform_sources[i]);
    }

    Nob_File_Paths objects = {0};
    for (size_t i = 0; i < sources.count; ++i) {
        const char *source = sources.items[i];
        const char *output = object_path(FREEGLUT_BUILD_FOLDER, source);
        const char *inputs[] = { source, "nob.c", nob_exe, NOB_HEADER };

        if (build_needed(output, inputs, NOB_ARRAY_LEN(inputs))) {
            Nob_Cmd cmd = {0};
            nob_cmd_append(&cmd, "cc", "-O2", "-fPIC", "-DHAVE_CONFIG_H", "-Wno-deprecated-declarations",
                           "-I" FREEGLUT_CONFIG_INCLUDE, "-I" FREEGLUT_SRC_FOLDER, "-I" FREEGLUT_INCLUDE);
            // FreeGLUT's own sources call no GLU function; skip
            // freeglut_std.h's unconditional GL/glu.h include (a separate dev
            // package on many distros) and provide GL/gl.h instead.
            nob_cmd_append(&cmd, "-DFREEGLUT_NO_GL_INCLUDE");
#if defined(__APPLE__)
            nob_cmd_append(&cmd, "-DNDEBUG", "-DTARGET_HOST_MACOS_COCOA=1", "-include", "OpenGL/gl.h");
#else
            nob_cmd_append(&cmd, "-include", "GL/gl.h");
#endif
#if defined(_WIN32)
            nob_cmd_append(&cmd, "-DFREEGLUT_STATIC");
#endif
            if (nob_sv_ends_with_cstr(nob_sv_from_cstr(source), ".m")) {
                nob_cmd_append(&cmd, "-x", "objective-c");
            }
            nob_cmd_append(&cmd, "-c", source, "-o", output);
            if (!nob_cmd_run(&cmd)) return false;
        } else {
            nob_log(NOB_INFO, "%s is up to date", output);
        }
        nob_da_append(&objects, output);
    }

    Nob_File_Paths lib_inputs = {0};
    for (size_t i = 0; i < objects.count; ++i) nob_da_append(&lib_inputs, objects.items[i]);
    add_common_build_deps(&lib_inputs, nob_exe);

    if (!build_needed(FREEGLUT_LIB, lib_inputs.items, lib_inputs.count)) {
        nob_log(NOB_INFO, "%s is up to date", FREEGLUT_LIB);
        return true;
    }

    // Remove first: `ar rcs` only adds/replaces members, so an object
    // dropped from the source lists above would otherwise linger.
    if (!delete_if_exists(FREEGLUT_LIB)) return false;
    Nob_Cmd ar = {0};
    nob_cmd_append(&ar, "ar", "rcs", FREEGLUT_LIB);
    for (size_t i = 0; i < objects.count; ++i) nob_cmd_append(&ar, objects.items[i]);
    return nob_cmd_run(&ar);
}

// Compile flags shared by the library objects and every example/test: the
// old Makefile.common defaults (-Werror=return-type; C++14 unless a caller
// needs a newer standard, e.g. C++17 for SFML) plus the vendored GLAD and
// FreeGLUT headers.
static void append_glv_flags(Nob_Cmd *cmd, const char *cxx_std)
{
    nob_cmd_append(cmd, cxx_std, "-Werror=return-type",
                        "-I" INCLUDE_FOLDER, "-I" GLAD_INCLUDE, "-I" FREEGLUT_INCLUDE);
    // glv_conf.h already includes the GL headers before any GLUT header, and
    // GLV calls no GLU function, so freeglut_std.h's own GL/GLU includes are
    // not needed (and GLU would be one more system package on Linux).
    nob_cmd_append(cmd, "-DFREEGLUT_NO_GL_INCLUDE");
    // M_PI and friends are not ISO C/C++, and -std=c++NN defines __STRICT_ANSI__,
    // under which MinGW's <math.h> hides them behind _USE_MATH_DEFINES. glibc and
    // Apple's libc expose them regardless, which is why only MinGW needs this; the
    // macro is simply unknown - and harmless - on the other two platforms. Four
    // examples per backend (chladni, graphicsData, spaceCurve, widgets) use M_PI.
    nob_cmd_append(cmd, "-D_USE_MATH_DEFINES");
#if defined(_WIN32)
    nob_cmd_append(cmd, "-DFREEGLUT_STATIC");
#elif defined(__APPLE__)
    // glv_binding_glut.cpp includes Apple's <GLUT/glut.h> (the vendored
    // FreeGLUT only provides GL/glut.h), whose declarations are all marked
    // deprecated; the implementation linked is still the vendored FreeGLUT,
    // the same combination AntTweakBar-Legacy's GLUT examples use.
    nob_cmd_append(cmd, "-Wno-deprecated-declarations");
#else
    nob_cmd_append(cmd, "-D__LINUX__", "-DLINUX"); // as the old Makefile.common did
#endif
}

// System libraries needed by GLV (GL; GLAD's loader needs -ldl on Linux) and
// by the vendored FreeGLUT (windowing). FREEGLUT_LIB itself is added by the
// caller.
static void append_system_libs(Nob_Cmd *cmd)
{
#if defined(_WIN32)
    nob_cmd_append(cmd, "-lopengl32", "-lgdi32", "-lwinmm", "-luser32");
#elif defined(__APPLE__)
    nob_cmd_append(cmd, "-framework", "Cocoa", "-framework", "OpenGL",
                        "-framework", "IOKit", "-framework", "CoreVideo");
#else
    nob_cmd_append(cmd, "-lGL", "-lX11", "-lXrandr", "-lXi", "-lXxf86vm",
                        "-lpthread", "-ldl", "-lm");
#endif
}

static bool build_object(const char *source, const char *folder, Nob_File_Paths *common_deps)
{
    const char *output = object_path(folder, source);

    Nob_File_Paths inputs = {0};
    nob_da_append(&inputs, source);
    for (size_t i = 0; i < common_deps->count; ++i) {
        nob_da_append(&inputs, common_deps->items[i]);
    }

    if (!build_needed(output, inputs.items, inputs.count)) {
        nob_log(NOB_INFO, "%s is up to date", output);
        return true;
    }

    Nob_Cmd cmd = {0};
    // -O3 -DNDEBUG: the old Makefile.common's default Release configuration.
    // -fPIC unconditionally: needed by the shared object set, harmless for
    // the static one.
    if (strcmp(source, GLAD_SRC) == 0) {
        // Generated, vendored C code: plain C99, no -pedantic (its
        // void*-to-function-pointer casts are standard loader practice).
        nob_cmd_append(&cmd, "cc", "-O3", "-DNDEBUG", "-fPIC", "-std=c99", "-I" GLAD_INCLUDE);
    } else {
        nob_cmd_append(&cmd, "c++", "-O3", "-DNDEBUG", "-fPIC");
        append_glv_flags(&cmd, "-std=c++14");
    }
    nob_cmd_append(&cmd, "-c", source, "-o", output);
    return nob_cmd_run(&cmd);
}

static bool build_static_archive(Nob_File_Paths *objects, const char *nob_exe)
{
    Nob_File_Paths inputs = {0};
    for (size_t i = 0; i < objects->count; ++i) nob_da_append(&inputs, objects->items[i]);
    add_common_build_deps(&inputs, nob_exe);

    if (!build_needed(LIB_STATIC, inputs.items, inputs.count)) {
        nob_log(NOB_INFO, "%s is up to date", LIB_STATIC);
        return true;
    }

    if (!delete_if_exists(LIB_STATIC)) return false;
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "ar", "rcs", LIB_STATIC);
    for (size_t i = 0; i < objects->count; ++i) nob_cmd_append(&cmd, objects->items[i]);
    return nob_cmd_run(&cmd);
}

static void append_shared_link_flags(Nob_Cmd *cmd)
{
#if defined(_WIN32)
    nob_cmd_append(cmd, "-shared", "-o", LIB_SHARED, "-Wl,--out-implib," LIB_IMPORT);
#elif defined(__APPLE__)
    nob_cmd_append(cmd, "-dynamiclib", "-o", LIB_SHARED);
#else
    nob_cmd_append(cmd, "-shared", "-Wl,-soname," LIB_SHARED_SONAME_NAME, "-o", LIB_SHARED);
#endif
}

// The shared library contains the vendored FreeGLUT too (linked in from
// FREEGLUT_LIB), so dynamically linked programs must not link FREEGLUT_LIB a
// second time - that would give them two copies of FreeGLUT's global state.
static bool link_shared_library(Nob_File_Paths *objects, const char *nob_exe)
{
    Nob_File_Paths inputs = {0};
    for (size_t i = 0; i < objects->count; ++i) nob_da_append(&inputs, objects->items[i]);
    nob_da_append(&inputs, FREEGLUT_LIB);
    add_common_build_deps(&inputs, nob_exe);

    if (!build_needed(LIB_SHARED, inputs.items, inputs.count)) {
        nob_log(NOB_INFO, "%s is up to date", LIB_SHARED);
        return true;
    }

    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "c++"); // GLV is C++: let the driver pull in the C++ runtime
    append_shared_link_flags(&cmd);
    for (size_t i = 0; i < objects->count; ++i) nob_cmd_append(&cmd, objects->items[i]);
    // Pull in every FreeGLUT object, not just those libGLV's own objects
    // reference, so the shared library exports the complete GLUT API.
#if defined(__APPLE__)
    nob_cmd_append(&cmd, "-Wl,-force_load," FREEGLUT_LIB);
#else
    nob_cmd_append(&cmd, "-Wl,--whole-archive", FREEGLUT_LIB, "-Wl,--no-whole-archive");
#endif
    append_system_libs(&cmd);
    if (!nob_cmd_run(&cmd)) return false;

#if !defined(_WIN32) && !defined(__APPLE__)
    if (!delete_if_exists(LIB_SHARED_SONAME)) return false;
    Nob_Cmd ln = {0};
    nob_cmd_append(&ln, "ln", "-sf", nob_path_name(LIB_SHARED), LIB_SHARED_SONAME);
    if (!nob_cmd_run(&ln)) return false;
#endif

    return true;
}

static bool copy_public_headers(void)
{
    if (!nob_mkdir_if_not_exists(BUILD_INCLUDE_FOLDER)) return false;
    if (!nob_mkdir_if_not_exists(BUILD_GLV_INCLUDE)) return false;
    for (size_t i = 0; i < NOB_ARRAY_LEN(public_headers); ++i) {
        const char *src = nob_temp_sprintf("%s%s", INCLUDE_FOLDER, public_headers[i]);
        const char *dst = nob_temp_sprintf("%s%s", BUILD_GLV_INCLUDE, public_headers[i]);
        if (!nob_copy_file(src, dst)) return false;
    }
    return true;
}

static bool build_all(const char *nob_exe)
{
    if (!check_linux_deps()) return false;
    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)
        || !nob_mkdir_if_not_exists(BUILD_STATIC_FOLDER)
        || !nob_mkdir_if_not_exists(BUILD_SHARED_FOLDER)
        || !nob_mkdir_if_not_exists(LIB_FOLDER)) return false;

    if (!build_freeglut(nob_exe)) return false;

    Nob_File_Paths common_deps = {0};
    if (!collect_tree_files(&common_deps, SRC_FOLDER)) return false;
    if (!collect_tree_files(&common_deps, INCLUDE_FOLDER)) return false;
    if (!collect_tree_files(&common_deps, FREEGLUT_INCLUDE)) return false;
    if (!collect_tree_files(&common_deps, GLAD_INCLUDE)) return false;
    add_common_build_deps(&common_deps, nob_exe);

    // GLV has no import/export macros, so the two object sets are compiled
    // identically; they are kept separate only so each final library's
    // build_needed() check sees its own inputs.
    Nob_File_Paths static_objects = {0};
    Nob_File_Paths shared_objects = {0};
    for (size_t i = 0; i < NOB_ARRAY_LEN(library_sources); ++i) {
        if (!build_object(library_sources[i], BUILD_STATIC_FOLDER, &common_deps)) return false;
        nob_da_append(&static_objects, object_path(BUILD_STATIC_FOLDER, library_sources[i]));

        if (!build_object(library_sources[i], BUILD_SHARED_FOLDER, &common_deps)) return false;
        nob_da_append(&shared_objects, object_path(BUILD_SHARED_FOLDER, library_sources[i]));
    }

    if (!build_static_archive(&static_objects, nob_exe)) return false;
    if (!link_shared_library(&shared_objects, nob_exe)) return false;

    // Same trade-off as AntTweakBarC99's nob.c: the libraries now contain
    // everything these intermediate objects provided, so remove them rather
    // than leave stale build output behind - at the cost of every `./nob`
    // recompiling the library sources from scratch.
    if (!delete_objects(&static_objects)) return false;
    if (!delete_objects(&shared_objects)) return false;
    if (!remove_folder(BUILD_STATIC_FOLDER)) return false;
    if (!remove_folder(BUILD_SHARED_FOLDER)) return false;

    // Copy (not move - GLV/ stays the real, git-tracked source) the public
    // headers next to the libraries, so build/ is a self-contained lib+include
    // pair for anything linking against it (the old Makefile `install` layout).
    if (!copy_public_headers()) return false;

    nob_log(NOB_INFO, "built %s, %s, %s and %s", LIB_STATIC, FREEGLUT_LIB, LIB_SHARED, BUILD_GLV_INCLUDE);
    return true;
}

static bool check_library_built(bool dynamic)
{
    if (dynamic) {
        if (!nob_file_exists(LIB_SHARED)
#if defined(_WIN32)
            || !nob_file_exists(LIB_IMPORT)
#endif
            ) {
            nob_log(NOB_ERROR, "%s does not exist yet.", LIB_SHARED);
            nob_log(NOB_ERROR, "Run `./nob` first to build the library, then `./nob -examples-<backend> -dynamic`.");
            return false;
        }
    } else if (!nob_file_exists(LIB_STATIC) || !nob_file_exists(FREEGLUT_LIB)) {
        nob_log(NOB_ERROR, "%s or %s does not exist yet.", LIB_STATIC, FREEGLUT_LIB);
        nob_log(NOB_ERROR, "Run `./nob` first to build the library, then `./nob -examples-<backend>` or `./nob -test`.");
        return false;
    }
    return true;
}

// GLFW's X11 backend (built only for the examples) needs a few more X11
// development headers than the library's FreeGLUT build (check_linux_deps).
static bool check_linux_glfw_deps(void)
{
#if !defined(_WIN32) && !defined(__APPLE__)
    static const DevHeader required[] = {
        { "/usr/include/X11/Xcursor/Xcursor.h",     "libxcursor-dev" },
        { "/usr/include/X11/extensions/Xinerama.h", "libxinerama-dev" },
        { "/usr/include/X11/extensions/shape.h",    "libxext-dev" },
    };
    return check_dev_headers(required, NOB_ARRAY_LEN(required),
                             "libxcursor-dev libxinerama-dev libxext-dev");
#else
    return true;
#endif
}

// SDL3's X11 backend (vendor/sdl/config/SDL_build_config_linux.h) compiles
// against every X11 extension it then dlopen()s, so their headers - but not
// their shared libraries - are needed at build time. libxxf86vm-dev from
// check_linux_deps() is not among them; SDL3 does not use XF86VidMode.
static bool check_linux_sdl_deps(void)
{
#if !defined(_WIN32) && !defined(__APPLE__)
    static const DevHeader required[] = {
        { "/usr/include/X11/Xlib.h",                 "libx11-dev" },
        { "/usr/include/X11/Xcursor/Xcursor.h",      "libxcursor-dev" },
        { "/usr/include/X11/extensions/shape.h",     "libxext-dev" },
        { "/usr/include/X11/extensions/Xdbe.h",      "libxext-dev" },
        { "/usr/include/X11/extensions/sync.h",      "libxext-dev" },
        { "/usr/include/X11/extensions/Xfixes.h",    "libxfixes-dev" },
        { "/usr/include/X11/extensions/XInput2.h",   "libxi-dev" },
        { "/usr/include/X11/extensions/Xrandr.h",    "libxrandr-dev" },
        { "/usr/include/X11/extensions/scrnsaver.h", "libxss-dev" },
    };
    return check_dev_headers(required, NOB_ARRAY_LEN(required),
                             "libx11-dev libxcursor-dev libxext-dev libxfixes-dev "
                             "libxi-dev libxrandr-dev libxss-dev");
#else
    return true;
#endif
}

// SFML3's Unix backend links its X11 extensions directly (no dlopen), and
// its JoystickImplUnix.cpp hard-depends on libudev.
static bool check_linux_sfml_deps(void)
{
#if !defined(_WIN32) && !defined(__APPLE__)
    static const DevHeader required[] = {
        { "/usr/include/X11/Xlib.h",               "libx11-dev" },
        { "/usr/include/X11/Xcursor/Xcursor.h",    "libxcursor-dev" },
        { "/usr/include/X11/extensions/Xrandr.h",  "libxrandr-dev" },
        { "/usr/include/X11/extensions/XInput2.h", "libxi-dev" },
        { "/usr/include/libudev.h",                "libudev-dev" },
    };
    return check_dev_headers(required, NOB_ARRAY_LEN(required),
                             "libx11-dev libxcursor-dev libxrandr-dev libxi-dev libudev-dev");
#else
    return true;
#endif
}

// Compiles the vendored GLFW3 unity build (vendor/glfw/glfw_unity.c) into a
// single object, as AntTweakBarC99's build_glfw() does.
static bool build_glfw(const char *nob_exe)
{
    if (!check_linux_glfw_deps()) return false;

    const char *inputs[] = { GLFW_SRC, "nob.c", nob_exe, NOB_HEADER };
    if (!build_needed(GLFW_OBJ, inputs, NOB_ARRAY_LEN(inputs))) {
        nob_log(NOB_INFO, "%s is up to date", GLFW_OBJ);
        return true;
    }

    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "cc", "-O2", "-I" GLFW_INCLUDE);
#if defined(_WIN32)
    nob_cmd_append(&cmd, "-D_GLFW_WIN32");
#elif defined(__APPLE__)
    // glfw_unity.c #includes Objective-C (.m) sources under _GLFW_COCOA.
    nob_cmd_append(&cmd, "-D_GLFW_COCOA", "-x", "objective-c");
#else
    nob_cmd_append(&cmd, "-D_GLFW_X11");
#endif
    nob_cmd_append(&cmd, "-c", GLFW_SRC, "-o", GLFW_OBJ);
    return nob_cmd_run(&cmd);
}

static void append_glfw_libs(Nob_Cmd *cmd)
{
    nob_cmd_append(cmd, GLFW_OBJ);
#if defined(_WIN32)
    nob_cmd_append(cmd, "-lopengl32", "-lgdi32");
#elif defined(__APPLE__)
    nob_cmd_append(cmd, "-framework", "Cocoa", "-framework", "IOKit", "-framework", "CoreVideo",
                        "-framework", "OpenGL");
#else
    // GLFW dlopen()s its X11 extension libraries itself at runtime; only
    // core Xlib and libdl are hard link-time dependencies.
    nob_cmd_append(cmd, "-lGL", "-lX11", "-ldl", "-lpthread");
#endif
}

static bool is_cpp_source(const char *source)
{
    return nob_sv_ends_with_cstr(nob_sv_from_cstr(source), ".cpp");
}

// Compiles one vendored SDL3 source file to its own object.
// -DANTTWEAKBARC99_SDL_VENDORED trips the one deliberate edit to the vendored
// code (vendor/sdl/src/dynapi/SDL_dynapi.h) that disables SDL's dynamic-API
// jump table; the macro keeps the name it has in AntTweakBarC99, where the
// vendored tree comes from.
static bool build_sdl_object(const char *source, Nob_File_Paths *common_deps)
{
    const char *output = object_path(SDL_OBJ_FOLDER, source);

    Nob_File_Paths inputs = {0};
    nob_da_append(&inputs, source);
    for (size_t i = 0; i < common_deps->count; ++i) {
        nob_da_append(&inputs, common_deps->items[i]);
    }

    if (!build_needed(output, inputs.items, inputs.count)) {
        nob_log(NOB_INFO, "%s is up to date", output);
        return true;
    }

    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, is_cpp_source(source) ? "c++" : "cc");
    // SDL3's Cocoa backend files are Objective-C and need ARC.
    if (nob_sv_ends_with_cstr(nob_sv_from_cstr(source), ".m")) nob_cmd_append(&cmd, "-fobjc-arc");
    nob_cmd_append(&cmd, "-O2", "-Wno-deprecated-declarations", "-DANTTWEAKBARC99_SDL_VENDORED",
                        "-I" SDL_CONFIG_INCLUDE, "-I" SDL_INCLUDE, "-I" SDL_SRC_ROOT);
    nob_cmd_append(&cmd, "-c", source, "-o", output);
    return nob_cmd_run(&cmd);
}

// Builds every SDL3 source (plus AntTweakBarC99's small sdl_stubs.c) and
// archives them into SDL_LIB. Unlike glfw.o/sfml.o these are kept after the
// build: recompiling ~140 files on every run would be far too slow.
// `./nob -clean` removes them.
static bool build_sdl(const char *nob_exe)
{
    if (!check_linux_sdl_deps()) return false;

    if (!nob_mkdir_if_not_exists(SDL_OBJ_FOLDER)) return false;

    Nob_File_Paths common_deps = {0};
    if (!collect_tree_files(&common_deps, SDL_CONFIG_INCLUDE)) return false;
    add_common_build_deps(&common_deps, nob_exe);

    Nob_File_Paths objects = {0};
    for (size_t i = 0; i < NOB_ARRAY_LEN(sdl_sources_common); ++i) {
        const char *source = nob_temp_sprintf("%s%s", SDL_SRC_ROOT, sdl_sources_common[i]);
        if (!build_sdl_object(source, &common_deps)) return false;
        nob_da_append(&objects, object_path(SDL_OBJ_FOLDER, source));
    }
#if defined(_WIN32)
    const char **sdl_sources_platform = sdl_sources_win32;
    size_t sdl_sources_platform_count = NOB_ARRAY_LEN(sdl_sources_win32);
#elif defined(__APPLE__)
    const char **sdl_sources_platform = sdl_sources_macos;
    size_t sdl_sources_platform_count = NOB_ARRAY_LEN(sdl_sources_macos);
#else
    const char **sdl_sources_platform = sdl_sources_linux;
    size_t sdl_sources_platform_count = NOB_ARRAY_LEN(sdl_sources_linux);
#endif
    for (size_t i = 0; i < sdl_sources_platform_count; ++i) {
        const char *source = nob_temp_sprintf("%s%s", SDL_SRC_ROOT, sdl_sources_platform[i]);
        if (!build_sdl_object(source, &common_deps)) return false;
        nob_da_append(&objects, object_path(SDL_OBJ_FOLDER, source));
    }
    if (!build_sdl_object(SDL_STUB_SRC, &common_deps)) return false;
    nob_da_append(&objects, object_path(SDL_OBJ_FOLDER, SDL_STUB_SRC));

    Nob_File_Paths archive_inputs = {0};
    for (size_t i = 0; i < objects.count; ++i) nob_da_append(&archive_inputs, objects.items[i]);
    add_common_build_deps(&archive_inputs, nob_exe);

    if (!build_needed(SDL_LIB, archive_inputs.items, archive_inputs.count)) {
        nob_log(NOB_INFO, "%s is up to date", SDL_LIB);
        return true;
    }

    if (!delete_if_exists(SDL_LIB)) return false;
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "ar", "rcs", SDL_LIB);
    for (size_t i = 0; i < objects.count; ++i) nob_cmd_append(&cmd, objects.items[i]);
    return nob_cmd_run(&cmd);
}

static void append_sdl_libs(Nob_Cmd *cmd)
{
    nob_cmd_append(cmd, SDL_LIB);
#if defined(_WIN32)
    nob_cmd_append(cmd, "-luser32", "-lgdi32", "-lopengl32", "-limm32",
                        "-lole32", "-loleaut32", "-luuid", "-lwinmm", "-lsetupapi", "-lversion");
#elif defined(__APPLE__)
    nob_cmd_append(cmd, "-framework", "Cocoa", "-framework", "IOKit", "-framework", "CoreVideo",
                        "-framework", "Carbon", "-framework", "OpenGL", "-framework", "CoreFoundation",
                        "-framework", "UniformTypeIdentifiers");
#else
    // Every X11 extension is dlopen()ed by SDL_x11dyn.c (see
    // vendor/sdl/config/SDL_build_config_linux.h), so core Xlib is the only
    // X link-time dependency.
    nob_cmd_append(cmd, "-lX11", "-lGL", "-lpthread", "-ldl", "-lm");
#endif
}

// Compiles the vendored SFML3 unity build into a single object, as
// AntTweakBarC99's build_sfml() does (C++17, as SFML requires).
static bool build_sfml(const char *nob_exe)
{
    if (!check_linux_sfml_deps()) return false;

    const char *inputs[] = { SFML_SRC, "nob.c", nob_exe, NOB_HEADER };
    if (!build_needed(SFML_OBJ, inputs, NOB_ARRAY_LEN(inputs))) {
        nob_log(NOB_INFO, "%s is up to date", SFML_OBJ);
        return true;
    }

    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "c++", "-std=c++17", "-DSFML_STATIC",
                        "-I" SFML_INCLUDE, "-Ivendor/sfml/src",
                        "-Ivendor/sfml/extlibs/headers/cpp-unicodelib",
                        "-Ivendor/sfml/extlibs/headers/glad/include",
                        "-Ivendor/sfml/extlibs/headers/vulkan");
    nob_cmd_append(&cmd, "-c", SFML_SRC, "-o", SFML_OBJ);
    return nob_cmd_run(&cmd);
}

static void append_sfml_libs(Nob_Cmd *cmd)
{
    nob_cmd_append(cmd, SFML_OBJ);
#if defined(_WIN32)
    nob_cmd_append(cmd, "-lopengl32", "-lgdi32", "-luser32", "-lwinmm", "-lole32");
#elif defined(__APPLE__)
    nob_cmd_append(cmd, "-framework", "Foundation", "-framework", "AppKit",
                        "-framework", "IOKit", "-framework", "Carbon", "-framework", "OpenGL");
#else
    // SFML's Unix backend links its X11 extensions directly (no dlopen) and
    // enumerates joysticks through libudev.
    nob_cmd_append(cmd, "-lGL", "-lX11", "-lXrandr", "-lXcursor", "-lXi", "-ludev",
                        "-lpthread", "-ldl");
#endif
}

// One of eight folders (EXAMPLES_{STATIC,SHARED}_{GLUT,GLFW,SDL,SFML}_FOLDER).
static const char *example_output_folder(bool dynamic, Backend backend)
{
    switch (backend) {
    case BACKEND_GLFW: return dynamic ? EXAMPLES_SHARED_GLFW_FOLDER : EXAMPLES_STATIC_GLFW_FOLDER;
    case BACKEND_SDL:  return dynamic ? EXAMPLES_SHARED_SDL_FOLDER  : EXAMPLES_STATIC_SDL_FOLDER;
    case BACKEND_SFML: return dynamic ? EXAMPLES_SHARED_SFML_FOLDER : EXAMPLES_STATIC_SFML_FOLDER;
    case BACKEND_GLUT: default:
        return dynamic ? EXAMPLES_SHARED_GLUT_FOLDER : EXAMPLES_STATIC_GLUT_FOLDER;
    }
}

// The per-folder helper header every example of a backend includes.
static const char *backend_helper_header(Backend backend)
{
    switch (backend) {
    case BACKEND_GLFW: return EXAMPLES_GLFW_FOLDER "glv_glfw.h";
    case BACKEND_SDL:  return EXAMPLES_SDL_FOLDER "glv_sdl.h";
    case BACKEND_SFML: return EXAMPLES_SFML_FOLDER "glv_sfml.h";
    case BACKEND_GLUT: default: return EXAMPLES_GLUT_FOLDER "example.h";
    }
}

// Compiles and links one example (or test, with BACKEND_GLUT) against the
// static library (libGLV.a, plus libfreeglut.a for GLUT) or, with dynamic,
// against the shared library, which already contains FreeGLUT. GLFW/SDL/SFML
// examples never reference GLV's Window/Application classes, so a static link
// pulls no GLUT binding objects out of libGLV.a and needs no FreeGLUT.
static bool build_example(const char *source, const char *folder, Backend backend, bool dynamic,
                          const char *nob_exe)
{
    const char *output = executable_path(folder, source);

    Nob_File_Paths inputs = {0};
    nob_da_append(&inputs, source);
    if (!collect_tree_files(&inputs, INCLUDE_FOLDER)) return false;
    nob_da_append(&inputs, backend_helper_header(backend));
#if defined(_WIN32)
    nob_da_append(&inputs, dynamic ? LIB_IMPORT : LIB_STATIC);
#else
    nob_da_append(&inputs, dynamic ? LIB_SHARED : LIB_STATIC);
#endif
    switch (backend) {
    case BACKEND_GLUT: if (!dynamic) nob_da_append(&inputs, FREEGLUT_LIB); break;
    case BACKEND_GLFW: nob_da_append(&inputs, GLFW_OBJ); break;
    case BACKEND_SDL:  nob_da_append(&inputs, SDL_LIB);  break;
    case BACKEND_SFML: nob_da_append(&inputs, SFML_OBJ); break;
    }
    add_common_build_deps(&inputs, nob_exe);

    if (!build_needed(output, inputs.items, inputs.count)) {
        nob_log(NOB_INFO, "%s is up to date", output);
        return true;
    }

    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "c++", "-O2");
    append_glv_flags(&cmd, backend == BACKEND_SFML ? "-std=c++17" : "-std=c++14");
    switch (backend) {
    case BACKEND_GLUT: break;
    case BACKEND_GLFW: nob_cmd_append(&cmd, "-I" GLFW_INCLUDE); break;
    case BACKEND_SDL:  nob_cmd_append(&cmd, "-I" SDL_INCLUDE); break;
    // -DSFML_STATIC: without it SFML's Export.hpp marks every sf:: symbol
    // dllimport on Windows, which doesn't match the static sfml.o.
    case BACKEND_SFML: nob_cmd_append(&cmd, "-DSFML_STATIC", "-I" SFML_INCLUDE); break;
    }
    nob_cmd_append(&cmd, source, "-o", output);
    if (dynamic) {
#if defined(_WIN32)
        nob_cmd_append(&cmd, LIB_IMPORT);
#else
        nob_cmd_append(&cmd, LIB_SHARED);
#endif
    } else {
        nob_cmd_append(&cmd, LIB_STATIC);
        if (backend == BACKEND_GLUT) nob_cmd_append(&cmd, FREEGLUT_LIB);
    }
    switch (backend) {
    case BACKEND_GLUT: break;
    case BACKEND_GLFW: append_glfw_libs(&cmd); break;
    case BACKEND_SDL:  append_sdl_libs(&cmd);  break;
    case BACKEND_SFML: append_sfml_libs(&cmd); break;
    }
    append_system_libs(&cmd);
    return nob_cmd_run(&cmd);
}

// Printed after a successful `-dynamic` examples build: these executables
// need the shared library to be locatable at *run* time, not just link time,
// which is easy to miss since the build itself succeeds either way.
static void print_dynamic_runtime_notice(void)
{
#if defined(_WIN32)
    nob_log(NOB_INFO, "-dynamic executables need %s next to the .exe, or on PATH.", LIB_SHARED);
#elif defined(__APPLE__)
    nob_log(NOB_INFO, "-dynamic executables find %s relative to the current directory:", LIB_SHARED);
    nob_log(NOB_INFO, "run them from the repository root, or set DYLD_LIBRARY_PATH.");
#else
    nob_log(NOB_INFO, "-dynamic executables need %s to be locatable at runtime", LIB_SHARED);
    nob_log(NOB_INFO, "(e.g. LD_LIBRARY_PATH=%s, or installed to a standard library path).", LIB_FOLDER);
#endif
}

static bool build_examples(const char *nob_exe, bool dynamic, Backend backend)
{
    if (!check_linux_deps()) return false;
    if (!check_library_built(dynamic)) return false;
    const char *folder = example_output_folder(dynamic, backend);
    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return false;
    if (!nob_mkdir_if_not_exists(EXAMPLES_BUILD_FOLDER)) return false;
    if (!nob_mkdir_if_not_exists(folder)) return false;

    const char **sources;
    size_t count;
    switch (backend) {
    case BACKEND_GLFW:
        if (!build_glfw(nob_exe)) return false;
        sources = glfw_examples; count = NOB_ARRAY_LEN(glfw_examples);
        break;
    case BACKEND_SDL:
        if (!build_sdl(nob_exe)) return false;
        sources = sdl_examples; count = NOB_ARRAY_LEN(sdl_examples);
        break;
    case BACKEND_SFML:
        if (!build_sfml(nob_exe)) return false;
        sources = sfml_examples; count = NOB_ARRAY_LEN(sfml_examples);
        break;
    case BACKEND_GLUT: default:
        sources = glut_examples; count = NOB_ARRAY_LEN(glut_examples);
        break;
    }

    for (size_t i = 0; i < count; ++i) {
        if (!build_example(sources[i], folder, backend, dynamic, nob_exe)) return false;
    }

    // glfw.o/sfml.o are cheap single unity objects, only needed while linking
    // the examples above - remove them rather than leave stale leftovers (as
    // in AntTweakBarC99). SDL_LIB is deliberately kept (see build_sdl()).
    if (backend == BACKEND_GLFW && !delete_if_exists(GLFW_OBJ)) return false;
    if (backend == BACKEND_SFML && !delete_if_exists(SFML_OBJ)) return false;

    nob_log(NOB_INFO, "built %zu %s examples into %s (%s)", count, backend_name(backend), folder,
            dynamic ? "dynamically linked" : "statically linked");
    if (dynamic) print_dynamic_runtime_notice();
    return true;
}

static bool build_tests(const char *nob_exe)
{
    if (!check_linux_deps()) return false;
    if (!check_library_built(false)) return false;
    if (!nob_mkdir_if_not_exists(TESTS_BUILD_FOLDER)) return false;

    for (size_t i = 0; i < NOB_ARRAY_LEN(tests); ++i) {
        if (!build_example(tests[i], TESTS_BUILD_FOLDER, BACKEND_GLUT, false, nob_exe)) return false;
    }

    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, executable_path(TESTS_BUILD_FOLDER, TEST_UNITS_SRC));
    return nob_cmd_run(&cmd);
}

// Deletes path and, if it is a directory, everything below it. Symbolic links
// are deleted, never followed (lstat(), not nob_get_file_type()'s stat()), so
// nothing outside the tree can be touched.
static bool delete_tree(const char *path)
{
    if (!path_exists_or_dangling_symlink(path)) return true;
#if defined(_WIN32)
    bool is_dir = nob_get_file_type(path) == NOB_FILE_DIRECTORY;
#else
    struct stat st;
    if (lstat(path, &st) != 0) return false;
    bool is_dir = S_ISDIR(st.st_mode);
#endif
    if (is_dir) {
        Nob_File_Paths children = {0};
        if (!nob_read_entire_dir(path, &children)) return false;
        size_t len = strlen(path);
        const char *sep = (len > 0 && path[len - 1] == '/') ? "" : "/";
        bool ok = true;
        for (size_t i = 0; i < children.count; ++i) {
            const char *name = children.items[i];
            if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;
            ok = delete_tree(nob_temp_sprintf("%s%s%s", path, sep, name)) && ok;
        }
        nob_da_free(children);
        if (!ok) return false;
    }
    return nob_delete_file(path);
}

// Removes the whole build folder, including anything left over from an
// earlier output layout (e.g. build/examples/static/ from before the
// per-backend folders): every build output lives under BUILD_FOLDER.
static bool clean(void)
{
    return delete_tree(BUILD_FOLDER);
}

static void usage(const char *program)
{
    printf("usage: %s [-examples-glut | -examples-glfw | -examples-sdl | -examples-sfml]\n", program);
    printf("             [-dynamic] [-test] [-clean] [-help]\n");
    printf("  (no flags)      build the library: build/lib/libGLV.a, build/lib/libfreeglut.a,\n");
    printf("                  the shared build/lib/libGLV.{dll,so,dylib} and build/include/GLV/;\n");
    printf("                  this one build serves every -examples-* flag below\n");
    printf("  -examples-glut  build examples/glut/ into build/examples/static-glut/\n");
    printf("                  (requires the library to already be built with ./nob)\n");
    printf("  -examples-glfw  same, for the GLFW3 examples in examples/glfw/\n");
    printf("  -examples-sdl   same, for the SDL3 examples in examples/sdl/\n");
    printf("  -examples-sfml  same, for the SFML3 examples in examples/sfml/\n");
    printf("  -dynamic        with any -examples-* flag, link against the shared library and\n");
    printf("                  put the executables into build/examples/shared-<backend>/\n");
    printf("  -test           build test/test_units.cpp into build/tests/ and run it\n");
    printf("  -clean          remove generated build files and exit\n");
    printf("  -help           print this help and exit\n");
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF_PLUS(argc, argv, NOB_HEADER);

    const char *nob_exe = argv[0];
    bool clean_requested = false;
    bool dynamic_requested = false;
    bool tests_requested = false;
    int examples_requested = 0;
    Backend backend = BACKEND_GLUT;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-clean") == 0) {
            clean_requested = true;
        } else if (strcmp(argv[i], "-examples-glut") == 0) {
            backend = BACKEND_GLUT; ++examples_requested;
        } else if (strcmp(argv[i], "-examples-glfw") == 0) {
            backend = BACKEND_GLFW; ++examples_requested;
        } else if (strcmp(argv[i], "-examples-sdl") == 0) {
            backend = BACKEND_SDL; ++examples_requested;
        } else if (strcmp(argv[i], "-examples-sfml") == 0) {
            backend = BACKEND_SFML; ++examples_requested;
        } else if (strcmp(argv[i], "-dynamic") == 0) {
            dynamic_requested = true;
        } else if (strcmp(argv[i], "-test") == 0) {
            tests_requested = true;
        } else if (strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "--help") == 0) {
            usage(argv[0]);
            return 0;
        } else {
            nob_log(NOB_ERROR, "unknown argument: %s", argv[i]);
            usage(argv[0]);
            return 1;
        }
    }

    if (examples_requested > 1) {
        nob_log(NOB_ERROR, "-examples-glut, -examples-glfw, -examples-sdl and -examples-sfml are mutually exclusive");
        return 1;
    }
    if (tests_requested && (examples_requested || dynamic_requested || clean_requested)) {
        nob_log(NOB_ERROR, "-test cannot be combined with -examples-*, -dynamic or -clean");
        return 1;
    }
    if (dynamic_requested && !examples_requested) {
        nob_log(NOB_WARNING, "-dynamic has no effect without an -examples-* flag");
    }

    if (clean_requested) return clean() ? 0 : 1;
    if (tests_requested) return build_tests(nob_exe) ? 0 : 1;
    if (examples_requested) return build_examples(nob_exe, dynamic_requested, backend) ? 0 : 1;
    return build_all(nob_exe) ? 0 : 1;
}
