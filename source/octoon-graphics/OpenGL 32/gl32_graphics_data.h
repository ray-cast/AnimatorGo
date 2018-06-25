#ifndef OCTOON_GL32_GRAPHICS_DATA_H_
#define OCTOON_GL32_GRAPHICS_DATA_H_

#include "gl32_types.h"

namespace octoon
{
	namespace graphics
	{
		class GL32GraphicsData final : public GraphicsData
		{
			OctoonDeclareSubClass(GL32GraphicsData, GraphicsData)
		public:
			GL32GraphicsData() noexcept;
			virtual ~GL32GraphicsData() noexcept;

			bool setup(const GraphicsDataDesc& desc) noexcept;
			void close() noexcept;

			int flush() noexcept;
			int flush(GLintptr offset, GLsizeiptr cnt) noexcept;

			bool map(std::ptrdiff_t offset, std::ptrdiff_t count, void** data) noexcept;
			void unmap() noexcept;

			GLuint getInstanceID() const noexcept;

			const GraphicsDataDesc& getDataDesc() const noexcept override;

		private:
			friend class GL32Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL32GraphicsData(const GL32GraphicsData&) noexcept = delete;
			GL32GraphicsData& operator=(const GL32GraphicsData&) noexcept = delete;

		private:
			GLuint _buffer;
			GLenum _target;
			GraphicsDataDesc _desc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif