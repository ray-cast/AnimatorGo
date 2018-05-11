#ifndef OCTOON_GL20_GRAPHICS_DATA_H_
#define OCTOON_GL20_GRAPHICS_DATA_H_

#include "gl20_types.h"

namespace octoon 
{
	namespace graphics
	{
		class GL20GraphicsData final : public GraphicsData
		{
			OctoonDeclareSubClass(GL20GraphicsData, GraphicsData)
		public:
			GL20GraphicsData() noexcept;
			~GL20GraphicsData() noexcept;

			bool setup(const GraphicsDataDesc& desc) noexcept;
			void close() noexcept;

			int flush() noexcept;
			int flush(GLintptr offset, GLsizeiptr cnt) noexcept;

			bool map(std::intptr_t offset, std::intptr_t count, void** data) noexcept;
			void unmap() noexcept;

			GLuint getInstanceID() const noexcept;

			void bind() noexcept;

			const GraphicsDataDesc& getGraphicsDataDesc() const noexcept;

		private:
			friend class GL20Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			GL20GraphicsData(const GL20GraphicsData&) noexcept = delete;
			GL20GraphicsData& operator=(const GL20GraphicsData&) noexcept = delete;

		private:
			GLuint _buffer;
			GLenum _target;
			GLsizeiptr _dataSize;
			GLintptr _dataOffset;
			GLboolean _isMapping;
			std::vector<char> _data;
			std::uint32_t _usage;
			GraphicsDataDesc _desc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif