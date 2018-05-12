#ifndef OCTOON_GL30_GRAPHICS_DATA_H_
#define OCTOON_GL30_GRAPHICS_DATA_H_

#include "gl30_types.h"

namespace octoon
{
	namespace graphics
	{
		class GL30GraphicsData final : public GraphicsData
		{
			OctoonDeclareSubClass(GL30GraphicsData, GraphicsData)
		public:
			GL30GraphicsData() noexcept;
			virtual ~GL30GraphicsData() noexcept;

			bool setup(const GraphicsDataDesc& desc) noexcept;
			void close() noexcept;

			int flush() noexcept;
			int flush(GLintptr offset, GLsizeiptr cnt) noexcept;

			bool map(std::ptrdiff_t offset, std::ptrdiff_t count, void** data) noexcept;
			void unmap() noexcept;

			GLuint getInstanceID() const noexcept;

			const GraphicsDataDesc& getGraphicsDataDesc() const noexcept;

		private:
			friend class GL30Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			GL30GraphicsData(const GL30GraphicsData&) noexcept = delete;
			GL30GraphicsData& operator=(const GL30GraphicsData&) noexcept = delete;

		private:
			GLuint _buffer;
			GLenum _target;
			GraphicsDataDesc _desc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif