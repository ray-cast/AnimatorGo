#include "ogl_graphics_data.h"
#include "ogl_device.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(OGLGraphicsData, GraphicsData, "OGLGraphicsData")

		OGLGraphicsData::OGLGraphicsData() noexcept
			: _buffer(GL_NONE)
			, _data(nullptr)
		{
		}

		OGLGraphicsData::~OGLGraphicsData() noexcept
		{
			this->close();
		}

		bool
		OGLGraphicsData::setup(const GraphicsDataDesc& desc) noexcept
		{
			assert(_buffer == GL_NONE);
			assert(desc.getStreamSize() > 0);

			_desc = desc;
			_data = nullptr;

			auto type = desc.getType();
			if (type == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer)
				_target = GL_ARRAY_BUFFER;
			else if (type == GraphicsDataType::GraphicsDataTypeStorageIndexBuffer)
				_target = GL_ELEMENT_ARRAY_BUFFER;
			else if (type == GraphicsDataType::GraphicsDataTypeStorageTexelBuffer)
				_target = GL_TEXTURE_BUFFER;
			else if (type == GraphicsDataType::GraphicsDataTypeStorageBuffer)
				_target = GL_SHADER_STORAGE_BUFFER;
			else if (type == GraphicsDataType::GraphicsDataTypeUniformBuffer)
				_target = GL_UNIFORM_BUFFER;
			else if (type == GraphicsDataType::GraphicsDataTypeIndirectBiffer)
				_target = GL_DRAW_INDIRECT_BUFFER;
			else if (type == GraphicsDataType::GraphicsDataTypeTransferSrc || type == GraphicsDataType::GraphicsDataTypeTransferSrc)
				_target = GL_TRANSFORM_FEEDBACK_BUFFER;
			else
			{
				this->getDevice()->downcast<OGLDevice>()->message("Unkown data type.");
				return false;
			}

			GLenum flags = GL_STATIC_DRAW;

			auto usage = desc.getUsage();
			if (usage & GraphicsUsageFlagBits::GraphicsUsageFlagReadBit)
				flags = GL_STATIC_DRAW;
			if (usage & GraphicsUsageFlagBits::GraphicsUsageFlagWriteBit)
			{
				if (usage & GraphicsUsageFlagBits::GraphicsUsageFlagReadBit)
					flags = GL_DYNAMIC_READ;
				else
					flags = GL_DYNAMIC_DRAW;
			}

			glGenBuffers(1, &_buffer);
			glBindBuffer(_target, _buffer);
			glBufferData(_target, desc.getStreamSize(), desc.getStream(), flags);

			return true;
		}

		bool
		OGLGraphicsData::is_open() const noexcept
		{
			return _buffer == GL_NONE ? false : true;
		}

		void
		OGLGraphicsData::close() noexcept
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
		OGLGraphicsData::map(std::ptrdiff_t offset, std::ptrdiff_t count, void** data) noexcept
		{
			assert(data);
			assert(!_data);

			glBindBuffer(_target, _buffer);

			GLbitfield flags = 0;

			auto usage = _desc.getUsage();
			if (usage & GraphicsUsageFlagBits::GraphicsUsageFlagReadBit)
				flags |= GL_MAP_READ_BIT;
			if (usage & GraphicsUsageFlagBits::GraphicsUsageFlagWriteBit)
				flags |= GL_MAP_WRITE_BIT;

			_data = *data = glMapBufferRange(_target, offset, count, flags);
			return _data ? true : false;
		}

		void
		OGLGraphicsData::unmap() noexcept
		{
			if (_data)
			{
				glBindBuffer(_target, _buffer);
				glUnmapBuffer(_target);
				_data = nullptr;
			}
		}

		GLuint
		OGLGraphicsData::getInstanceID() const noexcept
		{
			return _buffer;
		}

		const GraphicsDataDesc&
		OGLGraphicsData::getGraphicsDataDesc() const noexcept
		{
			return _desc;
		}

		void
		OGLGraphicsData::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		OGLGraphicsData::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}