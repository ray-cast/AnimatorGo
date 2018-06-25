#ifndef OCTOON_GL33_GRAPHICS_DATA_H_
#define OCTOON_GL33_GRAPHICS_DATA_H_

#include "gl33_types.h"

namespace octoon
{
	namespace graphics
	{
		class GL33GraphicsData final : public GraphicsData
		{
			OctoonDeclareSubClass(GL33GraphicsData, GraphicsData)
		public:
			GL33GraphicsData() noexcept;
			virtual ~GL33GraphicsData() noexcept;

			bool setup(const GraphicsDataDesc& desc) noexcept;
			void close() noexcept;

			bool is_open() const noexcept;

			bool map(std::ptrdiff_t begin, std::ptrdiff_t count, void** data) noexcept;
			void unmap() noexcept;

			GLuint getInstanceID() const noexcept;

			const GraphicsDataDesc& getDataDesc() const noexcept override;

		private:
			friend class GL33Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL33GraphicsData(const GL33GraphicsData&) noexcept = delete;
			GL33GraphicsData& operator=(const GL33GraphicsData&) noexcept = delete;

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