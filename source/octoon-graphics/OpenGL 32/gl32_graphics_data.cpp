#include "gl32_graphics_data.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(GL32GraphicsData, GraphicsData, "GL32GraphicsData")

		GL32GraphicsData::GL32GraphicsData() noexcept
			: _buffer(GL_NONE)
		{
		}

		GL32GraphicsData::~GL32GraphicsData() noexcept
		{
			this->close();
		}

		bool
		GL32GraphicsData::setup(const GraphicsDataDesc& desc) noexcept
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
			else if (type == GraphicsDataType::StorageTexelBuffer)
				_target = GL_TEXTURE_BUFFER;
			else if (type == GraphicsDataType::StorageBuffer)
				_target = GL_SHADER_STORAGE_BUFFER;
			else if (type == GraphicsDataType::StorageBuffer)
				_target = GL_DRAW_INDIRECT_BUFFER;
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
		GL32GraphicsData::close() noexcept
		{
			if (_buffer != GL_NONE)
			{
				glDeleteBuffers(1, &_buffer);
				_buffer = GL_NONE;
			}
		}

		int
		GL32GraphicsData::flush() noexcept
		{
			return this->flush(0, _desc.getStreamSize());
		}

		int
		GL32GraphicsData::flush(GLintptr offset, GLsizeiptr cnt) noexcept
		{
			glBindBuffer(_target, _buffer);
			glFlushMappedBufferRange(_buffer, offset, cnt);
			return cnt;
		}

		bool
		GL32GraphicsData::map(std::ptrdiff_t offset, std::ptrdiff_t count, void** data) noexcept
		{
			assert(data);
			glBindBuffer(_target, _buffer);
			*data = glMapBufferRange(_target, offset, count, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
			return *data ? true : false;
		}

		void
		GL32GraphicsData::unmap() noexcept
		{
			glBindBuffer(_target, _buffer);
			glUnmapBuffer(_target);
		}

		GLuint
		GL32GraphicsData::getInstanceID() const noexcept
		{
			return _buffer;
		}

		const GraphicsDataDesc&
		GL32GraphicsData::getDataDesc() const noexcept
		{
			return _desc;
		}

		void
		GL32GraphicsData::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL32GraphicsData::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}