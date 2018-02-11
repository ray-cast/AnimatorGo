#include <octoon/graphics/graphics_texture.h>

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubInterface(GraphicsTexture, GraphicsResource, "GraphicsTexture")

		GraphicsTextureDesc::GraphicsTextureDesc() noexcept
			: _size(0, 0, 1)
			, _multisample(false)
			, _layerBase(0)
			, _layer(1)
			, _mipBase(0)
			, _mipLevel(1)
			, _format(GraphicsFormat::Undefined)
			, _dim(GraphicsTextureDim::Texture2D)
			, _filterMin(GraphicsSamplerFilter::Linear)
			, _filterMag(GraphicsSamplerFilter::Linear)
			, _wrap(GraphicsSamplerWrap::Repeat)
			, _anis(GraphicsSamplerAnis::Anis0)
			, _textureUsage(GraphicsViewUsageFlagBits::GraphicsViewUsageFlagBitsSampledBit)
			, _tiling(GraphicsImageTiling::Optimal)
			, _data(nullptr)
			, _dataSize(0)
		{
		}

		GraphicsTextureDesc::~GraphicsTextureDesc() noexcept
		{
		}

		void
		GraphicsTextureDesc::setWidth(std::uint32_t w) noexcept
		{
			_size.x = w;
		}

		void
		GraphicsTextureDesc::setHeight(std::uint32_t h) noexcept
		{
			_size.y = h;
		}

		void
		GraphicsTextureDesc::setDepth(std::uint32_t d) noexcept
		{
			_size.z = d;
		}

		void
		GraphicsTextureDesc::setSize(std::uint32_t w, std::uint32_t h, std::uint32_t depth) noexcept
		{
			_size.x = w;
			_size.y = h;
			_size.z = depth;
		}

		std::uint32_t
		GraphicsTextureDesc::getWidth() const noexcept
		{
			return _size.x;
		}

		std::uint32_t
		GraphicsTextureDesc::getHeight() const noexcept
		{
			return _size.y;
		}

		std::uint32_t
		GraphicsTextureDesc::getDepth() const noexcept
		{
			return _size.z;
		}

		void
		GraphicsTextureDesc::setTexFormat(GraphicsFormat format) noexcept
		{
			_format = format;
		}

		void
		GraphicsTextureDesc::setTexDim(GraphicsTextureDim dim) noexcept
		{
			_dim = dim;
		}

		void
		GraphicsTextureDesc::setTexTiling(GraphicsImageTiling tiling) noexcept
		{
			_tiling = tiling;
		}

		void
		GraphicsTextureDesc::setTexUsage(GraphicsViewUsageFlags flags) noexcept
		{
			_textureUsage = flags;
		}

		GraphicsFormat
		GraphicsTextureDesc::getTexFormat() const noexcept
		{
			return _format;
		}

		GraphicsTextureDim
		GraphicsTextureDesc::getTexDim() const noexcept
		{
			return _dim;
		}

		GraphicsViewUsageFlags
		GraphicsTextureDesc::getTexUsage() const noexcept
		{
			return _textureUsage;
		}

		GraphicsImageTiling
		GraphicsTextureDesc::getTexTiling() const noexcept
		{
			return _tiling;
		}

		void
		GraphicsTextureDesc::setSamplerWrap(GraphicsSamplerWrap wrap) noexcept
		{
			_wrap = wrap;
		}

		void
		GraphicsTextureDesc::setSamplerMinFilter(GraphicsSamplerFilter filter) noexcept
		{
			_filterMin = filter;
		}

		void
		GraphicsTextureDesc::setSamplerMagFilter(GraphicsSamplerFilter filter) noexcept
		{
			_filterMag = filter;
		}

		void
		GraphicsTextureDesc::setSamplerFilter(GraphicsSamplerFilter minFilter, GraphicsSamplerFilter magFilter) noexcept
		{
			_filterMin = minFilter;
			_filterMag = magFilter;
		}

		void
		GraphicsTextureDesc::setSamplerAnis(GraphicsSamplerAnis anis) noexcept
		{
			_anis = anis;
		}

		GraphicsSamplerWrap
		GraphicsTextureDesc::getSamplerWrap() const noexcept
		{
			return _wrap;
		}

		GraphicsSamplerFilter
		GraphicsTextureDesc::getSamplerMinFilter() const noexcept
		{
			return _filterMin;
		}

		GraphicsSamplerFilter
		GraphicsTextureDesc::getSamplerMagFilter() const noexcept
		{
			return _filterMag;
		}

		GraphicsSamplerAnis
		GraphicsTextureDesc::getSamplerAnis() const noexcept
		{
			return _anis;
		}

		void
		GraphicsTextureDesc::setMultisample(bool enable) noexcept
		{
			_multisample = enable;
		}

		bool
		GraphicsTextureDesc::isMultiSample() const noexcept
		{
			return _multisample;
		}

		void
		GraphicsTextureDesc::setLayerNums(std::uint32_t layer) noexcept
		{
			_layer = layer;
		}

		std::uint32_t
		GraphicsTextureDesc::getLayerNums() const noexcept
		{
			return _layer;
		}

		void
		GraphicsTextureDesc::setLayerBase(std::uint32_t minLayer) noexcept
		{
			_layerBase = minLayer;
		}

		std::uint32_t
		GraphicsTextureDesc::getLayerBase() const noexcept
		{
			return _layerBase;
		}

		void
		GraphicsTextureDesc::setMipNums(std::uint32_t nums) noexcept
		{
			_mipLevel = nums;
		}

		void
		GraphicsTextureDesc::setMipBase(std::uint32_t level) noexcept
		{
			_mipBase = level;
		}

		std::uint32_t
		GraphicsTextureDesc::getMipNums() const noexcept
		{
			return _mipLevel;
		}

		std::uint32_t
		GraphicsTextureDesc::getMipBase() const noexcept
		{
			return _mipBase;
		}

		void
		GraphicsTextureDesc::setStream(const void* data) noexcept
		{
			_data = data;
		}

		void
		GraphicsTextureDesc::setStreamSize(std::size_t size) noexcept
		{
			_dataSize = size;
		}

		const void*
		GraphicsTextureDesc::getStream() const noexcept
		{
			return _data;
		}

		std::size_t
		GraphicsTextureDesc::getStreamSize() const noexcept
		{
			return _dataSize;
		}
	}
}