#include <octoon/graphics/graphics_device.h>

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubInterface(GraphicsDevice, runtime::RttiInterface, "GraphicsDevice")

		GraphicsDeviceDesc::GraphicsDeviceDesc() noexcept
			: _deviceType(GraphicsDeviceType::GraphicsDeviceTypeUndefined)
		{
		}

		GraphicsDeviceDesc::GraphicsDeviceDesc(GraphicsDeviceType type) noexcept
			: _deviceType(type)
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
	}
}