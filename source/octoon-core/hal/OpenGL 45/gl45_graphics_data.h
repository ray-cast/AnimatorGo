#ifndef OCTOON_GL33_CORE_GRAPHICS_DATA_H_
#define OCTOON_GL33_CORE_GRAPHICS_DATA_H_

#include "gl33_types.h"

namespace octoon
{
	namespace hal
	{
		class GL45GraphicsData final : public GraphicsData
		{
			OctoonDeclareSubClass(GL45GraphicsData, GraphicsData)
		public:
			GL45GraphicsData() noexcept;
			virtual ~GL45GraphicsData() noexcept;

			bool setup(const GraphicsDataDesc& desc) noexcept;
			void close() noexcept;

			std::ptrdiff_t flush() noexcept;
			std::ptrdiff_t flush(GLintptr offset, GLsizeiptr cnt) noexcept;

			bool map(std::ptrdiff_t offset, std::ptrdiff_t count, void** data) noexcept;
			void unmap() noexcept;

			GLuint getInstanceID() const noexcept;
			GLuint64 getInstanceAddr() const noexcept;

			const GraphicsDataDesc& getDataDesc() const noexcept override;

		private:
			friend class GL33Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL45GraphicsData(const GL45GraphicsData&) noexcept = delete;
			GL45GraphicsData& operator=(const GL45GraphicsData&) noexcept = delete;

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