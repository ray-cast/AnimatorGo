#ifndef OCTOON_EGL3_DEVICE_PROPERTY_H_
#define OCTOON_EGL3_DEVICE_PROPERTY_H_

#include "egl3_types.h"

namespace octoon 
{
	namespace graphics
	{
		class EGL3DeviceProperty final : public GraphicsDeviceProperty
		{
		public:
			EGL3DeviceProperty() noexcept;
			~EGL3DeviceProperty() noexcept;

			bool setup(const GraphicsDeviceDesc& deviceDesc) noexcept;
			void close() noexcept;

			const GraphicsDeviceProperties& getDeviceProperties() const noexcept;

		private:
			bool initPixelFormat(const GraphicsDeviceDesc& deviceDesc, EGLConfig& config) noexcept;
			bool initContext(const GraphicsDeviceDesc& deviceDesc, EGLConfig config) noexcept;
			bool initDeviceProperties() noexcept;

			bool initTextureSupports() noexcept;
			bool initTextureDimSupports() noexcept;
			bool initVertexSupports() noexcept;
			bool initShaderSupports() noexcept;

		private:
			EGL3DeviceProperty(const EGL3DeviceProperty&) = delete;
			EGL3DeviceProperty& operator=(const EGL3DeviceProperty&) = delete;

		private:
			GraphicsDeviceProperties _deviceProperties;
		};
	}
}

#endif