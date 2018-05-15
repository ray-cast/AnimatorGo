#include "gl20_graphics_data.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubInterface(GL20GraphicsData, GraphicsData, "GL20GraphicsData")

		GL20GraphicsData::GL20GraphicsData() noexcept
			: _buffer(GL_NONE)
			, _isMapping(GL_FALSE)
			, _mapOffset(0)
			, _mapCount(0)
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
			GL_CHECK(glBufferData(_target, desc.getStreamSize(), desc.getStream(), flags));

			if (desc.getStream())
			{
				_data.resize(desc.getStreamSize());
				memcpy(_data.data(), desc.getStream(), desc.getStreamSize());
			}

			_desc = desc;
			return true;
		}

		void
		GL20GraphicsData::close() noexcept
		{
			assert(!_isMapping);

			if (_isMapping)
				this->unmap();

			if (_buffer)
			{
				glDeleteBuffers(1, &_buffer);
				_buffer = 0;
			}
		}

		int
		GL20GraphicsData::flush() noexcept
		{
			return 0;
		}

		int
		GL20GraphicsData::flush(GLintptr offset, GLsizeiptr cnt) noexcept
		{
			return 0;
		}

		bool
		GL20GraphicsData::map(std::ptrdiff_t offset, std::ptrdiff_t count, void** data) noexcept
		{
			_data.resize(_desc.getStreamSize());
			_isMapping = true;
			_mapOffset = offset;
			_mapCount = count;

			*data = _data.data() + offset;
			return true;
		}

		void
		GL20GraphicsData::unmap() noexcept
		{
			if (_isMapping)
			{
				glBufferSubData(_target, _mapOffset, _mapCount, _data.data() + _mapOffset);
				_isMapping = false;
			}
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