#ifndef OCTOON_OGL_GRAPHICS_DATA_H_
#define OCTOON_OGL_GRAPHICS_DATA_H_

#include "ogl_types.h"

namespace octoon
{
	namespace graphics
	{
		class OGLGraphicsData final : public GraphicsData
		{
			OctoonDeclareSubClass(OGLGraphicsData, GraphicsData)
		public:
			OGLGraphicsData() noexcept;
			virtual ~OGLGraphicsData() noexcept;

			bool setup(const GraphicsDataDesc& desc) noexcept;
			void close() noexcept;

			bool is_open() const noexcept;

			bool map(std::ptrdiff_t begin, std::ptrdiff_t count, void** data) noexcept;
			void unmap() noexcept;

			GLuint getInstanceID() const noexcept;

			const GraphicsDataDesc& getGraphicsDataDesc() const noexcept;

		private:
			friend class OGLDevice;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			OGLGraphicsData(const OGLGraphicsData&) noexcept = delete;
			OGLGraphicsData& operator=(const OGLGraphicsData&) noexcept = delete;

		private:
			GLuint _buffer;
			GLenum _target;
			GLvoid* _data;
			GraphicsDataDesc _desc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif