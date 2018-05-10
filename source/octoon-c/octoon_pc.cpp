#define OCTOON_BUILD_PLATFORM_WINDOWS
#if defined(OCTOON_BUILD_PLATFORM_WINDOWS) || defined(OCTOON_BUILD_PLATFORM_LINUX) || defined(OCTOON_BUILD_PLATFORM_APPLE)
#include <octoon/octoon.h>

#include <octoon/game_app.h>
#include <octoon/input/input_event.h>
#include <octoon/runtime/except.h>
#include <octoon/io/fcntl.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <chrono>
#include <iostream>
#include <cstring>

#if defined(GLFW_EXPOSE_NATIVE_WIN32)
#define glfwGetWinHandle(window) glfwGetWin32Window(window)
#elif defined(GLFW_EXPOSE_NATIVE_X11)
#define glfwGetWinHandle(window) glfwGetX11Window(window)
#elif defined(GLFW_EXPOSE_NATIVE_EGL)
#define glfwGetWinHandle(window) glfwGetEGLSurface(window)
#elif defined(GLFW_EXPOSE_NATIVE_NSGL)
#define glfwGetWinHandle(window) glfwGetCocoaWindow(window)
#endif

#undef None

#ifndef MAX_PATH
#	define MAX_PATH 260
#endif

GLFWwindow* window_ = nullptr;
octoon::GameApp* gameApp_;

std::string gameRootPath_;
std::string gameScenePath_;

octoon::input::InputKey::Code KeyCodetoInputKey(int key) noexcept
{
	switch (key)
	{
	case GLFW_KEY_UNKNOWN: return octoon::input::InputKey::Code::None;
	case GLFW_KEY_SPACE: return octoon::input::InputKey::Code::Space;
	case GLFW_KEY_APOSTROPHE: return octoon::input::InputKey::Code::Apostrophe;
	case GLFW_KEY_COMMA: return octoon::input::InputKey::Code::Comma;
	case GLFW_KEY_MINUS: return octoon::input::InputKey::Code::Minus;
	case GLFW_KEY_PERIOD: return octoon::input::InputKey::Code::Period;
	case GLFW_KEY_SLASH: return octoon::input::InputKey::Code::Slash;
	case GLFW_KEY_0: return octoon::input::InputKey::Code::Key0;
	case GLFW_KEY_1: return octoon::input::InputKey::Code::Key1;
	case GLFW_KEY_2: return octoon::input::InputKey::Code::Key2;
	case GLFW_KEY_3: return octoon::input::InputKey::Code::Key3;
	case GLFW_KEY_4: return octoon::input::InputKey::Code::Key4;
	case GLFW_KEY_5: return octoon::input::InputKey::Code::Key5;
	case GLFW_KEY_6: return octoon::input::InputKey::Code::Key6;
	case GLFW_KEY_7: return octoon::input::InputKey::Code::Key7;
	case GLFW_KEY_8: return octoon::input::InputKey::Code::Key8;
	case GLFW_KEY_9: return octoon::input::InputKey::Code::Key9;
	case GLFW_KEY_SEMICOLON: return octoon::input::InputKey::Code::Semicolon;
	case GLFW_KEY_EQUAL: return octoon::input::InputKey::Code::Equal;
	case GLFW_KEY_A: return octoon::input::InputKey::Code::A;
	case GLFW_KEY_B: return octoon::input::InputKey::Code::B;
	case GLFW_KEY_C: return octoon::input::InputKey::Code::C;
	case GLFW_KEY_D: return octoon::input::InputKey::Code::D;
	case GLFW_KEY_E: return octoon::input::InputKey::Code::E;
	case GLFW_KEY_F: return octoon::input::InputKey::Code::F;
	case GLFW_KEY_G: return octoon::input::InputKey::Code::G;
	case GLFW_KEY_H: return octoon::input::InputKey::Code::H;
	case GLFW_KEY_I: return octoon::input::InputKey::Code::I;
	case GLFW_KEY_J: return octoon::input::InputKey::Code::J;
	case GLFW_KEY_K: return octoon::input::InputKey::Code::K;
	case GLFW_KEY_L: return octoon::input::InputKey::Code::L;
	case GLFW_KEY_M: return octoon::input::InputKey::Code::M;
	case GLFW_KEY_N: return octoon::input::InputKey::Code::N;
	case GLFW_KEY_O: return octoon::input::InputKey::Code::O;
	case GLFW_KEY_P: return octoon::input::InputKey::Code::P;
	case GLFW_KEY_Q: return octoon::input::InputKey::Code::Q;
	case GLFW_KEY_R: return octoon::input::InputKey::Code::R;
	case GLFW_KEY_S: return octoon::input::InputKey::Code::S;
	case GLFW_KEY_T: return octoon::input::InputKey::Code::T;
	case GLFW_KEY_U: return octoon::input::InputKey::Code::U;
	case GLFW_KEY_V: return octoon::input::InputKey::Code::V;
	case GLFW_KEY_W: return octoon::input::InputKey::Code::W;
	case GLFW_KEY_X: return octoon::input::InputKey::Code::X;
	case GLFW_KEY_Y: return octoon::input::InputKey::Code::Y;
	case GLFW_KEY_Z: return octoon::input::InputKey::Code::Z;
	case GLFW_KEY_LEFT_BRACKET: return octoon::input::InputKey::Code::LeftBracket;
	case GLFW_KEY_BACKSLASH: return octoon::input::InputKey::Code::Backslash;
	case GLFW_KEY_RIGHT_BRACKET: return octoon::input::InputKey::Code::RightBracket;
	case GLFW_KEY_GRAVE_ACCENT: return octoon::input::InputKey::Code::GraveAccent;
	case GLFW_KEY_WORLD_1: return octoon::input::InputKey::Code::World1;
	case GLFW_KEY_WORLD_2: return octoon::input::InputKey::Code::World2;
	case GLFW_KEY_ESCAPE: return octoon::input::InputKey::Code::Escape;
	case GLFW_KEY_ENTER: return octoon::input::InputKey::Code::Enter;
	case GLFW_KEY_TAB: return octoon::input::InputKey::Code::Tab;
	case GLFW_KEY_BACKSPACE: return octoon::input::InputKey::Code::Backspace;
	case GLFW_KEY_INSERT: return octoon::input::InputKey::Code::Insert;
	case GLFW_KEY_DELETE: return octoon::input::InputKey::Code::Delete;
	case GLFW_KEY_RIGHT: return octoon::input::InputKey::Code::ArrowRight;
	case GLFW_KEY_LEFT: return octoon::input::InputKey::Code::ArrowLeft;
	case GLFW_KEY_DOWN: return octoon::input::InputKey::Code::ArrowDown;
	case GLFW_KEY_UP: return octoon::input::InputKey::Code::ArrowUp;
	case GLFW_KEY_PAGE_UP: return octoon::input::InputKey::Code::PageUp;
	case GLFW_KEY_PAGE_DOWN: return octoon::input::InputKey::Code::PageDown;
	case GLFW_KEY_HOME: return octoon::input::InputKey::Code::Home;
	case GLFW_KEY_END: return octoon::input::InputKey::Code::End;
	case GLFW_KEY_CAPS_LOCK: return octoon::input::InputKey::Code::CapsLock;
	case GLFW_KEY_SCROLL_LOCK: return octoon::input::InputKey::Code::ScrollLock;
	case GLFW_KEY_NUM_LOCK: return octoon::input::InputKey::Code::NumLock;
	case GLFW_KEY_PRINT_SCREEN: return octoon::input::InputKey::Code::PrintScreen;
	case GLFW_KEY_PAUSE: return octoon::input::InputKey::Code::Pause;
	case GLFW_KEY_F1: return octoon::input::InputKey::Code::F1;
	case GLFW_KEY_F2: return octoon::input::InputKey::Code::F2;
	case GLFW_KEY_F3: return octoon::input::InputKey::Code::F3;
	case GLFW_KEY_F4: return octoon::input::InputKey::Code::F4;
	case GLFW_KEY_F5: return octoon::input::InputKey::Code::F5;
	case GLFW_KEY_F6: return octoon::input::InputKey::Code::F6;
	case GLFW_KEY_F7: return octoon::input::InputKey::Code::F7;
	case GLFW_KEY_F8: return octoon::input::InputKey::Code::F8;
	case GLFW_KEY_F9: return octoon::input::InputKey::Code::F9;
	case GLFW_KEY_F10: return octoon::input::InputKey::Code::F10;
	case GLFW_KEY_F11: return octoon::input::InputKey::Code::F11;
	case GLFW_KEY_F12: return octoon::input::InputKey::Code::F12;
	case GLFW_KEY_F13: return octoon::input::InputKey::Code::F13;
	case GLFW_KEY_F14: return octoon::input::InputKey::Code::F14;
	case GLFW_KEY_F15: return octoon::input::InputKey::Code::F15;
	case GLFW_KEY_F16: return octoon::input::InputKey::Code::F16;
	case GLFW_KEY_F17: return octoon::input::InputKey::Code::F17;
	case GLFW_KEY_F18: return octoon::input::InputKey::Code::F18;
	case GLFW_KEY_F19: return octoon::input::InputKey::Code::F19;
	case GLFW_KEY_F20: return octoon::input::InputKey::Code::F20;
	case GLFW_KEY_F21: return octoon::input::InputKey::Code::F21;
	case GLFW_KEY_F22: return octoon::input::InputKey::Code::F22;
	case GLFW_KEY_F23: return octoon::input::InputKey::Code::F23;
	case GLFW_KEY_F24: return octoon::input::InputKey::Code::F24;
	case GLFW_KEY_F25: return octoon::input::InputKey::Code::F25;
	case GLFW_KEY_KP_0: return octoon::input::InputKey::Code::KP_0;
	case GLFW_KEY_KP_1: return octoon::input::InputKey::Code::KP_1;
	case GLFW_KEY_KP_2: return octoon::input::InputKey::Code::KP_2;
	case GLFW_KEY_KP_3: return octoon::input::InputKey::Code::KP_3;
	case GLFW_KEY_KP_4: return octoon::input::InputKey::Code::KP_4;
	case GLFW_KEY_KP_5: return octoon::input::InputKey::Code::KP_5;
	case GLFW_KEY_KP_6: return octoon::input::InputKey::Code::KP_6;
	case GLFW_KEY_KP_7: return octoon::input::InputKey::Code::KP_7;
	case GLFW_KEY_KP_8: return octoon::input::InputKey::Code::KP_8;
	case GLFW_KEY_KP_9: return octoon::input::InputKey::Code::KP_9;
	case GLFW_KEY_KP_DECIMAL: return octoon::input::InputKey::Code::KP_Decimal;
	case GLFW_KEY_KP_DIVIDE: return octoon::input::InputKey::Code::KP_Divide;
	case GLFW_KEY_KP_MULTIPLY: return octoon::input::InputKey::Code::KP_Multiply;
	case GLFW_KEY_KP_SUBTRACT: return octoon::input::InputKey::Code::KP_Subtract;
	case GLFW_KEY_KP_ADD: return octoon::input::InputKey::Code::KP_Add;
	case GLFW_KEY_KP_ENTER: return octoon::input::InputKey::Code::KP_Enter;
	case GLFW_KEY_KP_EQUAL: return octoon::input::InputKey::Code::KP_Equal;
	case GLFW_KEY_LEFT_SHIFT: return octoon::input::InputKey::Code::LeftShift;
	case GLFW_KEY_LEFT_CONTROL: return octoon::input::InputKey::Code::LeftControl;
	case GLFW_KEY_LEFT_ALT: return octoon::input::InputKey::Code::LeftAlt;
	case GLFW_KEY_LEFT_SUPER: return octoon::input::InputKey::Code::LeftSuper;
	case GLFW_KEY_RIGHT_SHIFT: return octoon::input::InputKey::Code::RightShift;
	case GLFW_KEY_RIGHT_CONTROL: return octoon::input::InputKey::Code::RightControl;
	case GLFW_KEY_RIGHT_ALT: return octoon::input::InputKey::Code::RightAlt;
	case GLFW_KEY_RIGHT_SUPER: return octoon::input::InputKey::Code::RightSuper;
	case GLFW_KEY_MENU: return octoon::input::InputKey::Code::Menu;
	default:
		return octoon::input::InputKey::Code::None;
	}
}

