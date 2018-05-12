#include <octoon/graphics/graphics_system.h>
#include <octoon/graphics/graphics_device.h>

#if defined(OCTOON_FEATURE_GRAPHICS_USE_OPENGL20)
#	include "OpenGL 20/gl20_device.h"
#endif
#if defined(OCTOON_FEATURE_GRAPHICS_USE_OPENGL30)
#	include "OpenGL 30/gl30_device.h"
#endif
#if defined(OCTOON_FEATURE_GRAPHICS_USE_OPENGL32)
#	include "OpenGL 32/gl32_device.h"
#endif
#if defined(OCTOON_FEATURE_GRAPHICS_USE_OPENGL33)
#	include "OpenGL 33/gl33_device.h"
#endif
#if defined(OCTOON_FEATURE_GRAPHICS_USE_VULKAN)
#   include "Vulkan/vk_system.h"
#	include "Vulkan/vk_device.h"
#endif

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSingleton(GraphicsSystem)

		GraphicsSystem::GraphicsSystem() noexcept
			: _debugMode(false)
		{
		}

		GraphicsSystem::~GraphicsSystem() noexcept
		{
			this->enableDebugControl(false);
		}

		bool
		GraphicsSystem::open(bool debugControl) noexcept
		{
			_debugMode = debugControl;
			return true;
		}

		void
		GraphicsSystem::close() noexcept
		{
			if (_debugMode)
			{
				this->enableDebugControl(false);
				_debugMode = false;
			}

			for (auto& it : _devices)
			{
				it.reset();
			}
		}

		void
		GraphicsSystem::enableDebugControl(bool enable) noexcept
		{
			for (auto& it : _devices)
			{
				auto device = it.lock();
				if (device)
				{
#if OCTOON_FEATURE_GRAPHICS_USE_OPENGL20
					if (device->isInstanceOf<GL20Device>())
					{
						device->downcast<GL20Device>()->enableDebugControl(enable);
						return;
					}
#endif
#if OCTOON_FEATURE_GRAPHICS_USE_OPENGL30
					if (device->isInstanceOf<GL30Device>())
					{
						device->downcast<GL30Device>()->enableDebugControl(enable);
						return;
					}
#endif
#if OCTOON_FEATURE_GRAPHICS_USE_OPENGL32
					if (device->isInstanceOf<GL32Device>())
					{
						device->downcast<GL32Device>()->enableDebugControl(enable);
						return;
			}
#endif
#if OCTOON_FEATURE_GRAPHICS_USE_OPENGL33
					if (device->isInstanceOf<GL33Device>())
					{
						device->downcast<GL33Device>()->enableDebugControl(enable);
						return;
					}
#endif
#if OCTOON_FEATURE_GRAPHICS_USE_VULKAN
					if (device->isInstanceOf<VulkanDevice>())
					{
						if (enable)
							VulkanSystem::instance()->startDebugControl();
						else
							VulkanSystem::instance()->stopDebugControl();
						return;
					}
#endif
				}
			}

			_debugMode = true;
		}

		bool
		GraphicsSystem::enableDebugControl() const noexcept
		{
			return _debugMode;
		}

		GraphicsDevicePtr
		GraphicsSystem::createDevice(const GraphicsDeviceDesc& deviceDesc) noexcept
		{
			GraphicsDeviceType deviceType = deviceDesc.getDeviceType();

#if defined(OCTOON_FEATURE_GRAPHICS_USE_OPENGL20)
			if (deviceType == GraphicsDeviceType::OpenGL20)
			{
				auto device = std::make_shared<GL20Device>();
				if (device->setup(deviceDesc))
				{
					_devices.push_back(device);
					return device;
				}

				return nullptr;
			}
#endif
#if defined(OCTOON_FEATURE_GRAPHICS_USE_OPENGL30)
			if (deviceType == GraphicsDeviceType::OpenGL30)
			{
				auto device = std::make_shared<GL30Device>();
				if (device->setup(deviceDesc))
				{
					_devices.push_back(device);
					return device;
				}

				return nullptr;
			}
#endif
#if defined(OCTOON_FEATURE_GRAPHICS_USE_OPENGL32)
			if (deviceType == GraphicsDeviceType::OpenGL32)
			{
				auto device = std::make_shared<GL32Device>();
				if (device->setup(deviceDesc))
				{
					_devices.push_back(device);
					return device;
				}

				return nullptr;
			}
#endif
#if defined(OCTOON_FEATURE_GRAPHICS_USE_OPENGL33)
			if (deviceType == GraphicsDeviceType::OpenGL45 ||
				deviceType == GraphicsDeviceType::OpenGL33)
			{
				auto device = std::make_shared<GL33Device>();
				if (device->setup(deviceDesc))
				{
					_devices.push_back(device);
					return device;
				}

				return nullptr;
			}

#endif
#if defined(OCTOON_FEATURE_GRAPHICS_USE_VULKAN)
			if (deviceType == GraphicsDeviceType::Vulkan)
			{
				if (!VulkanSystem::instance()->open())
					return false;

				auto device = std::make_shared<VulkanDevice>();
				if (device->setup(deviceDesc))
				{
					_devices.push_back(device);
					return device;
				}

				return nullptr;
	}
#endif
			return nullptr;
		}
	}
}