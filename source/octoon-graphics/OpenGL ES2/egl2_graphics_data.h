#ifndef OCTOON_EGL2_GRAPHICS_DATA_H_
#define OCTOON_EGL2_GRAPHICS_DATA_H_

#include "egl2_types.h"

namespace octoon 
{
	namespace graphics
	{
		class EGL2GraphicsData final : public GraphicsData
		{
			OctoonDeclareSubClass(EGL2GraphicsData, GraphicsData)
		public:
			EGL2GraphicsData() noexcept;
			~EGL2GraphicsData() noexcept;

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
			friend class EGL2Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			EGL2GraphicsData(const EGL2GraphicsData&) noexcept = delete;
			EGL2GraphicsData& operator=(const EGL2GraphicsData&) noexcept = delete;

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