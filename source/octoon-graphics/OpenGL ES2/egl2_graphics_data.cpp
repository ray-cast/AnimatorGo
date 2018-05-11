#include "egl2_graphics_data.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubInterface(EGL2GraphicsData, GraphicsData, "EGL2GraphicsData")

		EGL2GraphicsData::EGL2GraphicsData() noexcept
			: _buffer(GL_NONE)
			, _isMapping(GL_FALSE)
		{
		}

		EGL2GraphicsData::~EGL2GraphicsData() noexcept
		{
			this->close();
		}

		bool
		EGL2GraphicsData::setup(const GraphicsDataDesc& desc) noexcept
		{
			assert(!_buffer);
			assert(desc.getStreamSize() > 0);

			_dataOffset = 0;
			_dataSize = desc.getStreamSize();
			_usage = desc.getUsage();
			_desc = desc;
			_isMapping = GL_FALSE;

			auto type = desc.getType();
			if (type == GraphicsDataType::StorageVertexBuffer)
				_target = GL_ARRAY_BUFFER;
			else if (type == GraphicsDataType::StorageIndexBuffer)
				_target = GL_ELEMENT_ARRAY_BUFFER;
			else
				return false;

			GLenum flags = GL_STATIC_DRAW;

			auto usage = desc.getUsage();
			if (usage & GraphicsUsageFlagBits::ReadBit)
				flags = GL_STATIC_DRAW;
			if (usage & GraphicsUsageFlagBits::WriteBit)
				flags = GL_DYNAMIC_DRAW;

			GL_CHECK(glGenBuffers(1, &_buffer));
			GL_CHECK(glBindBuffer(_target, _buffer));
			GL_CHECK(glBufferData(_target, _dataSize, desc.getStream(), flags));

			return true;
		}

		void
		EGL2GraphicsData::close() noexcept
		{
			assert(!_isMapping);

			if (_isMapping)
			{
				this->unmap();
			}

			if (_buffer)
			{
				glDeleteBuffers(1, &_buffer);
				_buffer = 0;
			}
		}

		int
		EGL2GraphicsData::flush() noexcept
		{
			return this->flush(0, _dataSize);
		}

		int
		EGL2GraphicsData::flush(GLintptr offset, GLsizeiptr cnt) noexcept
		{
			return 0;
		}

		bool
		EGL2GraphicsData::map(std::intptr_t offset, std::intptr_t count, void** data) noexcept
		{
			return false;
		}

		void
		EGL2GraphicsData::unmap() noexcept
		{
		}

		GLuint
		EGL2GraphicsData::getInstanceID() const noexcept
		{
			return _buffer;
		}

		void
		EGL2GraphicsData::bind() noexcept
		{
			glBindBuffer(_target, _buffer);
		}

		const GraphicsDataDesc&
		EGL2GraphicsData::getGraphicsDataDesc() const noexcept
		{
			return _desc;
		}

		void
		EGL2GraphicsData::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		EGL2GraphicsData::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}