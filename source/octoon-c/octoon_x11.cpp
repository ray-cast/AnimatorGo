#include <octoon/octoon-c.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#undef None

#ifdef OCTOON_BUILD_PLATFORM_EMSCRIPTEN
#   include <emscripten.h>
#   include <emscripten/html5.h>
#   include <emscripten/bind.h>
#endif

#include <octoon/game_app.h>
#include <octoon/input/input_event.h>
#include <octoon/runtime/except.h>
#include <octoon/io/fcntl.h>

#include <chrono>
#include <iostream>
#include <cstring>
#include <cstdlib>

#ifdef OCTOON_BUILD_PLATFORM_EMSCRIPTEN
#   include <emscripten.h>
#endif

std::string gameScenePath_;
// X11 related local variables
static Display *x_display = NULL;
// win hwnd
octoon::WindHandle hwnd = 0;

octoon::GameApp* gameApp_;
bool should_close_ = false;

octoon::input::InputKey::Code KeyCodetoInputKey(const char* key) noexcept
{
    if(strcmp(key, "Unidentified") == 0) return octoon::input::InputKey::Code::None;
    else if (strcmp(key, "Backspace") == 0) return octoon::input::InputKey::Code::Space;
    else if (strcmp(key, "Alt") == 0) return octoon::input::InputKey::Code::Space;
    else if (strcmp(key, "F1") == 0) return octoon::input::InputKey::Code::F1;
	else if (strcmp(key, "F2") == 0) return octoon::input::InputKey::Code::F2;
	else if (strcmp(key, "F3") == 0) return octoon::input::InputKey::Code::F3;
	else if (strcmp(key, "F4") == 0) return octoon::input::InputKey::Code::F4;
	else if (strcmp(key, "F5") == 0) return octoon::input::InputKey::Code::F5;
	else if (strcmp(key, "F6") == 0) return octoon::input::InputKey::Code::F6;
	else if (strcmp(key, "F7") == 0) return octoon::input::InputKey::Code::F7;
	else if (strcmp(key, "F8") == 0) return octoon::input::InputKey::Code::F8;
	else if (strcmp(key, "F9") == 0) return octoon::input::InputKey::Code::F9;
	else if (strcmp(key, "F10") == 0) return octoon::input::InputKey::Code::F10;
	else if (strcmp(key, "F11") == 0) return octoon::input::InputKey::Code::F11;
	else if (strcmp(key, "F12") == 0) return octoon::input::InputKey::Code::F12;
	else if (strcmp(key, "F13") == 0) return octoon::input::InputKey::Code::F13;
	else if (strcmp(key, "F14") == 0) return octoon::input::InputKey::Code::F14;
	else if (strcmp(key, "F15") == 0) return octoon::input::InputKey::Code::F15;
	else if (strcmp(key, "F16") == 0) return octoon::input::InputKey::Code::F16;
	else if (strcmp(key, "F17") == 0) return octoon::input::InputKey::Code::F17;
	else if (strcmp(key, "F18") == 0) return octoon::input::InputKey::Code::F18;
	else if (strcmp(key, "F19") == 0) return octoon::input::InputKey::Code::F19;
	else if (strcmp(key, "F20") == 0) return octoon::input::InputKey::Code::F20;
	else if (strcmp(key, "F21") == 0) return octoon::input::InputKey::Code::F21;
	else if (strcmp(key, "F22") == 0) return octoon::input::InputKey::Code::F22;
	else if (strcmp(key, "F23") == 0) return octoon::input::InputKey::Code::F23;
	else if (strcmp(key, "F24") == 0) return octoon::input::InputKey::Code::F24;
	else if (strcmp(key, "F25") == 0) return octoon::input::InputKey::Code::F25;
    else if (strcmp(key, "A") == 0) return octoon::input::InputKey::Code::A;
	else if (strcmp(key, "B") == 0) return octoon::input::InputKey::Code::B;
	else if (strcmp(key, "C") == 0) return octoon::input::InputKey::Code::C;
	else if (strcmp(key, "D") == 0) return octoon::input::InputKey::Code::D;
	else if (strcmp(key, "E") == 0) return octoon::input::InputKey::Code::E;
	else if (strcmp(key, "F") == 0) return octoon::input::InputKey::Code::F;
	else if (strcmp(key, "G") == 0) return octoon::input::InputKey::Code::G;
	else if (strcmp(key, "H") == 0) return octoon::input::InputKey::Code::H;
	else if (strcmp(key, "I") == 0) return octoon::input::InputKey::Code::I;
	else if (strcmp(key, "J") == 0) return octoon::input::InputKey::Code::J;
	else if (strcmp(key, "K") == 0) return octoon::input::InputKey::Code::K;
	else if (strcmp(key, "L") == 0) return octoon::input::InputKey::Code::L;
	else if (strcmp(key, "M") == 0) return octoon::input::InputKey::Code::M;
	else if (strcmp(key, "N") == 0) return octoon::input::InputKey::Code::N;
	else if (strcmp(key, "O") == 0) return octoon::input::InputKey::Code::O;
	else if (strcmp(key, "P") == 0) return octoon::input::InputKey::Code::P;
	else if (strcmp(key, "Q") == 0) return octoon::input::InputKey::Code::Q;
	else if (strcmp(key, "R") == 0) return octoon::input::InputKey::Code::R;
	else if (strcmp(key, "S") == 0) return octoon::input::InputKey::Code::S;
	else if (strcmp(key, "T") == 0) return octoon::input::InputKey::Code::T;
	else if (strcmp(key, "U") == 0) return octoon::input::InputKey::Code::U;
	else if (strcmp(key, "V") == 0) return octoon::input::InputKey::Code::V;
	else if (strcmp(key, "W") == 0) return octoon::input::InputKey::Code::W;
	else if (strcmp(key, "X") == 0) return octoon::input::InputKey::Code::X;
	else if (strcmp(key, "Y") == 0) return octoon::input::InputKey::Code::Y;
	else if (strcmp(key, "Z") == 0) return octoon::input::InputKey::Code::Z;
    else if (strcmp(key, "0") == 0) return octoon::input::InputKey::Code::Key0;
	else if (strcmp(key, "1") == 0) return octoon::input::InputKey::Code::Key1;
	else if (strcmp(key, "2") == 0) return octoon::input::InputKey::Code::Key2;
	else if (strcmp(key, "3") == 0) return octoon::input::InputKey::Code::Key3;
	else if (strcmp(key, "4") == 0) return octoon::input::InputKey::Code::Key4;
	else if (strcmp(key, "5") == 0) return octoon::input::InputKey::Code::Key5;
	else if (strcmp(key, "6") == 0) return octoon::input::InputKey::Code::Key6;
	else if (strcmp(key, "7") == 0) return octoon::input::InputKey::Code::Key7;
	else if (strcmp(key, "8") == 0) return octoon::input::InputKey::Code::Key8;
	else if (strcmp(key, "9") == 0) return octoon::input::InputKey::Code::Key9;
	else return octoon::input::InputKey::Code::None;
}

static bool windowShouldClose()
{
    return should_close_;
}

EM_BOOL onMouseButtonDown(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
    if (gameApp_)
    {
        gameApp_->doWindowMouseButtonDown(hwnd,
        (octoon::input::InputButton::Code)(octoon::input::InputButton::Mouse0 + mouseEvent->button),
        mouseEvent->canvasX,
        mouseEvent->canvasY);
        return true;
    }
    else
    {
        return false;
    }
}

EM_BOOL onMouseButtonUp(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
    if (gameApp_)
    {
        gameApp_->doWindowMouseButtonUp(hwnd,
        (octoon::input::InputButton::Code)(octoon::input::InputButton::Mouse0 + mouseEvent->button),
        mouseEvent->canvasX,
        mouseEvent->canvasY);
        return true;
    }
    else
    {
        return false;
    }
}

EM_BOOL onMouseMove(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
    if (gameApp_)
    {
        gameApp_->doWindowMouseMotion(hwnd, mouseEvent->canvasX, mouseEvent->canvasY);
        return true;
    }
    else
    {
        return false;
    }
}

EM_BOOL onWindowScrool(int eventType, const EmscriptenWheelEvent *wheelEvent, void *userData)
{
    if (gameApp_)
    {
        gameApp_->doWindowScrool(hwnd, wheelEvent->deltaX, wheelEvent->deltaY);
        return true;
    }
	else
    {
        return false;
    }
}

EM_BOOL onWindowKeyDown(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData)
{
    if (gameApp_)
    {
        gameApp_->doWindowKeyDown(hwnd, KeyCodetoInputKey(keyEvent->key), std::atoi(keyEvent->code), 0);
        return true;
    }
    else
    {
        return false;
    }
}

EM_BOOL onWindowKeyUp(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData)
{
    if (gameApp_)
    {
        gameApp_->doWindowKeyUp(hwnd, KeyCodetoInputKey(keyEvent->key), std::atoi(keyEvent->code), 0);
        return true;
    }
    else
    {
        return false;
    }
}

EM_BOOL onWindowKeyPress(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData)
{
    if (gameApp_)
    {
        gameApp_->doWindowKeyPress(hwnd, KeyCodetoInputKey(keyEvent->key), std::atoi(keyEvent->code), 0);
        return true;
    }
    else
    {
        return false;
    }
}

EM_BOOL onWindowResize(int eventType, const EmscriptenUiEvent *uiEvent, void *userData)
{
    if (gameApp_)
    {
        gameApp_->doWindowResize(hwnd, uiEvent->documentBodyClientWidth, uiEvent->documentBodyClientHeight);
        return true;
    }
    else
    {
        return false;
    }
}

bool OCTOON_C_CALL OctoonOpenWindow(const char* title, int w, int h) noexcept
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

    // bind io
    emscripten_set_mousedown_callback("#canvas", 0, false, onMouseButtonDown);
    emscripten_set_mouseup_callback("#canvas", 0, false, onMouseButtonUp);
    emscripten_set_mousemove_callback("#canvas", 0, false, onMouseMove);

    emscripten_set_wheel_callback("#canvas", 0, false, onWindowScrool);

    emscripten_set_keypress_callback("#canvas", 0, false, onWindowKeyPress);
    emscripten_set_keydown_callback("#canvas", 0, false, onWindowKeyDown);
    emscripten_set_keyup_callback("#canvas", 0, false, onWindowKeyUp);

    emscripten_set_resize_callback("#canvas", 0, false, onWindowResize);

    // build gameapp
    hwnd = (octoon::WindHandle)win;

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

bool OCTOON_C_CALL OctoonInit(const char* gamedir, const char* scenename) noexcept
{
    if (scenename)
		gameScenePath_ = scenename;

    return true;
}

void OCTOON_C_CALL OctoonCloseWindow() noexcept
{
	if (gameApp_)
	{
		gameApp_->close();
		gameApp_ = nullptr;
	}
#ifndef OCTOON_BUILD_PLATFORM_EMSCRIPTEN
	XCloseDisplay(x_display);
#endif
}

bool OCTOON_C_CALL OctoonIsQuitRequest() noexcept
{
	if (!gameApp_)
		return true;

	if (windowShouldClose() || gameApp_->isQuitRequest())
		return true;

	return false;
}

void OCTOON_C_CALL OctoonUpdate() noexcept
{
    //_glfwPollJoystickEvents();
#ifndef OCTOON_BUILD_PLATFORM_EMSCRIPTEN
    int count = XPending(x_display);
    while (count--)
    {
        XEvent event;
        XNextEvent(x_display, &event);
        processEvent(&event);
    }

    //if (_glfw.x11.disabledCursorWindow)
    //    centerCursor(_glfw.x11.disabledCursorWindow);

#endif

    if (gameApp_)
		gameApp_->update();
}

void OCTOON_C_CALL OctoonMainLoop() noexcept
{
#ifdef OCTOON_BUILD_PLATFORM_EMSCRIPTEN
	// void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
	emscripten_set_main_loop(OctoonUpdate, 60, 1);
#else
    while (!OctoonIsQuitRequest())
        OctoonUpdate();
#endif
}

void OCTOON_C_CALL OctoonTerminate() noexcept
{
	OctoonCloseWindow();
}
