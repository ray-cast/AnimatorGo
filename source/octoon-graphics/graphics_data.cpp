#include <octoon/graphics/graphics_data.h>

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubInterface(GraphicsData, GraphicsResource, "GraphicsData")

		GraphicsDataDesc::GraphicsDataDesc() noexcept
			: _data(nullptr)
			, _dataSize(0)
			, _usage(GraphicsUsageFlagBits::GraphicsUsageFlagReadBit)
			, _type(GraphicsDataType::None)
		{
		}

		GraphicsDataDesc::GraphicsDataDesc(GraphicsDataType type, GraphicsUsageFlags usage, const void* data, std::size_t size) noexcept
			: _data((std::uint8_t*)data)
			, _dataSize(size)
			, _usage(usage)
			, _type(type)
		{
		}

		GraphicsDataDesc::~GraphicsDataDesc() noexcept
		{
		}

		void
		GraphicsDataDesc::setUsage(GraphicsUsageFlags usage) noexcept
		{
			_usage = usage;
		}

		std::uint32_t
		GraphicsDataDesc::getUsage() const noexcept
		{
			return _usage;
		}

		void
		GraphicsDataDesc::setType(GraphicsDataType type) noexcept
		{
			_type = type;
		}

		GraphicsDataType
		GraphicsDataDesc::getType() const noexcept
		{
			return _type;
		}

		void
		GraphicsDataDesc::setStream(std::uint8_t* data) noexcept
		{
			_data = data;
		}

		std::uint8_t*
		GraphicsDataDesc::getStream() const noexcept
		{
			return _data;
		}

		void
		GraphicsDataDesc::setStreamSize(std::size_t size) noexcept
		{
			_dataSize = size;
		}

		std::size_t
		GraphicsDataDesc::getStreamSize() const noexcept
		{
			return _dataSize;
		}

		GraphicsData::GraphicsData() noexcept
		{
		}

		GraphicsData::~GraphicsData() noexcept
		{
		}
	}
}