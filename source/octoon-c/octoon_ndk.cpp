#if defined(OCTOON_BUILD_PLATFORM_ANDROID)
#include <ray/ray.h>
#include <ray/ray_main.h>

#include <ray/game_application.h>

#include <ray/fcntl.h>
#include <ray/input.h>

#include <android_native_app_glue.h>

octoon::GameApplicationPtr gameApp_;
std::string gameRootPath_;
std::string gameScenePath_;
bool isQuitRequest_ = false;
android_app* android_app_ = nullptr;

bool OctoonInit(const char* gamedir, const char* scenename) noexcept
{
	if (gamedir)
	{
		if (octoon::fcntl::access(gamedir, 0) == 0)
			gameRootPath_ = octoon::util::directory(gamedir);
	}

	if (scenename)
		gameScenePath_ = scenename;

	isQuitRequest_ = false;

	return true;
}

void OctoonTerminate() noexcept
{
	OctoonCloseWindow();
}

bool OctoonOpenWindow(const char* title, int w, int h) noexcept
{
	gameApp_ = std::make_shared<octoon::GameApplication>();
	gameApp_->setFileService(true);
	gameApp_->setFileServicePath(gameRootPath_);

	if (w == 0)
		w = android_app_->contentRect.right - android_app_->contentRect.left;
	if (h == 0)
		h = android_app_->contentRect.bottom - android_app_->contentRect.top;

	if (!gameApp_->open(android_app_->window, w, h))
		return false;

	if (!gameScenePath_.empty())
	{
		if (!gameApp_->openScene(gameScenePath_))
			return false;
	}

	return true;
}

void OctoonCloseWindow() noexcept
{
	if (gameApp_)
	{
		gameApp_.reset();
		gameApp_ = nullptr;
	}

	isQuitRequest_ = true;
}

bool OctoonIsQuitRequest() noexcept
{
	return isQuitRequest_ || android_app_->destroyRequested ? true : false;
}

void OctoonUpdate() noexcept
{
	if (gameApp_)
		gameApp_->update();
}

static int32_t onInputEvent(struct android_app* app, AInputEvent* event)
{
	int32_t lEventType = AInputEvent_getType(event);
	switch (lEventType)
	{
	case AINPUT_EVENT_TYPE_MOTION:
		switch (AInputEvent_getSource(event))
		{
		case AINPUT_SOURCE_TOUCHSCREEN:
			break;
		case AINPUT_SOURCE_TRACKBALL:
			break;
		}
	case AINPUT_EVENT_TYPE_KEY:
	{
		octoon::input::InputEvent inputEvent;
		inputEvent.event = octoon::input::InputEvent::Character;
		inputEvent.key.timestamp = ::AKeyEvent_getEventTime(event);
		inputEvent.key.state = true;

		inputEvent.key.keysym.raw = AKeyEvent_getScanCode(event);
		inputEvent.key.keysym.sym = AKeyEvent_getKeyCode(event);
		inputEvent.key.keysym.unicode = AKeyEvent_getKeyCode(event);
	}
	}

	int32_t id = AMotionEvent_getAction(event);
	switch (id)
	{
	case AMOTION_EVENT_ACTION_MOVE:
	{
		octoon::input::InputEvent inputEvent;
		inputEvent.event = octoon::input::InputEvent::TouchMotionMove;
		inputEvent.motion.x = ::AMotionEvent_getXOffset(event);
		inputEvent.motion.y = ::AMotionEvent_getYOffset(event);
		inputEvent.motion.xrel = inputEvent.motion.x;
		inputEvent.motion.yrel = inputEvent.motion.y;
		inputEvent.motion.state = ::AMotionEvent_getButtonState(event);
		inputEvent.motion.timestamp = ::AMotionEvent_getEventTime(event);
		inputEvent.button.button = octoon::input::InputButton::MOUSE0;
	}
	break;
	case AMOTION_EVENT_ACTION_DOWN:
	{
		octoon::input::InputEvent inputEvent;
		inputEvent.event = octoon::input::InputEvent::TouchMotionDown;
		inputEvent.motion.x = ::AMotionEvent_getXOffset(event);
		inputEvent.motion.y = ::AMotionEvent_getYOffset(event);
		inputEvent.motion.xrel = inputEvent.motion.x;
		inputEvent.motion.yrel = inputEvent.motion.y;
		inputEvent.motion.state = ::AMotionEvent_getButtonState(event);
		inputEvent.motion.timestamp = ::AMotionEvent_getDownTime(event);
		inputEvent.button.button = octoon::input::InputButton::MOUSE0;
	}
	break;
	case AMOTION_EVENT_ACTION_UP:
	{
		octoon::input::InputEvent inputEvent;
		inputEvent.event = octoon::input::InputEvent::TouchMotionUp;
		inputEvent.motion.x = ::AMotionEvent_getXOffset(event);
		inputEvent.motion.y = ::AMotionEvent_getYOffset(event);
		inputEvent.motion.xrel = inputEvent.motion.x;
		inputEvent.motion.yrel = inputEvent.motion.y;
		inputEvent.motion.state = ::AMotionEvent_getButtonState(event);
		inputEvent.motion.timestamp = ::AMotionEvent_getEventTime(event);
		inputEvent.button.button = octoon::input::InputButton::MOUSE0;
	}
	break;
	case AMOTION_EVENT_ACTION_CANCEL:
	{
		octoon::input::InputEvent inputEvent;
		inputEvent.event = octoon::input::InputEvent::TouchMotionCancel;
		inputEvent.motion.x = ::AMotionEvent_getXOffset(event);
		inputEvent.motion.y = ::AMotionEvent_getYOffset(event);
		inputEvent.motion.xrel = inputEvent.motion.x;
		inputEvent.motion.yrel = inputEvent.motion.y;
		inputEvent.motion.state = ::AMotionEvent_getButtonState(event);
		inputEvent.motion.timestamp = ::AMotionEvent_getEventTime(event);
		inputEvent.button.button = octoon::input::InputButton::MOUSE0;
	}
	break;
	}

	return 1;
}

void onAppCmd(struct android_app* app, int32_t cmd)
{
}

void android_main(android_app* app)
{
	android_app_ = app;
	android_app_->onAppCmd = &onAppCmd;
	android_app_->onInputEvent = &onInputEvent;

	ray_main(0, 0);
}

#endif