#include <octoon/hal/graphics_device.h>

namespace octoon
{
	OctoonImplementSubInterface(GraphicsDevice, Object, "GraphicsDevice")

	GraphicsDeviceDesc::GraphicsDeviceDesc() noexcept
		: _deviceType(GraphicsDeviceType::OpenGL33)
		, _enableDebug(false)
	{
	}

	GraphicsDeviceDesc::GraphicsDeviceDesc(GraphicsDeviceType type, bool enable) noexcept
		: _deviceType(type)
		, _enableDebug(enable)
	{
	}

	GraphicsDeviceDesc::~GraphicsDeviceDesc() noexcept
	{
	}

	void
	GraphicsDeviceDesc::setDeviceType(GraphicsDeviceType deviceType) noexcept
	{
		_deviceType = deviceType;
	}

	GraphicsDeviceType
	GraphicsDeviceDesc::getDeviceType() const noexcept
	{
		return _deviceType;
	}

	void
	GraphicsDeviceDesc::setDebugControl(bool enable) noexcept
	{
		_enableDebug = enable;
	}

	bool
	GraphicsDeviceDesc::isEnabledDebugControl() const noexcept
	{
		return _enableDebug;
	}
}