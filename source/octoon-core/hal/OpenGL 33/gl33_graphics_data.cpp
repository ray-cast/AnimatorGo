#include "gl33_graphics_data.h"
#include "gl33_device.h"

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL33GraphicsData, GraphicsData, "GL33GraphicsData")

		GL33GraphicsData::GL33GraphicsData() noexcept
			: _buffer(GL_NONE)
			, _data(nullptr)
		{
		}

		GL33GraphicsData::~GL33GraphicsData() noexcept
		{
			this->close();
		}

		bool
		GL33GraphicsData::setup(const GraphicsDataDesc& desc) noexcept
		{
			assert(_buffer == GL_NONE);
			assert(desc.getStreamSize() > 0);

			_desc = desc;
			_data = nullptr;

			auto type = desc.getType();
			if (type == GraphicsDataType::StorageVertexBuffer)
				_target = GL_ARRAY_BUFFER;
			else if (type == GraphicsDataType::StorageIndexBuffer)
				_target = GL_ELEMENT_ARRAY_BUFFER;
			else if (type == GraphicsDataType::StorageTexelBuffer)
				_target = GL_TEXTURE_BUFFER;
			else if (type == GraphicsDataType::StorageBuffer)
				_target = GL_SHADER_STORAGE_BUFFER;
			else if (type == GraphicsDataType::UniformBuffer)
				_target = GL_UNIFORM_BUFFER;
			else if (type == GraphicsDataType::IndirectBiffer)
				_target = GL_DRAW_INDIRECT_BUFFER;
			else if (type == GraphicsDataType::TransferSrc || type == GraphicsDataType::TransferSrc)
				_target = GL_TRANSFORM_FEEDBACK_BUFFER;
			else
			{
				this->getDevice()->downcast<GL33Device>()->message("Unkown data type.");
				return false;
			}

			GLenum flags = GL_STATIC_DRAW;

			auto usage = desc.getUsage();
			if (usage & GraphicsUsageFlagBits::ReadBit)
				flags = GL_STATIC_DRAW;
			if (usage & GraphicsUsageFlagBits::WriteBit)
			{
				if (usage & GraphicsUsageFlagBits::ReadBit)
					flags = GL_DYNAMIC_READ;
				else
					flags = GL_DYNAMIC_DRAW;
			}

			glGenBuffers(1, &_buffer);
			glBindBuffer(_target, _buffer);
			glBufferData(_target, desc.getStreamSize(), desc.getStream(), flags);
			glBindBuffer(_target, 0);

			return true;
		}

		bool
		GL33GraphicsData::is_open() const noexcept
		{
			return _buffer == GL_NONE ? false : true;
		}

		void
		GL33GraphicsData::close() noexcept
		{
			assert(!_data);

			if (_data)
				this->unmap();

			if (_buffer)
			{
				glDeleteBuffers(1, &_buffer);
				_buffer = 0;
			}
		}

		bool
		GL33GraphicsData::map(std::ptrdiff_t offset, std::ptrdiff_t count, void** data) noexcept
		{
			assert(data);
			assert(!_data);

			glBindBuffer(_target, _buffer);

			GLbitfield flags = 0;

			auto usage = _desc.getUsage();
			if (usage & GraphicsUsageFlagBits::ReadBit)
				flags |= GL_MAP_READ_BIT;
			if (usage & GraphicsUsageFlagBits::WriteBit)
				flags |= GL_MAP_WRITE_BIT;

			_data = *data = glMapBufferRange(_target, offset, count, flags);
			return _data ? true : false;
		}

		void
		GL33GraphicsData::unmap() noexcept
		{
			if (_data)
			{
				glBindBuffer(_target, _buffer);
				glUnmapBuffer(_target);
				_data = nullptr;
			}
		}

		GLuint
		GL33GraphicsData::getInstanceID() const noexcept
		{
			return _buffer;
		}

		const GraphicsDataDesc&
		GL33GraphicsData::getDataDesc() const noexcept
		{
			return _desc;
		}

		void
		GL33GraphicsData::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL33GraphicsData::getDevice() const noexcept
		{
			return _device.lock();
		}
	}
}