#ifndef OCTOON_GL20_DEVICE_PROPERTY_H_
#define OCTOON_GL20_DEVICE_PROPERTY_H_

#include "gl20_types.h"

namespace octoon
{
	namespace graphics
	{
		class GL20DeviceProperty final : public GraphicsDeviceProperty
		{
		public:
			GL20DeviceProperty() noexcept;
			~GL20DeviceProperty() noexcept;

			bool setup(const GraphicsDeviceDesc& deviceDesc) noexcept;
			void close() noexcept;

			const GraphicsDeviceProperties& getDeviceProperties() const noexcept override;

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
			GL20DeviceProperty(const GL20DeviceProperty&) = delete;
			GL20DeviceProperty& operator=(const GL20DeviceProperty&) = delete;

		private:
			GraphicsDeviceProperties _deviceProperties;
		};
	}
}

#endif