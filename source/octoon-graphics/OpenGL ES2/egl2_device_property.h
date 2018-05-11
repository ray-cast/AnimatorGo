#ifndef OCTOON_EGL2_DEVICE_PROPERTY_H_
#define OCTOON_EGL2_DEVICE_PROPERTY_H_

#include "egl2_types.h"

namespace octoon 
{
	namespace graphics
	{
		class EGL2DeviceProperty final : public GraphicsDeviceProperty
		{
		public:
			EGL2DeviceProperty() noexcept;
			~EGL2DeviceProperty() noexcept;

			bool setup(const GraphicsDeviceDesc& deviceDesc) noexcept;
			void close() noexcept;

			const GraphicsDeviceProperties& getDeviceProperties() const noexcept override;

		private:
#if defined(_WIN32)
			bool setupWGLEnvironment(HWND& hwnd, HDC& hdc, HINSTANCE& hinstance) noexcept;
			bool setupWGLPixelFormat(HDC hdc) noexcept;
			bool setupWGLExtensions(HDC hdc) noexcept;
			void closeWGLEnvironment(HWND hwnd, HDC hdc, HINSTANCE hinstance) noexcept;
#endif

			bool initPixelFormat(const GraphicsDeviceDesc& deviceDesc, EGLConfig& config) noexcept;
			bool initContext(const GraphicsDeviceDesc& deviceDesc, EGLConfig config) noexcept;
			bool initDeviceProperties() noexcept;

			bool initTextureSupports() noexcept;
			bool initTextureDimSupports() noexcept;
			bool initVertexSupports() noexcept;
			bool initShaderSupports() noexcept;

		private:
			EGL2DeviceProperty(const EGL2DeviceProperty&) = delete;
			EGL2DeviceProperty& operator=(const EGL2DeviceProperty&) = delete;

		private:
			GraphicsDeviceProperties _deviceProperties;
		};
	}
}

#endif