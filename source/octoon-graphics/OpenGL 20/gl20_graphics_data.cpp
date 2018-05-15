#include "gl20_graphics_data.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubInterface(GL20GraphicsData, GraphicsData, "GL20GraphicsData")

		GL20GraphicsData::GL20GraphicsData() noexcept
			: _buffer(GL_NONE)
			, _isMapping(GL_FALSE)
		{
		}

		GL20GraphicsData::~GL20GraphicsData() noexcept
		{
			this->close();
		}

		bool
		GL20GraphicsData::setup(const GraphicsDataDesc& desc) noexcept
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
		GL20GraphicsData::close() noexcept
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
		GL20GraphicsData::flush() noexcept
		{
			return this->flush(0, _dataSize);
		}

		int
		GL20GraphicsData::flush(GLintptr offset, GLsizeiptr cnt) noexcept
		{
			return 0;
		}

		bool
		GL20GraphicsData::map(std::ptrdiff_t offset, std::ptrdiff_t count, void** data) noexcept
		{
#if !defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN)
			assert(data);
			glBindBuffer(_target, _buffer);
			*data = glMapBufferRange(_target, offset, count, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
			return *data ? true : false;
#else
			return false;
#endif
		}

		void
		GL20GraphicsData::unmap() noexcept
		{
#if !defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN)
			glBindBuffer(_target, _buffer);
			glUnmapBuffer(_target);
#endif
		}

		GLuint
		GL20GraphicsData::getInstanceID() const noexcept
		{
			return _buffer;
		}

		void
		GL20GraphicsData::bind() noexcept
		{
			glBindBuffer(_target, _buffer);
		}

		const GraphicsDataDesc&
		GL20GraphicsData::getGraphicsDataDesc() const noexcept
		{
			return _desc;
		}

		void
		GL20GraphicsData::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL20GraphicsData::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}