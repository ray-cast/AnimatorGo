#include "gl20_device_property.h"
#include "gl20_types.h"

namespace octoon
{
	namespace graphics
	{
		GL20DeviceProperty::GL20DeviceProperty() noexcept
		{
		}

		GL20DeviceProperty::~GL20DeviceProperty() noexcept
		{
		}

		bool
		GL20DeviceProperty::initDeviceProperties() noexcept
		{
			static_assert(sizeof(GLint) == sizeof(std::uint32_t), "not match");

			GL20Types::setup();

			this->initTextureSupports();
			this->initVertexSupports();
			this->initTextureDimSupports();
			this->initShaderSupports();

			glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimension1D);
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimension2D);
			glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimensionCube);

			glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, (GLint*)&_deviceProperties.maxVertexInputAttributes);
			glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, (GLint*)&_deviceProperties.maxVertexInputBindings);
			glGetIntegerv(GL_MAX_VARYING_VECTORS, (GLint*)&_deviceProperties.maxVertexOutputComponents);

			glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, (GLint*)&_deviceProperties.maxFragmentInputComponents);

			glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.subPixelPrecisionBits);
			glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.subTexelPrecisionBits);
			glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.mipmapPrecisionBits);

			glGetIntegerv(GL_MAX_VIEWPORT_DIMS, (GLint*)&_deviceProperties.maxViewportDimensionsW);

			glGetIntegerv(GL_LINE_WIDTH, (GLint*)&_deviceProperties.strictLines);

			return true;
		}

		bool
		GL20DeviceProperty::initTextureSupports() noexcept
		{
			if (GL20Types::isSupportFeature(GL20Features::GL20_OES_required_internalformat))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R4G4UNormPack8);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R4G4B4A4UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B4G4R4A4UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B5G5R5A1UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B5G6R5UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R5G6B5UNormPack16);
			}

			if (GL20Types::isSupportFeature(GL20Features::GL20_EXT_read_format_bgra))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A1R5G5B5UNormPack16);
			}

			if (GL20Types::isSupportFeature(GL20Features::GL20_ARM_rgba8) || GL20Types::isSupportFeature(GL20Features::GL20_OES_rgb8_rgba8))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8A8UNorm);
			}

			if (GL20Types::isSupportFeature(GL20Features::GL20_EXT_texture_format_BGRA8888) || GL20Types::isSupportFeature(GL20Features::GL20_APPLE_texture_format_BGRA8888))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8A8UNorm);
			}

			if (GL20Types::isSupportFeature(GL20Features::GL20_EXT_sRGB))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8A8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8A8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A8B8G8R8SRGBPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A8B8G8R8SRGBPack32);
			}

			if (GL20Types::isSupportFeature(GL20Features::GL20_NV_sRGB_formats))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBSRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBASRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC3SRGBBlock);
			}

			if (GL20Types::isSupportFeature(GL20Features::GL20_EXT_texture_rg))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8UNorm);
			}

			if (GL20Types::isSupportFeature(GL20Features::GL20_EXT_texture_norm16))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16A16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16A16SNorm);
			}

			if (GL20Types::isSupportFeature(GL20Features::GL20_OES_texture_half_float) || GL20Types::isSupportFeature(GL20Features::GL20_EXT_color_buffer_half_float))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16A16SFloat);
			}

			if (GL20Types::isSupportFeature(GL20Features::GL20_OES_texture_float) || GL20Types::isSupportFeature(GL20Features::GL20_EXT_color_buffer_float))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32B32SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32B32A32SFloat);
			}

			if (GL20Types::isSupportFeature(GL20Features::GL20_EXT_texture_type_2_10_10_10_REV))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A2R10G10B10UNormPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A2B10G10R10UNormPack32);
			}

			if (GL20Types::isSupportFeature(GL20Features::GL20_APPLE_texture_packed_float))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B10G11R11UFloatPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::E5B9G9R9UFloatPack32);
			}

			if (GL20Types::isSupportFeature(GL20Features::GL20_OES_depth_texture))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::D16UNorm);
			}

			if (GL20Types::isSupportFeature(GL20Features::GL20_OES_depth24))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::X8_D24UNormPack32);
			}

			if (GL20Types::isSupportFeature(GL20Features::GL20_OES_depth32))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::D32_SFLOAT);
			}

			if (GL20Types::isSupportFeature(GL20Features::GL20_OES_texture_stencil8))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::S8UInt);
			}

			/*if (GL20Types::isSupportFeature(GL20Features::GL20_OES_packed_depth_stencil))
			{
			_deviceProperties.supportTextures.push_back(GraphicsFormat::D24UNorm_S8UInt);
			}*/

			if (GL20Types::isSupportFeature(GL20Features::GL20_EXT_texture_compression_dxt1))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBUNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBAUNormBlock);
			}

			if (GL20Types::isSupportFeature(GL20Features::GL20_EXT_texture_compression_s3tc))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC2UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC3UNormBlock);
			}

			if (GL20Types::isSupportFeature(GL20Features::GL20_KHR_texture_compression_astc_ldr))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC4x4UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC4x4SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC5x4UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC5x4SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC5x5UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC5x5SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC6x5UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC6x5SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC6x6UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC6x6SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC8x5UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC8x5SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC8x6UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC8x6SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC8x8UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC8x8SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x5UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x5SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x6UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x6SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x8UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x8SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x10UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x10SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC12x10UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC12x10SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC12x12UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC12x12SRGBBlock);
			}

			return true;
		}

		bool
		GL20DeviceProperty::initTextureDimSupports() noexcept
		{
			_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::Texture2D);
			_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::Texture2DArray);
			_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::Cube);

			if (GL20Types::isSupportFeature(GL20Features::GL20_EXT_texture_cube_map_array))
				_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::CubeArray);

			return true;
		}

		bool
		GL20DeviceProperty::initVertexSupports() noexcept
		{
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8UNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8SNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8SNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8UNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8B8SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8B8UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8B8SNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8B8UNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8B8A8SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8B8A8UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8B8A8SNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8B8A8UNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16UNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16SNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16SNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16UNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16B16SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16B16UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16B16SNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16B16UNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16B16A16SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16B16A16UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16B16A16SNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16B16A16UNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32G32SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32G32UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32G32B32SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32G32B32UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32G32B32A32SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32G32B32A32UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32SFloat);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32G32SFloat);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32G32B32SFloat);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32G32B32A32SFloat);

			if (GL20Types::isSupportFeature(GL20Features::GL20_OES_vertex_type_10_10_10_2))
			{
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::A2R10G10B10SIntPack32);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::A2R10G10B10UIntPack32);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::A2R10G10B10UNormPack32);
			}

			if (GL20Types::isSupportFeature(GL20Features::GL20_OES_vertex_half_float))
			{
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16SFloat);
			}

			return true;
		}

		bool
		GL20DeviceProperty::initShaderSupports() noexcept
		{
			_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::VertexBit);
			_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::FragmentBit);
			return true;
		}
	}
}