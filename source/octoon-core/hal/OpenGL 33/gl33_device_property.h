#ifndef OCTOON_GL33_DEVICE_PROPERTY_H_
#define OCTOON_GL33_DEVICE_PROPERTY_H_

#include "ogl_device_property.h"

namespace octoon
{
	namespace hal
	{
		class GL33DeviceProperty final : public OGLDeviceProperty
		{
		public:
			GL33DeviceProperty() noexcept;
			~GL33DeviceProperty() noexcept;

		private:
			bool initDeviceProperties() noexcept;

			bool initTextureSupports() noexcept;
			bool initTextureDimSupports() noexcept;
			bool initVertexSupports() noexcept;
			bool initShaderSupports() noexcept;

		private:
			GL33DeviceProperty(const GL33DeviceProperty&) = delete;
			GL33DeviceProperty& operator=(const GL33DeviceProperty&) = delete;
		};
	}
}

#endif