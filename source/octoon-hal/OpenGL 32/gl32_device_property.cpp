#include "gl32_device_property.h"
#include "gl32_types.h"

namespace octoon
{
	namespace graphics
	{
		GL32DeviceProperty::GL32DeviceProperty() noexcept
		{
		}

		GL32DeviceProperty::~GL32DeviceProperty() noexcept
		{
		}

		bool
		GL32DeviceProperty::initDeviceProperties() noexcept
		{
			static_assert(sizeof(GLint) == sizeof(std::uint32_t), "not match");

			GL32Types::setup();

			this->initTextureSupports();
			this->initVertexSupports();
			this->initTextureDimSupports();
			this->initShaderSupports();

			glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimension1D);
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimension2D);
			glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimension3D);
			glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimensionCube);

			glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, (GLint*)&_deviceProperties.maxImageArrayLayers);
			glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, (GLint*)&_deviceProperties.maxTexelBufferElements);
			glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, (GLint*)&_deviceProperties.maxUniformBufferRange);
			glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, (GLint*)&_deviceProperties.maxStorageBufferRange);

			glGetIntegerv(GL_MAX_FRAGMENT_IMAGE_UNIFORMS, (GLint*)&_deviceProperties.maxPerStageDescriptorSamplers);
			glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, (GLint*)&_deviceProperties.maxPerStageDescriptorUniformBuffers);
			glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, (GLint*)&_deviceProperties.maxPerStageDescriptorStorageBuffers);
			glGetIntegerv(GL_MAX_FRAGMENT_IMAGE_UNIFORMS, (GLint*)&_deviceProperties.maxPerStageDescriptorSampledImages);
			glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, (GLint*)&_deviceProperties.maxPerStageDescriptorInputAttachments);
			glGetIntegerv(GL_MAX_FRAGMENT_IMAGE_UNIFORMS, (GLint*)&_deviceProperties.maxPerStageDescriptorStorageImages);
			glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, (GLint*)&_deviceProperties.maxPerStageResources);

			glGetIntegerv(GL_MAX_SAMPLES, (GLint*)&_deviceProperties.maxDescriptorSetSamplers);
			glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, (GLint*)&_deviceProperties.maxDescriptorSetUniformBuffers);
			glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, (GLint*)&_deviceProperties.maxDescriptorSetUniformBuffersDynamic);
			glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, (GLint*)&_deviceProperties.maxDescriptorSetStorageBuffers);
			glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, (GLint*)&_deviceProperties.maxDescriptorSetStorageBuffersDynamic);
			glGetIntegerv(GL_MAX_IMAGE_UNITS, (GLint*)&_deviceProperties.maxDescriptorSetSampledImages);

			glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, (GLint*)&_deviceProperties.maxVertexInputAttributes);
			glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, (GLint*)&_deviceProperties.maxVertexInputBindings);
			glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, (GLint*)&_deviceProperties.maxVertexInputAttributeOffset);
			glGetIntegerv(GL_MAX_VERTEX_ATTRIB_STRIDE, (GLint*)&_deviceProperties.maxVertexInputBindingStride);
			glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, (GLint*)&_deviceProperties.maxVertexOutputComponents);

			if (GL32Types::isSupportFeature(GL32Features::GL32_EXT_tessellation_shader))
			{
				glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, (GLint*)&_deviceProperties.maxTessellationGenerationLevel);
				glGetIntegerv(GL_MAX_PATCH_VERTICES, (GLint*)&_deviceProperties.maxTessellationPatchSize);
				glGetIntegerv(GL_MAX_TESS_CONTROL_INPUT_COMPONENTS, (GLint*)&_deviceProperties.maxTessellationControlPerVertexInputComponents);
				glGetIntegerv(GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS, (GLint*)&_deviceProperties.maxTessellationControlPerVertexOutputComponents);
				glGetIntegerv(GL_MAX_TESS_PATCH_COMPONENTS, (GLint*)&_deviceProperties.maxTessellationControlPerPatchOutputComponents);
				glGetIntegerv(GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS, (GLint*)&_deviceProperties.maxTessellationControlTotalOutputComponents);
				glGetIntegerv(GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS, (GLint*)&_deviceProperties.maxTessellationEvaluationInputComponents);
				glGetIntegerv(GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS, (GLint*)&_deviceProperties.maxTessellationEvaluationOutputComponents);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_EXT_geometry_shader))
			{
				glGetIntegerv(GL_MAX_GEOMETRY_SHADER_INVOCATIONS, (GLint*)&_deviceProperties.maxGeometryShaderInvocations);
				glGetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS, (GLint*)&_deviceProperties.maxGeometryInputComponents);
				glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, (GLint*)&_deviceProperties.maxGeometryOutputComponents);
				glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, (GLint*)&_deviceProperties.maxGeometryOutputVertices);
				glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, (GLint*)&_deviceProperties.maxGeometryTotalOutputComponents);
			}

			glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, (GLint*)&_deviceProperties.maxComputeSharedMemorySize);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, (GLint*)&_deviceProperties.maxComputeWorkGroupCount[0]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, (GLint*)&_deviceProperties.maxComputeWorkGroupCount[1]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, (GLint*)&_deviceProperties.maxComputeWorkGroupCount[2]);
			glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, (GLint*)&_deviceProperties.maxComputeWorkGroupInvocations);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, (GLint*)&_deviceProperties.maxComputeWorkGroupSize[0]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, (GLint*)&_deviceProperties.maxComputeWorkGroupSize[1]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, (GLint*)&_deviceProperties.maxComputeWorkGroupSize[2]);

			glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, (GLint*)&_deviceProperties.maxFragmentInputComponents);
			glGetIntegerv(GL_MAX_DRAW_BUFFERS, (GLint*)&_deviceProperties.maxFragmentOutputAttachments);
			glGetIntegerv(GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES, (GLint*)&_deviceProperties.maxFragmentCombinedOutputResources);
			glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.subPixelPrecisionBits);
			glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.subTexelPrecisionBits);
			glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.mipmapPrecisionBits);
			glGetIntegerv(GL_MAX_ELEMENT_INDEX, (GLint*)&_deviceProperties.maxDrawIndexedIndexValue);
			glGetIntegerv(GL_MAX_ELEMENTS_INDICES, (GLint*)&_deviceProperties.maxDrawIndirectCount);

			glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &_deviceProperties.maxSamplerLodBias);
			glGetIntegerv(GL_MAX_VIEWPORT_DIMS, (GLint*)&_deviceProperties.maxViewportDimensionsW);
			glGetInteger64v(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, (GLint64*)&_deviceProperties.minUniformBufferOffsetAlignment);
			glGetInteger64v(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, (GLint64*)&_deviceProperties.minStorageBufferOffsetAlignment);
			glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, (GLint*)&_deviceProperties.minTexelOffset);
			glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, (GLint*)&_deviceProperties.maxTexelOffset);
			glGetIntegerv(GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET, (GLint*)&_deviceProperties.minTexelGatherOffset);
			glGetIntegerv(GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET, (GLint*)&_deviceProperties.minTexelGatherOffset);
			glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, (GLint*)&_deviceProperties.maxFramebufferWidth);
			glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, (GLint*)&_deviceProperties.maxFramebufferHeight);
			glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, (GLint*)&_deviceProperties.maxFramebufferLayers);
			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, (GLint*)&_deviceProperties.maxFramebufferColorAttachments);
			glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, (GLint*)&_deviceProperties.framebufferColorSampleCounts);
			glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, (GLint*)&_deviceProperties.framebufferDepthSampleCounts);
			glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, (GLint*)&_deviceProperties.framebufferStencilSampleCounts);
			glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, (GLint*)&_deviceProperties.framebufferNoAttachmentsSampleCounts);
			glGetIntegerv(GL_MAX_INTEGER_SAMPLES, (GLint*)&_deviceProperties.sampledImageIntegerSampleCounts);
			glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, (GLint*)&_deviceProperties.sampledImageColorSampleCounts);
			glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, (GLint*)&_deviceProperties.sampledImageDepthSampleCounts);
			glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, (GLint*)&_deviceProperties.sampledImageStencilSampleCounts);
			glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, (GLint*)&_deviceProperties.storageImageSampleCounts);
			glGetIntegerv(GL_MAX_SAMPLE_MASK_WORDS, (GLint*)&_deviceProperties.maxSampleMaskWords);

			glGetIntegerv(GL_LINE_WIDTH, (GLint*)&_deviceProperties.strictLines);

			return true;
		}

		bool
		GL32DeviceProperty::initTextureSupports() noexcept
		{
			if (GL32Types::isSupportFeature(GL32Features::GL32_OES_required_internalformat))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R4G4UNormPack8);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R4G4B4A4UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B4G4R4A4UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B5G5R5A1UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B5G6R5UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R5G6B5UNormPack16);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_EXT_read_format_bgra))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A1R5G5B5UNormPack16);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_ARM_rgba8) || GL32Types::isSupportFeature(GL32Features::GL32_OES_rgb8_rgba8))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8A8UNorm);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_EXT_texture_format_BGRA8888) || GL32Types::isSupportFeature(GL32Features::GL32_APPLE_texture_format_BGRA8888))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8A8UNorm);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_EXT_sRGB))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8A8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8A8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A8B8G8R8SRGBPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A8B8G8R8SRGBPack32);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_NV_sRGB_formats))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBSRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBASRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC3SRGBBlock);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_EXT_texture_rg))
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

			if (GL32Types::isSupportFeature(GL32Features::GL32_EXT_texture_norm16))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16A16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16A16SNorm);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_OES_texture_half_float) || GL32Types::isSupportFeature(GL32Features::GL32_EXT_color_buffer_half_float))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16A16SFloat);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_OES_texture_float) || GL32Types::isSupportFeature(GL32Features::GL32_EXT_color_buffer_float))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32B32SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32B32A32SFloat);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_EXT_texture_type_2_10_10_10_REV))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A2R10G10B10UNormPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A2B10G10R10UNormPack32);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_APPLE_texture_packed_float))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B10G11R11UFloatPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::E5B9G9R9UFloatPack32);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_OES_depth_texture))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::D16UNorm);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_OES_depth24))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::X8_D24UNormPack32);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_OES_depth32))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::D32_SFLOAT);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_OES_texture_stencil8))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::S8UInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::D16UNorm_S8UInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::D24UNorm_S8UInt);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_EXT_texture_compression_dxt1))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBUNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBAUNormBlock);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_EXT_texture_compression_s3tc))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC2UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC3UNormBlock);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_KHR_texture_compression_astc_ldr))
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
		GL32DeviceProperty::initTextureDimSupports() noexcept
		{
			_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::Texture2D);
			_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::Texture2DArray);
			_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::Cube);

			if (GL32Types::isSupportFeature(GL32Features::GL32_EXT_texture_cube_map_array))
				_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::CubeArray);

			if (GL32Types::isSupportFeature(GL32Features::GL32_OES_texture_3D))
				_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::Texture3D);

			return true;
		}

		bool
		GL32DeviceProperty::initVertexSupports() noexcept
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

			if (GL32Types::isSupportFeature(GL32Features::GL32_OES_vertex_type_10_10_10_2))
			{
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::A2R10G10B10SIntPack32);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::A2R10G10B10UIntPack32);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::A2R10G10B10UNormPack32);
			}

			if (GL32Types::isSupportFeature(GL32Features::GL32_OES_vertex_half_float))
			{
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16SFloat);
			}

			return true;
		}

		bool
		GL32DeviceProperty::initShaderSupports() noexcept
		{
			_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::VertexBit);
			_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::FragmentBit);
			_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::ComputeBit);

			if (GL32Types::isSupportFeature(GL32Features::GL32_EXT_geometry_shader))
				_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::GeometryBit);

			return true;
		}
	}
}