void onWindowResize(GLFWwindow* window, int w, int h)
{
	octoon::WindHandle hwnd = (octoon::WindHandle)glfwGetWinHandle(window);
	if (gameApp_)
		gameApp_->doWindowResize(hwnd, w, h);
}

void onWindowFramebufferResize(GLFWwindow* window, int w, int h)
{
	octoon::WindHandle hwnd = (octoon::WindHandle)glfwGetWinHandle(window);
	if (gameApp_)
		gameApp_->doWindowFramebufferResize(hwnd, w, h);
}

void onWindowClose(GLFWwindow* window)
{
	octoon::WindHandle hwnd = (octoon::WindHandle)glfwGetWinHandle(window);
	if (gameApp_)
		gameApp_->doWindowClose(hwnd);
}

void onWindowFocus(GLFWwindow* window, int focus)
{
	octoon::WindHandle hwnd = (octoon::WindHandle)glfwGetWinHandle(window);
	if (gameApp_)
		gameApp_->doWindowFocus(hwnd, focus);
}

void onWindowKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	octoon::WindHandle hwnd = (octoon::WindHandle)glfwGetWinHandle(window);
	if (gameApp_)
	{
		auto octoon_key = KeyCodetoInputKey(key);
		if (action == GLFW_PRESS)
			gameApp_->doWindowKeyDown(hwnd, octoon_key, scancode, mods);
		else if (action == GLFW_REPEAT)
			gameApp_->doWindowKeyPress(hwnd, octoon_key, scancode, mods);
		else if (action == GLFW_RELEASE)
			gameApp_->doWindowKeyUp(hwnd, octoon_key, scancode, mods);
	}
}

