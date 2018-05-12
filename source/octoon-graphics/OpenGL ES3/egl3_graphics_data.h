#ifndef OCTOON_EGL3_GRAPHICS_DATA_H_
#define OCTOON_EGL3_GRAPHICS_DATA_H_

#include "egl3_types.h"

namespace octoon
{
	namespace graphics
	{
		class EGL3GraphicsData final : public GraphicsData
		{
			OctoonDeclareSubClass(EGL3GraphicsData, GraphicsData)
		public:
			EGL3GraphicsData() noexcept;
			virtual ~EGL3GraphicsData() noexcept;

			bool setup(const GraphicsDataDesc& desc) noexcept;
			void close() noexcept;

			int flush() noexcept;
			int flush(GLintptr offset, GLsizeiptr cnt) noexcept;

			bool map(std::ptrdiff_t offset, std::ptrdiff_t count, void** data) noexcept;
			void unmap() noexcept;

			GLuint getInstanceID() const noexcept;

			const GraphicsDataDesc& getGraphicsDataDesc() const noexcept;

		private:
			friend class EGL3Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			EGL3GraphicsData(const EGL3GraphicsData&) noexcept = delete;
			EGL3GraphicsData& operator=(const EGL3GraphicsData&) noexcept = delete;

		private:
			GLuint _buffer;
			GLenum _target;
			GraphicsDataDesc _desc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif