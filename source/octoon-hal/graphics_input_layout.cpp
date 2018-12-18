#include <octoon/hal/graphics_input_layout.h>

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubInterface(GraphicsInputLayout, GraphicsChild, "GraphicsInputLayoutDesc")

		GraphicsVertexBinding::GraphicsVertexBinding() noexcept
			: _divisor(GraphicsVertexDivisor::Vertex)
		{
		}

		GraphicsVertexBinding::GraphicsVertexBinding(std::uint8_t slot, std::uint16_t size) noexcept
			: _slot(slot)
			, _size(size)
			, _divisor(GraphicsVertexDivisor::Vertex)
		{
		}

		GraphicsVertexBinding::GraphicsVertexBinding(std::uint8_t slot, std::uint16_t size, GraphicsVertexDivisor divisor) noexcept
			: _slot(slot)
			, _size(size)
			, _divisor(divisor)
		{
		}

		GraphicsVertexBinding::~GraphicsVertexBinding() noexcept
		{
		}

		void
		GraphicsVertexBinding::setVertexSlot(std::uint8_t slot) noexcept
		{
			_slot = slot;
		}

		std::uint8_t
		GraphicsVertexBinding::getVertexSlot() const noexcept
		{
			return _slot;
		}

		void
		GraphicsVertexBinding::setVertexSize(std::uint16_t size) noexcept
		{
			_size = size;
		}

		std::uint16_t
		GraphicsVertexBinding::getVertexSize() const noexcept
		{
			return _size;
		}

		void
		GraphicsVertexBinding::setVertexDivisor(GraphicsVertexDivisor divisor) noexcept
		{
			_divisor = divisor;
		}

		GraphicsVertexDivisor
		GraphicsVertexBinding::getVertexDivisor() const noexcept
		{
			return _divisor;
		}

		GraphicsVertexLayout::GraphicsVertexLayout() noexcept
			: _index(0)
			, _count(0)
			, _size(0)
			, _offset(0)
			, _slot(0)
			, _format(GraphicsFormat::Undefined)
		{
		}

		GraphicsVertexLayout::GraphicsVertexLayout(std::uint8_t slot, std::string&& semantic, std::uint8_t semanticIndex, GraphicsFormat format, std::uint16_t offset) noexcept
			: _name(std::move(semantic))
			, _index(semanticIndex)
			, _offset(offset)
			, _format(format)
			, _slot(slot)
		{
			_count = getVertexCount(format);
			_size = getVertexSize(format);
		}

		GraphicsVertexLayout::GraphicsVertexLayout(std::uint8_t slot, const std::string& semantic, std::uint8_t semanticIndex, GraphicsFormat format, std::uint16_t offset) noexcept
			: _name(semantic)
			, _index(semanticIndex)
			, _offset(offset)
			, _format(format)
			, _slot(slot)
		{
			_count = getVertexCount(format);
			_size = getVertexSize(format);
		}

		GraphicsVertexLayout::~GraphicsVertexLayout() noexcept
		{
		}

		void
		GraphicsVertexLayout::setSemantic(const std::string& name) noexcept
		{
			_name = name;
		}

		const std::string&
		GraphicsVertexLayout::getSemantic() const noexcept
		{
			return _name;
		}

		void
		GraphicsVertexLayout::setSemanticIndex(std::uint8_t index) noexcept
		{
			_index = index;
		}

		std::uint8_t
		GraphicsVertexLayout::getSemanticIndex() const noexcept
		{
			return _index;
		}

		void
		GraphicsVertexLayout::setVertexFormat(GraphicsFormat format) noexcept
		{
			if (_format != format)
			{
				_count = getVertexCount(format);
				_size = getVertexSize(format);

				_format = format;
			}
		}

		GraphicsFormat
		GraphicsVertexLayout::getVertexFormat() const noexcept
		{
			return _format;
		}

		void
		GraphicsVertexLayout::setVertexOffset(std::uint16_t offset) noexcept
		{
			_offset = offset;
		}

		std::uint16_t
		GraphicsVertexLayout::getVertexOffset() const noexcept
		{
			return _offset;
		}

		void
		GraphicsVertexLayout::setVertexSlot(std::uint8_t slot) noexcept
		{
			_slot = slot;
		}

		std::uint8_t
		GraphicsVertexLayout::getVertexSlot() const noexcept
		{
			return _slot;
		}

		std::uint8_t
		GraphicsVertexLayout::getVertexCount() const noexcept
		{
			return _count;
		}

		std::uint8_t
		GraphicsVertexLayout::getVertexSize() const noexcept
		{
			return _size;
		}

		std::uint8_t
		GraphicsVertexLayout::getVertexCount(GraphicsFormat format) noexcept
		{
			switch (format)
			{
			case GraphicsFormat::R8UNorm:
			case GraphicsFormat::R8SNorm:
			case GraphicsFormat::R8UScaled:
			case GraphicsFormat::R8SScaled:
			case GraphicsFormat::R8UInt:
			case GraphicsFormat::R8SInt:
			case GraphicsFormat::R8SRGB:
			case GraphicsFormat::R16UNorm:
			case GraphicsFormat::R16SNorm:
			case GraphicsFormat::R16UScaled:
			case GraphicsFormat::R16SScaled:
			case GraphicsFormat::R16UInt:
			case GraphicsFormat::R16SInt:
			case GraphicsFormat::R16SFloat:
			case GraphicsFormat::D16UNorm:
			case GraphicsFormat::R32UInt:
			case GraphicsFormat::R32SInt:
			case GraphicsFormat::R32SFloat:
			case GraphicsFormat::R64UInt:
			case GraphicsFormat::R64SInt:
			case GraphicsFormat::R64SFloat:
			case GraphicsFormat::S8UInt:
			case GraphicsFormat::X8_D24UNormPack32:
			case GraphicsFormat::D32_SFLOAT:
				return 1;
			case GraphicsFormat::R4G4UNormPack8:
			case GraphicsFormat::R8G8SInt:
			case GraphicsFormat::R8G8UInt:
			case GraphicsFormat::R8G8UNorm:
			case GraphicsFormat::R8G8SNorm:
			case GraphicsFormat::R8G8UScaled:
			case GraphicsFormat::R8G8SScaled:
			case GraphicsFormat::R8G8SRGB:
			case GraphicsFormat::R16G16SInt:
			case GraphicsFormat::R16G16UInt:
			case GraphicsFormat::R16G16SNorm:
			case GraphicsFormat::R16G16UNorm:
			case GraphicsFormat::R16G16UScaled:
			case GraphicsFormat::R16G16SScaled:
			case GraphicsFormat::R16G16SFloat:
			case GraphicsFormat::R32G32SInt:
			case GraphicsFormat::R32G32UInt:
			case GraphicsFormat::R32G32SFloat:
			case GraphicsFormat::R64G64UInt:
			case GraphicsFormat::R64G64SInt:
			case GraphicsFormat::R64G64SFloat:
			case GraphicsFormat::D16UNorm_S8UInt:
			case GraphicsFormat::D24UNorm_S8UInt:
			case GraphicsFormat::D32_SFLOAT_S8UInt:
				return 2;
			case GraphicsFormat::R5G6B5UNormPack16:
			case GraphicsFormat::B5G6R5UNormPack16:
			case GraphicsFormat::B10G11R11UFloatPack32:
			case GraphicsFormat::E5B9G9R9UFloatPack32:
			case GraphicsFormat::R8G8B8SInt:
			case GraphicsFormat::R8G8B8UInt:
			case GraphicsFormat::R8G8B8UNorm:
			case GraphicsFormat::R8G8B8SNorm:
			case GraphicsFormat::R8G8B8UScaled:
			case GraphicsFormat::R8G8B8SScaled:
			case GraphicsFormat::R8G8B8SRGB:
			case GraphicsFormat::R16G16B16SInt:
			case GraphicsFormat::R16G16B16UInt:
			case GraphicsFormat::R16G16B16SNorm:
			case GraphicsFormat::R16G16B16UNorm:
			case GraphicsFormat::R16G16B16UScaled:
			case GraphicsFormat::R16G16B16SScaled:
			case GraphicsFormat::R16G16B16SFloat:
			case GraphicsFormat::R32G32B32SInt:
			case GraphicsFormat::R32G32B32UInt:
			case GraphicsFormat::R32G32B32SFloat:
			case GraphicsFormat::B8G8R8UNorm:
			case GraphicsFormat::B8G8R8SNorm:
			case GraphicsFormat::B8G8R8UScaled:
			case GraphicsFormat::B8G8R8SScaled:
			case GraphicsFormat::B8G8R8UInt:
			case GraphicsFormat::B8G8R8SInt:
			case GraphicsFormat::B8G8R8SRGB:
			case GraphicsFormat::R64G64B64UInt:
			case GraphicsFormat::R64G64B64SInt:
			case GraphicsFormat::R64G64B64SFloat:
				return 3;
			case GraphicsFormat::R4G4B4A4UNormPack16:
			case GraphicsFormat::B4G4R4A4UNormPack16:
			case GraphicsFormat::R5G5B5A1UNormPack16:
			case GraphicsFormat::B5G5R5A1UNormPack16:
			case GraphicsFormat::A1R5G5B5UNormPack16:
			case GraphicsFormat::R8G8B8A8UNorm:
			case GraphicsFormat::R8G8B8A8SNorm:
			case GraphicsFormat::R8G8B8A8UScaled:
			case GraphicsFormat::R8G8B8A8SScaled:
			case GraphicsFormat::R8G8B8A8UInt:
			case GraphicsFormat::R8G8B8A8SInt:
			case GraphicsFormat::R8G8B8A8SRGB:
			case GraphicsFormat::B8G8R8A8UNorm:
			case GraphicsFormat::B8G8R8A8SNorm:
			case GraphicsFormat::B8G8R8A8UScaled:
			case GraphicsFormat::B8G8R8A8SScaled:
			case GraphicsFormat::B8G8R8A8UInt:
			case GraphicsFormat::B8G8R8A8SInt:
			case GraphicsFormat::B8G8R8A8SRGB:
			case GraphicsFormat::A8B8G8R8UNormPack32:
			case GraphicsFormat::A8B8G8R8SNormPack32:
			case GraphicsFormat::A8B8G8R8UScaledPack32:
			case GraphicsFormat::A8B8G8R8SScaledPack32:
			case GraphicsFormat::A8B8G8R8UIntPack32:
			case GraphicsFormat::A8B8G8R8SIntPack32:
			case GraphicsFormat::A8B8G8R8SRGBPack32:
			case GraphicsFormat::A2R10G10B10UNormPack32:
			case GraphicsFormat::A2R10G10B10SNormPack32:
			case GraphicsFormat::A2R10G10B10UScaledPack32:
			case GraphicsFormat::A2R10G10B10SScaledPack32:
			case GraphicsFormat::A2R10G10B10UIntPack32:
			case GraphicsFormat::A2R10G10B10SIntPack32:
			case GraphicsFormat::A2B10G10R10UNormPack32:
			case GraphicsFormat::A2B10G10R10SNormPack32:
			case GraphicsFormat::A2B10G10R10UScaledPack32:
			case GraphicsFormat::A2B10G10R10SScaledPack32:
			case GraphicsFormat::A2B10G10R10UIntPack32:
			case GraphicsFormat::A2B10G10R10SIntPack32:
			case GraphicsFormat::R16G16B16A16SNorm:
			case GraphicsFormat::R16G16B16A16UNorm:
			case GraphicsFormat::R16G16B16A16SScaled:
			case GraphicsFormat::R16G16B16A16UScaled:
			case GraphicsFormat::R16G16B16A16SFloat:
			case GraphicsFormat::R16G16B16A16SInt:
			case GraphicsFormat::R16G16B16A16UInt:
			case GraphicsFormat::R32G32B32A32SFloat:
			case GraphicsFormat::R32G32B32A32SInt:
			case GraphicsFormat::R32G32B32A32UInt:
			case GraphicsFormat::R64G64B64A64UInt:
			case GraphicsFormat::R64G64B64A64SInt:
			case GraphicsFormat::R64G64B64A64SFloat:
				return 4;
			case GraphicsFormat::BC1RGBUNormBlock:
			case GraphicsFormat::BC1RGBSRGBBlock:
			case GraphicsFormat::BC1RGBAUNormBlock:
			case GraphicsFormat::BC1RGBASRGBBlock:
			case GraphicsFormat::BC2UNormBlock:
			case GraphicsFormat::BC2SRGBBlock:
			case GraphicsFormat::BC3UNormBlock:
			case GraphicsFormat::BC3SRGBBlock:
			case GraphicsFormat::BC4UNormBlock:
			case GraphicsFormat::BC4SNormBlock:
			case GraphicsFormat::BC5UNormBlock:
			case GraphicsFormat::BC5SNormBlock:
			case GraphicsFormat::BC6HUFloatBlock:
			case GraphicsFormat::BC6HSFloatBlock:
			case GraphicsFormat::BC7UNormBlock:
			case GraphicsFormat::BC7SRGBBlock:
			case GraphicsFormat::ETC2R8G8B8UNormBlock:
			case GraphicsFormat::ETC2R8G8B8SRGBBlock:
			case GraphicsFormat::ETC2R8G8B8A1UNormBlock:
			case GraphicsFormat::ETC2R8G8B8A1SRGBBlock:
			case GraphicsFormat::ETC2R8G8B8A8UNormBlock:
			case GraphicsFormat::ETC2R8G8B8A8SRGBBlock:
			case GraphicsFormat::EACR11UNormBlock:
			case GraphicsFormat::EACR11SNormBlock:
			case GraphicsFormat::EACR11G11UNormBlock:
			case GraphicsFormat::EACR11G11SNormBlock:
			case GraphicsFormat::ASTC4x4UNormBlock:
			case GraphicsFormat::ASTC4x4SRGBBlock:
			case GraphicsFormat::ASTC5x4UNormBlock:
			case GraphicsFormat::ASTC5x4SRGBBlock:
			case GraphicsFormat::ASTC5x5UNormBlock:
			case GraphicsFormat::ASTC5x5SRGBBlock:
			case GraphicsFormat::ASTC6x5UNormBlock:
			case GraphicsFormat::ASTC6x5SRGBBlock:
			case GraphicsFormat::ASTC6x6UNormBlock:
			case GraphicsFormat::ASTC6x6SRGBBlock:
			case GraphicsFormat::ASTC8x5UNormBlock:
			case GraphicsFormat::ASTC8x5SRGBBlock:
			case GraphicsFormat::ASTC8x6UNormBlock:
			case GraphicsFormat::ASTC8x6SRGBBlock:
			case GraphicsFormat::ASTC8x8UNormBlock:
			case GraphicsFormat::ASTC8x8SRGBBlock:
			case GraphicsFormat::ASTC10x5UNormBlock:
			case GraphicsFormat::ASTC10x5SRGBBlock:
			case GraphicsFormat::ASTC10x6UNormBlock:
			case GraphicsFormat::ASTC10x6SRGBBlock:
			case GraphicsFormat::ASTC10x8UNormBlock:
			case GraphicsFormat::ASTC10x8SRGBBlock:
			case GraphicsFormat::ASTC10x10UNormBlock:
			case GraphicsFormat::ASTC10x10SRGBBlock:
			case GraphicsFormat::ASTC12x10UNormBlock:
			case GraphicsFormat::ASTC12x10SRGBBlock:
			case GraphicsFormat::ASTC12x12UNormBlock:
			case GraphicsFormat::ASTC12x12SRGBBlock:
			default:
				assert(false);
				return 0;
			}
		}

		std::uint8_t
		GraphicsVertexLayout::getVertexSize(GraphicsFormat format) noexcept
		{
			switch (format)
			{
			case GraphicsFormat::R4G4UNormPack8:
			case GraphicsFormat::R8UNorm:
			case GraphicsFormat::R8SNorm:
			case GraphicsFormat::R8UScaled:
			case GraphicsFormat::R8SScaled:
			case GraphicsFormat::R8UInt:
			case GraphicsFormat::R8SInt:
			case GraphicsFormat::R8SRGB:
			case GraphicsFormat::S8UInt:
				return 1;
			case GraphicsFormat::R4G4B4A4UNormPack16:
			case GraphicsFormat::B4G4R4A4UNormPack16:
			case GraphicsFormat::R5G6B5UNormPack16:
			case GraphicsFormat::B5G6R5UNormPack16:
			case GraphicsFormat::R5G5B5A1UNormPack16:
			case GraphicsFormat::B5G5R5A1UNormPack16:
			case GraphicsFormat::A1R5G5B5UNormPack16:
			case GraphicsFormat::R8G8UNorm:
			case GraphicsFormat::R8G8SNorm:
			case GraphicsFormat::R8G8UScaled:
			case GraphicsFormat::R8G8SScaled:
			case GraphicsFormat::R8G8UInt:
			case GraphicsFormat::R8G8SInt:
			case GraphicsFormat::R8G8SRGB:
			case GraphicsFormat::R16UNorm:
			case GraphicsFormat::R16SNorm:
			case GraphicsFormat::R16UScaled:
			case GraphicsFormat::R16SScaled:
			case GraphicsFormat::R16UInt:
			case GraphicsFormat::R16SInt:
			case GraphicsFormat::R16SFloat:
			case GraphicsFormat::D16UNorm:
				return 2;
			case GraphicsFormat::R8G8B8UNorm:
			case GraphicsFormat::R8G8B8SNorm:
			case GraphicsFormat::R8G8B8UScaled:
			case GraphicsFormat::R8G8B8SScaled:
			case GraphicsFormat::R8G8B8UInt:
			case GraphicsFormat::R8G8B8SInt:
			case GraphicsFormat::R8G8B8SRGB:
			case GraphicsFormat::B8G8R8UNorm:
			case GraphicsFormat::B8G8R8SNorm:
			case GraphicsFormat::B8G8R8UScaled:
			case GraphicsFormat::B8G8R8SScaled:
			case GraphicsFormat::B8G8R8UInt:
			case GraphicsFormat::B8G8R8SInt:
			case GraphicsFormat::B8G8R8SRGB:
				return 3;
			case GraphicsFormat::R8G8B8A8UNorm:
			case GraphicsFormat::R8G8B8A8SNorm:
			case GraphicsFormat::R8G8B8A8UScaled:
			case GraphicsFormat::R8G8B8A8SScaled:
			case GraphicsFormat::R8G8B8A8UInt:
			case GraphicsFormat::R8G8B8A8SInt:
			case GraphicsFormat::R8G8B8A8SRGB:
			case GraphicsFormat::B8G8R8A8UNorm:
			case GraphicsFormat::B8G8R8A8SNorm:
			case GraphicsFormat::B8G8R8A8UScaled:
			case GraphicsFormat::B8G8R8A8SScaled:
			case GraphicsFormat::B8G8R8A8UInt:
			case GraphicsFormat::B8G8R8A8SInt:
			case GraphicsFormat::B8G8R8A8SRGB:
			case GraphicsFormat::A8B8G8R8UNormPack32:
			case GraphicsFormat::A8B8G8R8SNormPack32:
			case GraphicsFormat::A8B8G8R8UScaledPack32:
			case GraphicsFormat::A8B8G8R8SScaledPack32:
			case GraphicsFormat::A8B8G8R8UIntPack32:
			case GraphicsFormat::A8B8G8R8SIntPack32:
			case GraphicsFormat::A8B8G8R8SRGBPack32:
			case GraphicsFormat::R16G16SInt:
			case GraphicsFormat::R16G16UInt:
			case GraphicsFormat::R16G16SNorm:
			case GraphicsFormat::R16G16UNorm:
			case GraphicsFormat::R16G16UScaled:
			case GraphicsFormat::R16G16SScaled:
			case GraphicsFormat::R16G16SFloat:
			case GraphicsFormat::R32UInt:
			case GraphicsFormat::R32SInt:
			case GraphicsFormat::R32SFloat:
			case GraphicsFormat::A2R10G10B10UNormPack32:
			case GraphicsFormat::A2R10G10B10SNormPack32:
			case GraphicsFormat::A2R10G10B10UScaledPack32:
			case GraphicsFormat::A2R10G10B10SScaledPack32:
			case GraphicsFormat::A2R10G10B10UIntPack32:
			case GraphicsFormat::A2R10G10B10SIntPack32:
			case GraphicsFormat::A2B10G10R10UNormPack32:
			case GraphicsFormat::A2B10G10R10SNormPack32:
			case GraphicsFormat::A2B10G10R10UScaledPack32:
			case GraphicsFormat::A2B10G10R10SScaledPack32:
			case GraphicsFormat::A2B10G10R10UIntPack32:
			case GraphicsFormat::A2B10G10R10SIntPack32:
			case GraphicsFormat::X8_D24UNormPack32:
			case GraphicsFormat::B10G11R11UFloatPack32:
			case GraphicsFormat::E5B9G9R9UFloatPack32:
			case GraphicsFormat::D32_SFLOAT:
			case GraphicsFormat::D16UNorm_S8UInt:
			case GraphicsFormat::D24UNorm_S8UInt:
			case GraphicsFormat::D32_SFLOAT_S8UInt:
				return 4;
			case GraphicsFormat::R16G16B16SInt:
			case GraphicsFormat::R16G16B16UInt:
			case GraphicsFormat::R16G16B16SNorm:
			case GraphicsFormat::R16G16B16UNorm:
			case GraphicsFormat::R16G16B16UScaled:
			case GraphicsFormat::R16G16B16SScaled:
			case GraphicsFormat::R16G16B16SFloat:
				return 6;
			case GraphicsFormat::R32G32SFloat:
			case GraphicsFormat::R32G32SInt:
			case GraphicsFormat::R32G32UInt:
			case GraphicsFormat::R16G16B16A16SNorm:
			case GraphicsFormat::R16G16B16A16UNorm:
			case GraphicsFormat::R16G16B16A16SScaled:
			case GraphicsFormat::R16G16B16A16UScaled:
			case GraphicsFormat::R16G16B16A16SFloat:
			case GraphicsFormat::R16G16B16A16SInt:
			case GraphicsFormat::R16G16B16A16UInt:
			case GraphicsFormat::R64UInt:
			case GraphicsFormat::R64SInt:
			case GraphicsFormat::R64SFloat:
				return 8;
			case GraphicsFormat::R32G32B32SFloat:
			case GraphicsFormat::R32G32B32SInt:
			case GraphicsFormat::R32G32B32UInt:
				return 12;
			case GraphicsFormat::R32G32B32A32SFloat:
			case GraphicsFormat::R32G32B32A32SInt:
			case GraphicsFormat::R32G32B32A32UInt:
			case GraphicsFormat::R64G64UInt:
			case GraphicsFormat::R64G64SInt:
			case GraphicsFormat::R64G64SFloat:
				return 16;
			case GraphicsFormat::R64G64B64UInt:
			case GraphicsFormat::R64G64B64SInt:
			case GraphicsFormat::R64G64B64SFloat:
				return 24;
			case GraphicsFormat::R64G64B64A64UInt:
			case GraphicsFormat::R64G64B64A64SInt:
			case GraphicsFormat::R64G64B64A64SFloat:
				return 32;
			case GraphicsFormat::BC1RGBUNormBlock:
			case GraphicsFormat::BC1RGBSRGBBlock:
			case GraphicsFormat::BC1RGBAUNormBlock:
			case GraphicsFormat::BC1RGBASRGBBlock:
			case GraphicsFormat::BC2UNormBlock:
			case GraphicsFormat::BC2SRGBBlock:
			case GraphicsFormat::BC3UNormBlock:
			case GraphicsFormat::BC3SRGBBlock:
			case GraphicsFormat::BC4UNormBlock:
			case GraphicsFormat::BC4SNormBlock:
			case GraphicsFormat::BC5UNormBlock:
			case GraphicsFormat::BC5SNormBlock:
			case GraphicsFormat::BC6HUFloatBlock:
			case GraphicsFormat::BC6HSFloatBlock:
			case GraphicsFormat::BC7UNormBlock:
			case GraphicsFormat::BC7SRGBBlock:
			case GraphicsFormat::ETC2R8G8B8UNormBlock:
			case GraphicsFormat::ETC2R8G8B8SRGBBlock:
			case GraphicsFormat::ETC2R8G8B8A1UNormBlock:
			case GraphicsFormat::ETC2R8G8B8A1SRGBBlock:
			case GraphicsFormat::ETC2R8G8B8A8UNormBlock:
			case GraphicsFormat::ETC2R8G8B8A8SRGBBlock:
			case GraphicsFormat::EACR11UNormBlock:
			case GraphicsFormat::EACR11SNormBlock:
			case GraphicsFormat::EACR11G11UNormBlock:
			case GraphicsFormat::EACR11G11SNormBlock:
			case GraphicsFormat::ASTC4x4UNormBlock:
			case GraphicsFormat::ASTC4x4SRGBBlock:
			case GraphicsFormat::ASTC5x4UNormBlock:
			case GraphicsFormat::ASTC5x4SRGBBlock:
			case GraphicsFormat::ASTC5x5UNormBlock:
			case GraphicsFormat::ASTC5x5SRGBBlock:
			case GraphicsFormat::ASTC6x5UNormBlock:
			case GraphicsFormat::ASTC6x5SRGBBlock:
			case GraphicsFormat::ASTC6x6UNormBlock:
			case GraphicsFormat::ASTC6x6SRGBBlock:
			case GraphicsFormat::ASTC8x5UNormBlock:
			case GraphicsFormat::ASTC8x5SRGBBlock:
			case GraphicsFormat::ASTC8x6UNormBlock:
			case GraphicsFormat::ASTC8x6SRGBBlock:
			case GraphicsFormat::ASTC8x8UNormBlock:
			case GraphicsFormat::ASTC8x8SRGBBlock:
			case GraphicsFormat::ASTC10x5UNormBlock:
			case GraphicsFormat::ASTC10x5SRGBBlock:
			case GraphicsFormat::ASTC10x6UNormBlock:
			case GraphicsFormat::ASTC10x6SRGBBlock:
			case GraphicsFormat::ASTC10x8UNormBlock:
			case GraphicsFormat::ASTC10x8SRGBBlock:
			case GraphicsFormat::ASTC10x10UNormBlock:
			case GraphicsFormat::ASTC10x10SRGBBlock:
			case GraphicsFormat::ASTC12x10UNormBlock:
			case GraphicsFormat::ASTC12x10SRGBBlock:
			case GraphicsFormat::ASTC12x12UNormBlock:
			case GraphicsFormat::ASTC12x12SRGBBlock:
			default:
				assert(false);
				return 0;
			}
		}

		void
		GraphicsInputLayoutDesc::addVertexLayout(const GraphicsVertexLayout& layout) noexcept
		{
			assert(!layout.getSemantic().empty());

			auto it = std::find_if(_layouts.begin(), _layouts.end(), [layout](const GraphicsVertexLayout& it) { return it.getSemantic() == layout.getSemantic() && it.getSemanticIndex() == layout.getSemanticIndex();});
			if (it == _layouts.end())
				_layouts.push_back(layout);
			else
			{
				assert(false);
			}
		}

		void
		GraphicsInputLayoutDesc::addVertexBinding(const GraphicsVertexBinding& binding) noexcept
		{
			auto it = std::find_if(_bindings.begin(), _bindings.end(), [binding](const GraphicsVertexBinding& it) { return it.getVertexSlot() == binding.getVertexSlot();});
			if (it == _bindings.end())
				_bindings.push_back(binding);
			else
			{
				assert(false);
			}
		}

		void
		GraphicsInputLayoutDesc::setVertexLayouts(const GraphicsVertexLayouts& _layout) noexcept
		{
			_layouts = _layout;
		}

		const GraphicsVertexLayouts&
		GraphicsInputLayoutDesc::getVertexLayouts() const noexcept
		{
			return _layouts;
		}

		void
		GraphicsInputLayoutDesc::setVertexBindings(const GraphicsVertexBindings& binding) noexcept
		{
			_bindings = binding;
		}

		const GraphicsVertexBindings&
		GraphicsInputLayoutDesc::getVertexBindings() const noexcept
		{
			return _bindings;
		}

		std::uint16_t
		GraphicsInputLayoutDesc::getVertexSize(std::uint8_t slot) const noexcept
		{
			std::uint16_t size = 0;
			for (auto& it : _layouts)
			{
				if (it.getVertexSlot() == slot)
				{
					size += it.getVertexOffset();
					size += it.getVertexSize();
				}
			}

			return size;
		}
	}
}