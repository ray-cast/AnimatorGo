#ifndef OCTOON_GL20_DEVICE_PROPERTY_H_
#define OCTOON_GL20_DEVICE_PROPERTY_H_

#include "ogl_device_property.h"

namespace octoon
{
	namespace hal
	{
		class GL20DeviceProperty final : public OGLDeviceProperty
		{
		public:
			GL20DeviceProperty() noexcept;
			~GL20DeviceProperty() noexcept;

		private:
			bool initDeviceProperties() noexcept;

			bool initTextureSupports() noexcept;
			bool initTextureDimSupports() noexcept;
			bool initVertexSupports() noexcept;
			bool initShaderSupports() noexcept;

		private:
			GL20DeviceProperty(const GL20DeviceProperty&) = delete;
			GL20DeviceProperty& operator=(const GL20DeviceProperty&) = delete;
		};
	}
}

#endif