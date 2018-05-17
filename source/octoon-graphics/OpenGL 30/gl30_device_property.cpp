#include "gl30_device_property.h"
#include "gl30_types.h"

namespace octoon
{
	namespace graphics
	{
		GL30DeviceProperty::GL30DeviceProperty() noexcept
		{
		}

		GL30DeviceProperty::~GL30DeviceProperty() noexcept
		{
		}

		bool
		GL30DeviceProperty::initDeviceProperties() noexcept
		{
			static_assert(sizeof(GLint) == sizeof(std::uint32_t), "not match");

			GL30Types::setup();

			this->initTextureSupports();
			this->initVertexSupports();
			this->initTextureDimSupports();
			this->initShaderSupports();

			glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimension1D);
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimension2D);
			glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimension3D);
			glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimensionCube);

			glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, (GLint*)&_deviceProperties.maxImageArrayLayers);
			glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, (GLint*)&_deviceProperties.maxUniformBufferRange);

			glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, (GLint*)&_deviceProperties.maxPerStageDescriptorUniformBuffers);
			glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, (GLint*)&_deviceProperties.maxPerStageDescriptorInputAttachments);
			glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, (GLint*)&_deviceProperties.maxPerStageResources);

			glGetIntegerv(GL_MAX_SAMPLES, (GLint*)&_deviceProperties.maxDescriptorSetSamplers);
			glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, (GLint*)&_deviceProperties.maxDescriptorSetUniformBuffers);
			glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, (GLint*)&_deviceProperties.maxDescriptorSetUniformBuffersDynamic);

			glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, (GLint*)&_deviceProperties.maxVertexInputAttributes);
			glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, (GLint*)&_deviceProperties.maxVertexOutputComponents);

			glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, (GLint*)&_deviceProperties.maxFragmentInputComponents);
			glGetIntegerv(GL_MAX_DRAW_BUFFERS, (GLint*)&_deviceProperties.maxFragmentOutputAttachments);
			glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.subPixelPrecisionBits);
			glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.subTexelPrecisionBits);
			glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.mipmapPrecisionBits);
			glGetIntegerv(GL_MAX_ELEMENT_INDEX, (GLint*)&_deviceProperties.maxDrawIndexedIndexValue);
			glGetIntegerv(GL_MAX_ELEMENTS_INDICES, (GLint*)&_deviceProperties.maxDrawIndirectCount);

			glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &_deviceProperties.maxSamplerLodBias);
			glGetIntegerv(GL_MAX_VIEWPORT_DIMS, (GLint*)&_deviceProperties.maxViewportDimensionsW);
			glGetInteger64v(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, (GLint64*)&_deviceProperties.minUniformBufferOffsetAlignment);
			glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, (GLint*)&_deviceProperties.minTexelOffset);
			glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, (GLint*)&_deviceProperties.maxTexelOffset);
			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, (GLint*)&_deviceProperties.maxFramebufferColorAttachments);

			glGetIntegerv(GL_LINE_WIDTH, (GLint*)&_deviceProperties.strictLines);

			return true;
		}

		bool
		GL30DeviceProperty::initTextureSupports() noexcept
		{
			if (GL30Types::isSupportFeature(GL30Features::GL30_OES_required_internalformat))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R4G4UNormPack8);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R4G4B4A4UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B4G4R4A4UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B5G5R5A1UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B5G6R5UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R5G6B5UNormPack16);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_EXT_read_format_bgra))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A1R5G5B5UNormPack16);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_ARM_rgba8) || GL30Types::isSupportFeature(GL30Features::GL30_OES_rgb8_rgba8))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8A8UNorm);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_EXT_texture_format_BGRA8888) || GL30Types::isSupportFeature(GL30Features::GL30_APPLE_texture_format_BGRA8888))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8A8UNorm);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_EXT_sRGB))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8A8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8A8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A8B8G8R8SRGBPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A8B8G8R8SRGBPack32);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_NV_sRGB_formats))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBSRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBASRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC3SRGBBlock);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_EXT_texture_rg))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16UInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16SInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16UInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16SInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32SInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32UInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32SInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32UInt);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_EXT_texture_norm16))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16A16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16A16SNorm);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_OES_texture_half_float) || GL30Types::isSupportFeature(GL30Features::GL30_EXT_color_buffer_half_float))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16A16SFloat);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_OES_texture_float) || GL30Types::isSupportFeature(GL30Features::GL30_EXT_color_buffer_float))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32B32SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32B32A32SFloat);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_EXT_texture_type_2_10_10_10_REV))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A2R10G10B10UNormPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A2B10G10R10UNormPack32);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_APPLE_texture_packed_float))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B10G11R11UFloatPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::E5B9G9R9UFloatPack32);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_OES_depth_texture))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::D16UNorm);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_OES_depth24))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::X8_D24UNormPack32);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_OES_depth32))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::D32_SFLOAT);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_OES_texture_stencil8))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::S8UInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::D16UNorm_S8UInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::D24UNorm_S8UInt);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_EXT_texture_compression_dxt1))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBUNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBAUNormBlock);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_EXT_texture_compression_s3tc))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC2UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC3UNormBlock);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_KHR_texture_compression_astc_ldr))
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
		GL30DeviceProperty::initTextureDimSupports() noexcept
		{
			_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::Texture2D);
			_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::Texture2DArray);
			_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::Cube);

			if (GL30Types::isSupportFeature(GL30Features::GL30_EXT_texture_cube_map_array))
				_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::CubeArray);

			if (GL30Types::isSupportFeature(GL30Features::GL30_OES_texture_3D))
				_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::Texture3D);

			return true;
		}

		bool
		GL30DeviceProperty::initVertexSupports() noexcept
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

			if (GL30Types::isSupportFeature(GL30Features::GL30_OES_vertex_type_10_10_10_2))
			{
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::A2R10G10B10SIntPack32);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::A2R10G10B10UIntPack32);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::A2R10G10B10UNormPack32);
			}

			if (GL30Types::isSupportFeature(GL30Features::GL30_OES_vertex_half_float))
			{
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16SFloat);
			}

			return true;
		}

		bool
		GL30DeviceProperty::initShaderSupports() noexcept
		{
			_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::VertexBit);
			_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::FragmentBit);
			_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::ComputeBit);

			if (GL30Types::isSupportFeature(GL30Features::GL30_EXT_geometry_shader))
				_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::GeometryBit);

			return true;
		}
	}
}