void onWindowKeyChar(GLFWwindow* window, unsigned int unicode, int mods)
{
	octoon::WindHandle hwnd = (octoon::WindHandle)glfwGetWinHandle(window);
	if (gameApp_)
		gameApp_->doWindowKeyChar(hwnd, unicode, mods);
}

void onWindowMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	if (gameApp_)
	{
		double mouseX, mouseY;
		::glfwGetCursorPos(window, &mouseX, &mouseY);

		octoon::WindHandle hwnd = (octoon::WindHandle)glfwGetWinHandle(window);
		if (action == GLFW_PRESS)
			gameApp_->doWindowMouseButtonDown(hwnd, (octoon::input::InputButton::Code)(octoon::input::InputButton::Mouse0 + button), mouseX, mouseY);
		else if (action == GLFW_RELEASE)
			gameApp_->doWindowMouseButtonUp(hwnd, (octoon::input::InputButton::Code)(octoon::input::InputButton::Mouse0 + button), mouseX, mouseY);

		if (action == GLFW_RELEASE)
		{
			static auto clicks = false;
			static auto before = std::chrono::system_clock::now();

			if (!clicks)
			{
				before = std::chrono::system_clock::now();
				clicks = true;
			}
			else
			{
				auto now = std::chrono::system_clock::now();
				double diff_ms = std::chrono::duration <double, std::milli>(now - before).count();
				if (diff_ms > 10 && diff_ms < 200)
					gameApp_->doWindowMouseButtonDoubleClick(hwnd, (octoon::input::InputButton::Code)(octoon::input::InputButton::Mouse0 + button), mouseX, mouseY);

				clicks = false;
			}
		}
	}
}

void onWindowMouseMotion(GLFWwindow* window, double x, double y)
{
	octoon::WindHandle hwnd = (octoon::WindHandle)glfwGetWinHandle(window);
	if (gameApp_)
		gameApp_->doWindowMouseMotion(hwnd, x, y);
}

void onWindowScrool(GLFWwindow* window, double x, double y)
{
	octoon::WindHandle hwnd = (octoon::WindHandle)glfwGetWinHandle(window);
	if (gameApp_)
		gameApp_->doWindowScrool(hwnd, x, y);
}

void onWindowDrop(GLFWwindow* window, int count, const char** file_utf8)
{
	octoon::WindHandle hwnd = (octoon::WindHandle)glfwGetWinHandle(window);
	if (gameApp_)
		gameApp_->doWindowDrop(hwnd, count, file_utf8);
}

