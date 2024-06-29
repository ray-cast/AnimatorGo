#include "gl45_graphics_data.h"
#include "gl33_device.h"

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubInterface(GL45GraphicsData, GraphicsData, "GL45GraphicsData")

		GL45GraphicsData::GL45GraphicsData() noexcept
			: _buffer(GL_NONE)
			, _bufferAddr(GL_NONE)
			, _data(nullptr)
		{
		}

		GL45GraphicsData::~GL45GraphicsData() noexcept
		{
			this->close();
		}

		bool
		GL45GraphicsData::setup(const GraphicsDataDesc& desc) noexcept
		{
			assert(_buffer == GL_NONE);

			GLenum target;

			auto type = desc.getType();
			if (type == GraphicsDataType::StorageVertexBuffer)
				target = GL_ARRAY_BUFFER;
			else if (type == GraphicsDataType::StorageIndexBuffer)
				target = GL_ELEMENT_ARRAY_BUFFER;
			else if (type == GraphicsDataType::StorageTexelBuffer)
				target = GL_TEXTURE_BUFFER;
			else if (type == GraphicsDataType::StorageBuffer)
				target = GL_SHADER_STORAGE_BUFFER;
			else if (type == GraphicsDataType::UniformBuffer)
				target = GL_UNIFORM_BUFFER;
			else if (type == GraphicsDataType::IndirectBiffer)
				target = GL_DRAW_INDIRECT_BUFFER;
			else if (type == GraphicsDataType::TransferSrc || type == GraphicsDataType::TransferSrc)
				target = GL_TRANSFORM_FEEDBACK_BUFFER;
			else
			{
				this->getDevice()->downcast<GL33Device>()->message("Unknown data type.");
				return false;
			}

			glCreateBuffers(1, &_buffer);
			if (_buffer == GL_NONE)
			{
				this->getDevice()->downcast<GL33Device>()->message("glCreateBuffers() fail.");
				return false;
			}

			auto usage = desc.getUsage();

			GLbitfield flags = GL_MAP_READ_BIT;
			if (usage & GraphicsUsageFlagBits::ReadBit)
				flags |= GL_MAP_READ_BIT;
			if (usage & GraphicsUsageFlagBits::WriteBit)
				flags |= GL_MAP_WRITE_BIT;
			if (usage & GraphicsUsageFlagBits::PersistentBit)
				flags |= GL_MAP_PERSISTENT_BIT;
			if (usage & GraphicsUsageFlagBits::CoherentBit)
				flags |= GL_MAP_COHERENT_BIT;
			if (usage & GraphicsUsageFlagBits::FlushExplicitBit)
				flags |= GL_MAP_FLUSH_EXPLICIT_BIT;
			if (usage & GraphicsUsageFlagBits::DynamicStorageBit)
				flags |= GL_DYNAMIC_STORAGE_BIT;
			if (usage & GraphicsUsageFlagBits::ClientStorageBit)
				flags |= GL_CLIENT_STORAGE_BIT;

			glNamedBufferStorage(_buffer, desc.getStreamSize(), desc.getStream(), flags);

			if (GLEW_NV_vertex_buffer_unified_memory && type == GraphicsDataType::StorageVertexBuffer)
			{
				glGetNamedBufferParameterui64vNV(_buffer, GL_BUFFER_GPU_ADDRESS_NV, &_bufferAddr);
				glMakeNamedBufferResidentNV(_buffer, GL_READ_ONLY);
			}

			_desc = desc;
			return true;
		}

		void
		GL45GraphicsData::close() noexcept
		{
			if (_data)
				glUnmapNamedBuffer(_buffer);

			if (_buffer)
			{
				glDeleteBuffers(1, &_buffer);
				_buffer = 0;
			}
		}

		std::ptrdiff_t
		GL45GraphicsData::flush() noexcept
		{
			return this->flush(0, _desc.getStreamSize());
		}

		std::ptrdiff_t
		GL45GraphicsData::flush(GLintptr offset, GLsizeiptr cnt) noexcept
		{
			glFlushMappedNamedBufferRange(_buffer, offset, cnt);
			return cnt;
		}

		bool
		GL45GraphicsData::map(std::ptrdiff_t offset, std::ptrdiff_t count, void** data) noexcept
		{
			assert(data);

			GLbitfield flags = 0;

			auto usage = _desc.getUsage();
			if (usage & GraphicsUsageFlagBits::ReadBit)
				flags |= GL_MAP_READ_BIT;
			if (usage & GraphicsUsageFlagBits::WriteBit)
				flags |= GL_MAP_WRITE_BIT;
			if (usage & GraphicsUsageFlagBits::PersistentBit)
				flags |= GL_MAP_PERSISTENT_BIT;
			if (usage & GraphicsUsageFlagBits::CoherentBit)
				flags |= GL_MAP_COHERENT_BIT;
			if (usage & GraphicsUsageFlagBits::FlushExplicitBit)
				flags |= GL_MAP_FLUSH_EXPLICIT_BIT;

			if (!_data && usage & GraphicsUsageFlagBits::PersistentBit)
				_data = glMapNamedBuffer(_buffer, flags);

			if (_data && usage & GraphicsUsageFlagBits::PersistentBit)
			{
				*data = (std::uint8_t*)_data + offset;
				return true;
			}

			*data = _data = glMapNamedBufferRange(_buffer, offset, count, flags);
			return *data ? true : false;
		}

		void
		GL45GraphicsData::unmap() noexcept
		{
			auto usage = _desc.getUsage();
			if (!(usage & GraphicsUsageFlagBits::PersistentBit))
				glUnmapNamedBuffer(_buffer);
			_data = nullptr;
		}

		GLuint
		GL45GraphicsData::getInstanceID() const noexcept
		{
			return _buffer;
		}

		GLuint64
		GL45GraphicsData::getInstanceAddr() const noexcept
		{
			return _bufferAddr;
		}

		const GraphicsDataDesc&
		GL45GraphicsData::getDataDesc() const noexcept
		{
			return _desc;
		}

		void
		GL45GraphicsData::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL45GraphicsData::getDevice() const noexcept
		{
			return _device.lock();
		}
	}
}