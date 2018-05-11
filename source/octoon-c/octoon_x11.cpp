#ifdef OCTOON_TARGET_PLATFORM_EMSCRIPTEN
#include <octoon/octoon.h>

#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>

#undef None

#include <octoon/game_app.h>
#include <octoon/input/input_event.h>
#include <octoon/runtime/except.h>
#include <octoon/io/fcntl.h>

#include <chrono>
#include <iostream>
#include <cstring>


std::string gameScenePath_;
// X11 related local variables
static Display *x_display = NULL;
octoon::GameApp* gameApp_;

bool OCTOON_CALL OctoonOpenWindow(const char* title, int w, int h) noexcept
{
    Window root;
    XSetWindowAttributes swa;
    XSetWindowAttributes  xattr;
    Atom wm_state;
    XWMHints hints;
    XEvent xev;
    Window win;

    /*
     * X11 native display initialization
     */

    x_display = XOpenDisplay(NULL);
    if ( x_display == NULL )
    {
        return false;
    }

    root = DefaultRootWindow(x_display);

    swa.event_mask  =  ExposureMask | PointerMotionMask | KeyPressMask;
    win = XCreateWindow(
               x_display, root,
               0, 0, w, h, 0,
               CopyFromParent, InputOutput,
               CopyFromParent, CWEventMask,
               &swa );

    xattr.override_redirect = false;
    XChangeWindowAttributes ( x_display, win, CWOverrideRedirect, &xattr );

    hints.input = true;
    hints.flags = InputHint;
    XSetWMHints(x_display, win, &hints);

    // make the window visible on the screen
    XMapWindow (x_display, win);
    XStoreName (x_display, win, title);

    // get identifiers for the provided atom name strings
    wm_state = XInternAtom (x_display, "_NET_WM_STATE", false);

    memset ( &xev, 0, sizeof(xev) );
    xev.type                 = ClientMessage;
    xev.xclient.window       = win;
    xev.xclient.message_type = wm_state;
    xev.xclient.format       = 32;
    xev.xclient.data.l[0]    = 1;
    xev.xclient.data.l[1]    = false;
    XSendEvent (
       x_display,
       DefaultRootWindow ( x_display ),
       false,
       SubstructureNotifyMask,
       &xev );

    // build gameapp
    octoon::WindHandle hwnd = (octoon::WindHandle)win;

    gameApp_ = octoon::GameApp::instance();
    gameApp_->open(hwnd, w, h, w, h);
    gameApp_->setActive(true);
    gameApp_->doWindowFocus(hwnd, true);
    gameApp_->doWindowResize(hwnd, w, h);
    gameApp_->doWindowFramebufferResize(hwnd, w, h);

    if (!gameScenePath_.empty())
    {
        if (!gameApp_->openScene(gameScenePath_))
            throw octoon::runtime::runtime_error::create("GameApplication::open_scene() failed");
    }

    return true;
}


bool OCTOON_CALL OctoonInit(const char* gamedir, const char* scenename) noexcept
{
    if (scenename)
		gameScenePath_ = scenename;
}

void OCTOON_CALL OctoonCloseWindow() noexcept
{
	if (gameApp_)
	{
		gameApp_->close();
		gameApp_ = nullptr;
	}

	//::glfwTerminate();
}

bool OCTOON_CALL OctoonIsQuitRequest() noexcept
{
	if (!gameApp_)
		return true;

	//if (glfwWindowShouldClose(window_) || gameApp_->isQuitRequest())
	//	return true;

	return false;
}

void OCTOON_CALL OctoonUpdate() noexcept
{
	//::glfwPollEvents();

	if (gameApp_)
		gameApp_->update();
}

void OCTOON_CALL OctoonTerminate() noexcept
{
	OctoonCloseWindow();
}

#endif