bool OCTOON_CALL OctoonInit(const char* gamedir, const char* scenename) noexcept
{
#if GLFW_EXPOSE_NATIVE_WIN32
	::SetConsoleOutputCP(CP_UTF8);
#endif

	if (gamedir)
	{
		char drive[MAX_PATH];
		char dir[MAX_PATH];
		char filename[MAX_PATH];
		char ext[MAX_PATH];
		octoon::io::fcntl::splitpath(gamedir, drive, dir, filename, ext);

		gameRootPath_ = drive;
		gameRootPath_ += dir;

#if GLFW_EXPOSE_NATIVE_WIN32
		::SetCurrentDirectory(gameRootPath_.c_str());
#endif
	}

	if (scenename)
		gameScenePath_ = scenename;

	return true;
}

bool OCTOON_CALL OctoonOpenWindow(const char* title, int w, int h) noexcept
{
	assert(!gameApp_ && !window_);

	try
	{
		if (::glfwInit() == GL_FALSE)
			return false;

#if defined(GLFW_EXPOSE_NATIVE_X11)
		::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		::glfwWindowHint(GLFW_VISIBLE, false);
#else
		::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		::glfwWindowHint(GLFW_VISIBLE, false);
#endif

		window_ = ::glfwCreateWindow(w, h, title, nullptr, nullptr);
		if (window_)
		{
			::glfwSetWindowUserPointer(window_, &gameApp_);
			::glfwSetWindowFocusCallback(window_, &onWindowFocus);
			::glfwSetWindowCloseCallback(window_, &onWindowClose);
			::glfwSetWindowSizeCallback(window_, &onWindowResize);
			::glfwSetFramebufferSizeCallback(window_, &onWindowFramebufferResize);
			::glfwSetMouseButtonCallback(window_, &onWindowMouseButton);
			::glfwSetCursorPosCallback(window_, &onWindowMouseMotion);
			::glfwSetKeyCallback(window_, &onWindowKey);
			::glfwSetCharModsCallback(window_, &onWindowKeyChar);
			::glfwSetScrollCallback(window_, &onWindowScrool);
			::glfwSetDropCallback(window_, &onWindowDrop);

			auto screen = ::glfwGetVideoMode(::glfwGetPrimaryMonitor());
			::glfwSetWindowPos(window_, (screen->width - w) >> 1, (screen->height - h) >> 1);

			int widthMM, heightMM;
			::glfwGetMonitorPhysicalSize(::glfwGetPrimaryMonitor(), &widthMM, &heightMM);

			int framebuffer_w, framebuffer_h;
			::glfwGetFramebufferSize(window_, &framebuffer_w, &framebuffer_h);

			octoon::WindHandle hwnd = (octoon::WindHandle)::glfwGetWinHandle(window_);

			gameApp_ = octoon::GameApp::instance();
			gameApp_->open(hwnd, w, h, framebuffer_w, framebuffer_h);
			gameApp_->setActive(true);
			gameApp_->doWindowFocus(hwnd, true);
			gameApp_->doWindowResize(hwnd, w, h);
			gameApp_->doWindowFramebufferResize(hwnd, framebuffer_w, framebuffer_h);

			if (!gameScenePath_.empty())
			{
				if (!gameApp_->openScene(gameScenePath_))
					throw octoon::runtime::runtime_error::create("GameApplication::open_scene() failed");
			}

			::glfwShowWindow(window_);
			return true;
		}

		return true;
	}
	catch (...)
	{
		OctoonCloseWindow();

		gameApp_->close();
		gameApp_ = nullptr;

		return false;
	}
}

void OCTOON_CALL OctoonCloseWindow() noexcept
{
	if (gameApp_)
	{
		gameApp_->close();
		gameApp_ = nullptr;
	}

	if (window_)
	{
		::glfwDestroyWindow(window_);
		window_ = nullptr;
	}

	::glfwTerminate();
}

bool OCTOON_CALL OctoonIsQuitRequest() noexcept
{
	if (!gameApp_)
		return true;

	if (glfwWindowShouldClose(window_) || gameApp_->isQuitRequest())
		return true;

	return false;
}

void OCTOON_CALL OctoonUpdate() noexcept
{
	::glfwPollEvents();

	if (gameApp_)
		gameApp_->update();
}

void OCTOON_CALL OctoonTerminate() noexcept
{
	OctoonCloseWindow();
}
#endif