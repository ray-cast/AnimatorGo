#ifndef OCTOON_OGL_CORE_GRAPHICS_DATA_H_
#define OCTOON_OGL_CORE_GRAPHICS_DATA_H_

#include "ogl_types.h"

namespace octoon
{
	namespace graphics
	{
		class OGLCoreGraphicsData final : public GraphicsData
		{
			OctoonDeclareSubClass(OGLCoreGraphicsData, GraphicsData)
		public:
			OGLCoreGraphicsData() noexcept;
			virtual ~OGLCoreGraphicsData() noexcept;

			bool setup(const GraphicsDataDesc& desc) noexcept;
			void close() noexcept;

			std::ptrdiff_t flush() noexcept;
			std::ptrdiff_t flush(GLintptr offset, GLsizeiptr cnt) noexcept;

			bool map(std::ptrdiff_t offset, std::ptrdiff_t count, void** data) noexcept;
			void unmap() noexcept;

			GLuint getInstanceID() const noexcept;
			GLuint64 getInstanceAddr() const noexcept;

			const GraphicsDataDesc& getGraphicsDataDesc() const noexcept;

		private:
			friend class OGLDevice;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			OGLCoreGraphicsData(const OGLCoreGraphicsData&) noexcept = delete;
			OGLCoreGraphicsData& operator=(const OGLCoreGraphicsData&) noexcept = delete;

		private:
			GLuint _buffer;
			GLuint64 _bufferAddr;
			GLvoid* _data;
			GraphicsDataDesc _desc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif