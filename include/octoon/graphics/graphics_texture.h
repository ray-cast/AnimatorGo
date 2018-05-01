#ifndef OCTOON_GRAPHICS_TEXTURE_H_
#define OCTOON_GRAPHICS_TEXTURE_H_

#include <octoon/graphics/graphics_resource.h>

namespace octoon
{
	namespace graphics
	{
		class OCTOON_EXPORT GraphicsTextureDesc final
		{
		public:
			GraphicsTextureDesc() noexcept;
			~GraphicsTextureDesc() noexcept;

			void setWidth(std::uint32_t w) noexcept;
			void setHeight(std::uint32_t h) noexcept;
			void setDepth(std::uint32_t d) noexcept;
			void setSize(std::uint32_t w, std::uint32_t h, std::uint32_t depth = 0) noexcept;

			void setMipNums(std::uint32_t nums) noexcept;
			void setMipBase(std::uint32_t minLevel) noexcept;

			void setLayerNums(std::uint32_t layer) noexcept;
			void setLayerBase(std::uint32_t minLayer) noexcept;

			void setStream(const void* data) noexcept;
			void setStreamSize(std::size_t size) noexcept;

			void setTexFormat(GraphicsFormat format) noexcept;
			void setTexDim(GraphicsTextureDim mapping) noexcept;
			void setTexTiling(GraphicsImageTiling tiling) noexcept;
			void setTexUsage(GraphicsViewUsageFlags flags) noexcept;
			void setTexMultisample(std::uint32_t samples) noexcept;

			void setSamplerWrap(GraphicsSamplerWrap wrap) noexcept;
			void setSamplerMinFilter(GraphicsSamplerFilter filter) noexcept;
			void setSamplerMagFilter(GraphicsSamplerFilter filter) noexcept;
			void setSamplerFilter(GraphicsSamplerFilter minFilter, GraphicsSamplerFilter magFilter) noexcept;
			void setSamplerAnis(GraphicsSamplerAnis anis) noexcept;

			GraphicsFormat getTexFormat()  const noexcept;
			GraphicsTextureDim getTexDim() const noexcept;
			GraphicsImageTiling getTexTiling() const noexcept;
			GraphicsViewUsageFlags getTexUsage() const noexcept;

			GraphicsSamplerWrap getSamplerWrap() const noexcept;
			GraphicsSamplerFilter getSamplerMinFilter() const noexcept;
			GraphicsSamplerFilter getSamplerMagFilter() const noexcept;
			GraphicsSamplerAnis getSamplerAnis() const noexcept;

			std::uint32_t getWidth() const noexcept;
			std::uint32_t getHeight() const noexcept;
			std::uint32_t getDepth()  const noexcept;

			std::uint32_t getMipBase() const noexcept;
			std::uint32_t getMipNums() const noexcept;

			std::uint32_t getLayerBase() const noexcept;
			std::uint32_t getLayerNums() const noexcept;

			std::uint32_t getTexMultisample() const noexcept;

			const void* getStream() const noexcept;
			std::size_t getStreamSize() const noexcept;

		private:
			std::string _name;

			math::uint3 _size;

			std::uint32_t _layerBase;
			std::uint32_t _layer;

			std::uint32_t _mipLevel;
			std::uint32_t _mipBase;

			std::uint32_t _samples;

			GraphicsFormat _format;
			GraphicsTextureDim _dim;
			GraphicsSamplerFilter _filterMin;
			GraphicsSamplerFilter _filterMag;
			GraphicsSamplerWrap _wrap;
			GraphicsSamplerAnis _anis;
			GraphicsImageTiling _tiling;
			GraphicsViewUsageFlags _textureUsage;

			const void* _data;
			std::size_t _dataSize;
		};

		class OCTOON_EXPORT GraphicsTexture : public GraphicsResource
		{
			OctoonDeclareSubInterface(GraphicsTexture, GraphicsResource)
		public:
			GraphicsTexture() noexcept = default;
			virtual ~GraphicsTexture() = default;

			virtual bool map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint32_t mipLevel, void** data) noexcept = 0;
			virtual void unmap() noexcept = 0;

			virtual const GraphicsTextureDesc& getGraphicsTextureDesc() const noexcept = 0;

		private:
			GraphicsTexture(const GraphicsTexture&) noexcept = delete;
			GraphicsTexture& operator=(const GraphicsTexture&) noexcept = delete;
		};
	}
}

#endif