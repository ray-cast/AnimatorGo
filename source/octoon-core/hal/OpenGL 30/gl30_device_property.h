#ifndef OCTOON_GL30_DEVICE_PROPERTY_H_
#define OCTOON_GL30_DEVICE_PROPERTY_H_

#include "ogl_device_property.h"

namespace octoon
{
	namespace hal
	{
		class GL30DeviceProperty final : public OGLDeviceProperty
		{
		public:
			GL30DeviceProperty() noexcept;
			~GL30DeviceProperty() noexcept;

			bool initDeviceProperties() noexcept;

		private:
			bool initTextureSupports() noexcept;
			bool initTextureDimSupports() noexcept;
			bool initVertexSupports() noexcept;
			bool initShaderSupports() noexcept;

		private:
			GL30DeviceProperty(const GL30DeviceProperty&) = delete;
			GL30DeviceProperty& operator=(const GL30DeviceProperty&) = delete;
		};
	}
}

#endif