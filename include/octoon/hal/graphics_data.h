#ifndef OCTOON_GRAPHICS_DATA_H_
#define OCTOON_GRAPHICS_DATA_H_

#include <octoon/hal/graphics_resource.h>

namespace octoon
{
	namespace hal
	{
		class OCTOON_EXPORT GraphicsDataDesc final
		{
		public:
			GraphicsDataDesc() noexcept;
			GraphicsDataDesc(GraphicsDataType type, GraphicsUsageFlags usage, const void* data, std::size_t size) noexcept;
			~GraphicsDataDesc() noexcept;

			void setType(GraphicsDataType type) noexcept;
			GraphicsDataType getType() const noexcept;

			void setUsage(GraphicsUsageFlags usage) noexcept;
			GraphicsUsageFlags getUsage() const noexcept;

			void setStream(std::uint8_t* data) noexcept;
			std::uint8_t* getStream() const noexcept;

			void setStreamSize(std::size_t size) noexcept;
			std::size_t getStreamSize() const noexcept;

		private:
			std::uint8_t* _data;
			std::size_t _dataSize;
			GraphicsUsageFlags _usage;
			GraphicsDataType _type;
		};

		class OCTOON_EXPORT GraphicsData : public GraphicsResource
		{
			OctoonDeclareSubInterface(GraphicsData, GraphicsResource)
		public:
			GraphicsData() noexcept;
			virtual ~GraphicsData() noexcept;

			virtual bool map(std::ptrdiff_t offset, std::ptrdiff_t count, void** data) noexcept = 0;
			virtual void unmap() noexcept = 0;

			virtual const GraphicsDataDesc& getDataDesc() const noexcept = 0;

		private:
			GraphicsData(const GraphicsData&) = delete;
			GraphicsData& operator=(const GraphicsData&) = delete;
		};
	}
}

#endif