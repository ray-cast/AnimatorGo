#include <octoon/graphics/graphics_system.h>
#include <octoon/graphics/graphics_device.h>

#if defined(OCTOON_FEATURE_GRAPHICS_USE_OPENGL)
#	include "OpenGL/ogl_device.h"
#endif
#if defined(OCTOON_FEATURE_GRAPHICS_USE_OPENGL_ES2)
#	include "OpenGL ES2/egl2_device.h"
#endif
#if defined(OCTOON_FEATURE_GRAPHICS_USE_OPENGL_ES3)
#	include "OpenGL ES3/egl3_device.h"
#endif
#if defined(OCTOON_FEATURE_GRAPHICS_USE_VULKAN)
#   include "Vulkan/vk_system.h"
#	include "Vulkan/vk_device.h"
#endif

namespace octoon
{
	namespace graphics
	{
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
#if OCTOON_FEATURE_GRAPHICS_USE_OPENGL
					if (device->is_instance_of<OGLDevice>())
					{
						device->downcast<OGLDevice>()->enableDebugControl(enable);
						return;
					}
#endif
#if OCTOON_FEATURE_GRAPHICS_USE_OPENGL_ES2
					if (device->is_instance_of<EGL2Device>())
					{
						device->downcast<EGL2Device>()->enableDebugControl(enable);
						return;
					}
#endif
#if OCTOON_FEATURE_GRAPHICS_USE_OPENGL_ES3
					if (device->is_instance_of<EGL3Device>())
					{
						device->downcast<EGL3Device>()->enableDebugControl(enable);
						return;
			}

#endif
#if OCTOON_FEATURE_GRAPHICS_USE_VULKAN
					if (device->is_instance_of<VulkanDevice>())
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

#if defined(OCTOON_FEATURE_GRAPHICS_USE_OPENGL)
			if (deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLCore ||
				deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGL)
			{
				auto device = std::make_shared<OGLDevice>();
				if (device->setup(deviceDesc))
				{
					_devices.push_back(device);
					return device;
				}

				return nullptr;
		}

#endif
#if defined(OCTOON_FEATURE_GRAPHICS_USE_OPENGL_ES2)
			if (deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES2)
			{
				auto device = std::make_shared<EGL2Device>();
				if (device->setup(deviceDesc))
				{
					_devices.push_back(device);
					return device;
				}

				return nullptr;
			}
#endif
#if defined(OCTOON_FEATURE_GRAPHICS_USE_OPENGL_ES3)
			if (deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES3 ||
				deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES31)
			{
				auto device = std::make_shared<EGL3Device>();
				if (device->setup(deviceDesc))
				{
					_devices.push_back(device);
					return device;
				}

				return nullptr;
			}
#endif
#if defined(OCTOON_FEATURE_GRAPHICS_USE_VULKAN)
			if (deviceType == GraphicsDeviceType::GraphicsDeviceTypeVulkan)
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