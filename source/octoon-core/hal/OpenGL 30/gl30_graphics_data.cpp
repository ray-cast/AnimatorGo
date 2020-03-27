#include "gl30_graphics_data.h"

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL30GraphicsData, GraphicsData, "GL30GraphicsData")

		GL30GraphicsData::GL30GraphicsData() noexcept
			: _buffer(GL_NONE)
		{
		}

		GL30GraphicsData::~GL30GraphicsData() noexcept
		{
			this->close();
		}

		bool
		GL30GraphicsData::setup(const GraphicsDataDesc& desc) noexcept
		{
			assert(!_buffer);
			assert(desc.getStreamSize() > 0);

			_desc = desc;

			auto type = desc.getType();
			if (type == GraphicsDataType::StorageVertexBuffer)
				_target = GL_ARRAY_BUFFER;
			else if (type == GraphicsDataType::StorageIndexBuffer)
				_target = GL_ELEMENT_ARRAY_BUFFER;
			else if (type == GraphicsDataType::UniformBuffer)
				_target = GL_UNIFORM_BUFFER;
			else
				return false;

			GLenum flags = GL_STATIC_DRAW;

			auto usage = desc.getUsage();
			if (usage & GraphicsUsageFlagBits::ReadBit)
				flags = GL_STATIC_DRAW;
			if (usage & GraphicsUsageFlagBits::WriteBit)
				flags = GL_DYNAMIC_READ;

			GL_CHECK(glGenBuffers(1, &_buffer));
			GL_CHECK(glBindBuffer(_target, _buffer));
			GL_CHECK(glBufferData(_target, desc.getStreamSize(), desc.getStream(), flags));

			return true;
		}

		void
		GL30GraphicsData::close() noexcept
		{
			if (_buffer != GL_NONE)
			{
				glDeleteBuffers(1, &_buffer);
				_buffer = GL_NONE;
			}
		}

		std::ptrdiff_t
		GL30GraphicsData::flush() noexcept
		{
			return this->flush(0, _desc.getStreamSize());
		}

		std::ptrdiff_t
		GL30GraphicsData::flush(GLintptr offset, GLsizeiptr cnt) noexcept
		{
			glBindBuffer(_target, _buffer);
			glFlushMappedBufferRange(_buffer, offset, cnt);
			return cnt;
		}

		bool
		GL30GraphicsData::map(std::ptrdiff_t offset, std::ptrdiff_t count, void** data) noexcept
		{
			assert(data);
			glBindBuffer(_target, _buffer);
			*data = glMapBufferRange(_target, offset, count, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
			return *data ? true : false;
		}

		void
		GL30GraphicsData::unmap() noexcept
		{
			glBindBuffer(_target, _buffer);
			glUnmapBuffer(_target);
		}

		GLuint
		GL30GraphicsData::getInstanceID() const noexcept
		{
			return _buffer;
		}

		const GraphicsDataDesc&
		GL30GraphicsData::getDataDesc() const noexcept
		{
			return _desc;
		}

		void
		GL30GraphicsData::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL30GraphicsData::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}