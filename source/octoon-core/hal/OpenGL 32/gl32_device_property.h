#ifndef OCTOON_GL32_DEVICE_PROPERTY_H_
#define OCTOON_GL32_DEVICE_PROPERTY_H_

#include "ogl_device_property.h"

namespace octoon
{
	namespace hal
	{
		class GL32DeviceProperty final : public OGLDeviceProperty
		{
		public:
			GL32DeviceProperty() noexcept;
			~GL32DeviceProperty() noexcept;

			bool initDeviceProperties() noexcept;

		private:
			bool initTextureSupports() noexcept;
			bool initTextureDimSupports() noexcept;
			bool initVertexSupports() noexcept;
			bool initShaderSupports() noexcept;

		private:
			GL32DeviceProperty(const GL32DeviceProperty&) = delete;
			GL32DeviceProperty& operator=(const GL32DeviceProperty&) = delete;
		};
	}
}

#endif