#ifndef OCTOON_GL32_DEVICE_PROPERTY_H_
#define OCTOON_GL32_DEVICE_PROPERTY_H_

#include "gl32_types.h"

namespace octoon
{
	namespace graphics
	{
		class GL32DeviceProperty final : public GraphicsDeviceProperty
		{
		public:
			GL32DeviceProperty() noexcept;
			~GL32DeviceProperty() noexcept;

			bool setup(const GraphicsDeviceDesc& deviceDesc) noexcept;
			void close() noexcept;

			const GraphicsDeviceProperties& getDeviceProperties() const noexcept;

		private:
#if defined(_WIN32)
			bool setupWGLEnvironment(HWND& hwnd, HDC& hdc, HINSTANCE& hinstance) noexcept;
			bool setupWGLPixelFormat(HDC hdc) noexcept;
			bool setupWGLExtensions(HDC hdc) noexcept;
			void closeWGLEnvironment(HWND hwnd, HDC hdc, HINSTANCE hinstance) noexcept;
#else
			bool initPixelFormat(const GraphicsDeviceDesc& deviceDesc, EGLConfig& config) noexcept;
			bool initContext(const GraphicsDeviceDesc& deviceDesc, EGLConfig config) noexcept;
#endif

			bool initDeviceProperties() noexcept;

			bool initTextureSupports() noexcept;
			bool initTextureDimSupports() noexcept;
			bool initVertexSupports() noexcept;
			bool initShaderSupports() noexcept;

		private:
			GL32DeviceProperty(const GL32DeviceProperty&) = delete;
			GL32DeviceProperty& operator=(const GL32DeviceProperty&) = delete;

		private:
			GraphicsDeviceProperties _deviceProperties;
		};
	}
}

#endif