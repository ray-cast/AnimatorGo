#include "gl33_device_property.h"
#include "gl33_device.h"

#include <octoon/runtime/except.h>

namespace octoon
{
	namespace hal
	{
		GL33DeviceProperty::GL33DeviceProperty() noexcept
		{
		}

		GL33DeviceProperty::~GL33DeviceProperty() noexcept
		{
		}

		bool
		GL33DeviceProperty::initDeviceProperties() noexcept
		{
			static_assert(sizeof(GLint) == sizeof(std::uint32_t), "not match");

			this->initTextureSupports();
			this->initVertexSupports();
			this->initTextureDimSupports();
			this->initShaderSupports();

			_deviceProperties.graphicsDeviceVendor = (const char*)glGetString(GL_VENDOR);
			_deviceProperties.graphicsDeviceName = (const char*)glGetString(GL_RENDERER);
			_deviceProperties.graphicsDeviceVersion = (const char*)glGetString(GL_VERSION);
			_deviceProperties.graphicsShaderLevel = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

			glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimension1D);
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimension2D);
			glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimension3D);
			glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxCubemapSize);
			glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, (GLint*)&_deviceProperties.maxTextureSize);

			glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, (GLint*)&_deviceProperties.maxImageArrayLayers);
			glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, (GLint*)&_deviceProperties.maxUniformBufferRange);
			glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, (GLint*)&_deviceProperties.maxStorageBufferRange);

			glGetIntegerv(GL_MAX_FRAGMENT_IMAGE_UNIFORMS, (GLint*)&_deviceProperties.maxPerStageDescriptorSamplers);
			glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, (GLint*)&_deviceProperties.maxPerStageDescriptorUniformBuffers);
			glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, (GLint*)&_deviceProperties.maxPerStageDescriptorStorageBuffers);
			glGetIntegerv(GL_MAX_FRAGMENT_IMAGE_UNIFORMS, (GLint*)&_deviceProperties.maxPerStageDescriptorSampledImages);
			glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, (GLint*)&_deviceProperties.maxPerStageDescriptorInputAttachments);
			glGetIntegerv(GL_MAX_FRAGMENT_IMAGE_UNIFORMS, (GLint*)&_deviceProperties.maxPerStageDescriptorStorageImages);
			glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, (GLint*)&_deviceProperties.maxPerStageResources);
			glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, (GLint*)&_deviceProperties.maxFragmentInputComponents);

			glGetIntegerv(GL_MAX_SAMPLES, (GLint*)&_deviceProperties.maxDescriptorSetSamplers);
			glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, (GLint*)&_deviceProperties.maxDescriptorSetUniformBuffers);
			glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, (GLint*)&_deviceProperties.maxDescriptorSetUniformBuffersDynamic);
			glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, (GLint*)&_deviceProperties.maxDescriptorSetStorageBuffers);
			glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, (GLint*)&_deviceProperties.maxDescriptorSetStorageBuffersDynamic);
			glGetIntegerv(GL_MAX_IMAGE_UNITS, (GLint*)&_deviceProperties.maxDescriptorSetSampledImages);
			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, (GLint*)&_deviceProperties.maxDescriptorSetInputAttachments);

			glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, (GLint*)&_deviceProperties.maxVertexInputAttributes);
			glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, (GLint*)&_deviceProperties.maxVertexInputBindings);
			glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, (GLint*)&_deviceProperties.maxVertexInputAttributeOffset);
			glGetIntegerv(GL_MAX_VERTEX_ATTRIB_STRIDE, (GLint*)&_deviceProperties.maxVertexInputBindingStride);
			glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, (GLint*)&_deviceProperties.maxVertexOutputComponents);

			glGetFloatv(GL_POINT_SIZE_RANGE, &_deviceProperties.minPointSizeRange);
			glGetFloatv(GL_POINT_SIZE_GRANULARITY, &_deviceProperties.pointSizeGranularity);
			glGetFloatv(GL_LINE_WIDTH_RANGE, &_deviceProperties.minLineWidthRange);
			glGetFloatv(GL_LINE_WIDTH_GRANULARITY, &_deviceProperties.lineWidthGranularity);
			glGetIntegerv(GL_LINE_WIDTH, (GLint*)&_deviceProperties.strictLines);

			glGetIntegerv(GL_MAX_DRAW_BUFFERS, (GLint*)&_deviceProperties.maxFragmentOutputAttachments);
			glGetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, (GLint*)&_deviceProperties.maxFragmentDualSrcAttachments);
			glGetIntegerv(GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES, (GLint*)&_deviceProperties.maxFragmentCombinedOutputResources);
			glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.subPixelPrecisionBits);
			glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.subTexelPrecisionBits);
			glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.mipmapPrecisionBits);
			glGetIntegerv(GL_MAX_ELEMENT_INDEX, (GLint*)&_deviceProperties.maxDrawIndexedIndexValue);
			glGetIntegerv(GL_MAX_ELEMENTS_INDICES, (GLint*)&_deviceProperties.maxDrawIndirectCount);

			glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &_deviceProperties.maxSamplerLodBias);
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &_deviceProperties.maxSamplerAnisotropy);
			glGetIntegerv(GL_MAX_VIEWPORTS, (GLint*)&_deviceProperties.maxViewports);
			glGetIntegerv(GL_MAX_VIEWPORT_DIMS, (GLint*)&_deviceProperties.maxViewportDimensionsW);
			glGetFloatv(GL_VIEWPORT_BOUNDS_RANGE, &_deviceProperties.minViewportBoundsRange);
			glGetIntegerv(GL_VIEWPORT_SUBPIXEL_BITS, (GLint*)&_deviceProperties.viewportSubPixelBits);
			glGetIntegerv(GL_MIN_MAP_BUFFER_ALIGNMENT, (GLint*)&_deviceProperties.minMemoryMapAlignment);

			glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, (GLint*)&_deviceProperties.minTexelOffset);
			glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, (GLint*)&_deviceProperties.maxTexelOffset);
			glGetIntegerv(GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET, (GLint*)&_deviceProperties.minTexelGatherOffset);
			glGetIntegerv(GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET, (GLint*)&_deviceProperties.maxTexelGatherOffset);
			glGetIntegerv(GL_MIN_FRAGMENT_INTERPOLATION_OFFSET, (GLint*)&_deviceProperties.minInterpolationOffset);
			glGetIntegerv(GL_MAX_FRAGMENT_INTERPOLATION_OFFSET, (GLint*)&_deviceProperties.maxInterpolationOffset);
			glGetIntegerv(GL_FRAGMENT_INTERPOLATION_OFFSET_BITS, (GLint*)&_deviceProperties.subPixelInterpolationOffsetBits);
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
			glGetIntegerv(GL_MAX_CLIP_DISTANCES, (GLint*)&_deviceProperties.maxClipDistances);

			if (glGetInteger64v)
			{
				glGetInteger64v(GL_SPARSE_BUFFER_PAGE_SIZE_ARB, (GLint64*)&_deviceProperties.sparseAddressSpaceSize);
				glGetInteger64v(GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT, (GLint64*)&_deviceProperties.minTexelBufferOffsetAlignment);
				glGetInteger64v(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, (GLint64*)&_deviceProperties.minUniformBufferOffsetAlignment);
				glGetInteger64v(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, (GLint64*)&_deviceProperties.minStorageBufferOffsetAlignment);
			}

			if (GLEW_ARB_tessellation_shader)
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

			if (GLEW_ARB_geometry_shader4)
			{
				glGetIntegerv(GL_MAX_GEOMETRY_ATOMIC_COUNTERS, (GLint*)&_deviceProperties.maxGeometryAtomicCounters);
				glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, (GLint*)&_deviceProperties.maxGeometryShaderStorageBlocks);
				glGetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS, (GLint*)&_deviceProperties.maxGeometryInputComponents);
				glGetIntegerv(GL_MAX_GEOMETRY_SHADER_INVOCATIONS, (GLint*)&_deviceProperties.maxGeometryShaderInvocations);
				glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, (GLint*)&_deviceProperties.maxGeometryOutputComponents);
				glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, (GLint*)&_deviceProperties.maxGeometryOutputVertices);
				glGetIntegerv(GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS, (GLint*)&_deviceProperties.maxGeometryTotalOutputComponents);
			}

			if (GLEW_ARB_compute_shader)
			{
				glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, (GLint*)&_deviceProperties.maxComputeSharedMemorySize);
				glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, (GLint*)&_deviceProperties.maxComputeWorkGroupSize);

				glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, (GLint*)&_deviceProperties.maxComputeWorkGroupCountX);
				glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, (GLint*)&_deviceProperties.maxComputeWorkGroupCountY);
				glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, (GLint*)&_deviceProperties.maxComputeWorkGroupCountZ);
				glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, (GLint*)&_deviceProperties.maxComputeWorkGroupSizeX);
				glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, (GLint*)&_deviceProperties.maxComputeWorkGroupSizeY);
				glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, (GLint*)&_deviceProperties.maxComputeWorkGroupSizeZ);
			}

			if (GLEW_ARB_cull_distance)
			{
				glGetIntegerv(GL_MAX_CULL_DISTANCES, (GLint*)&_deviceProperties.maxCullDistances);
				glGetIntegerv(GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES, (GLint*)&_deviceProperties.maxCombinedClipAndCullDistances);
			}
			
			return true;
		}

		bool
		GL33DeviceProperty::initTextureSupports() noexcept
		{
			if (GLEW_VERSION_2_0 || GLEW_EXT_texture)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R4G4UNormPack8);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R4G4B4A4UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8A8UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16A16UNorm);
			}

			if (GLEW_VERSION_2_0 || GLEW_EXT_texture || GLEW_EXT_bgra)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8A8UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B4G4R4A4UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B5G5R5A1UNormPack16);
			}

			if (GLEW_VERSION_2_0 || GLEW_EXT_texture || GLEW_EXT_abgr)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A1R5G5B5UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A2R10G10B10UNormPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A2B10G10R10UNormPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A8B8G8R8UNormPack32);
			}

			if (GLEW_EXT_texture_integer)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16UInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16SInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16A16UInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16A16SInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32B32UInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32B32SInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32B32A32UInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32B32A32SInt);
			}

			if (GLEW_EXT_bgra && GLEW_EXT_texture_integer)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8SInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8A8SInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8UInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8A8UInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8SInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8A8SInt);
			}

			if (GLEW_EXT_abgr && GLEW_EXT_texture_integer)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A8B8G8R8UIntPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A8B8G8R8SIntPack32);
			}

			if (GLEW_ARB_texture_float)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16A16SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32B32SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32B32A32SFloat);
			}

			if (GLEW_EXT_packed_float)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B10G11R11UFloatPack32);
			}

			if (GLEW_EXT_texture_shared_exponent)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::E5B9G9R9UFloatPack32);
			}

			if (GLEW_ARB_texture_stencil8)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::S8UInt);
			}

			if (GLEW_ARB_depth_texture)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::D16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::X8_D24UNormPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::D32_SFLOAT);
			}

			if (GLEW_ARB_depth_buffer_float || GLEW_NV_depth_buffer_float)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::D32_SFLOAT_S8UInt);
			}

			if (GLEW_EXT_packed_depth_stencil || GLEW_ARB_framebuffer_object)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::D24UNorm_S8UInt);
			}

			if (GLEW_ARB_texture_rgb10_a2ui)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A2B10G10R10UIntPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A2R10G10B10UIntPack32);
			}

			if (GLEW_EXT_texture_sRGB)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8A8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBSRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBASRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC3SRGBBlock);
			}

			if (GLEW_EXT_bgra && GLEW_EXT_texture_sRGB)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8A8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A8B8G8R8SRGBPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A8B8G8R8SRGBPack32);
			}

			if (GLEW_EXT_abgr && GLEW_EXT_texture_sRGB)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A8B8G8R8SRGBPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A8B8G8R8SRGBPack32);
			}

			if (GLEW_ARB_texture_rg)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16UInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16SInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16UInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16SInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32SInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32UInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32SInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32UInt);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32SFloat);
			}

			if (GLEW_EXT_texture_snorm)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8SNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8SNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8SNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8A8SNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16SNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16SNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16SNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16A16SNorm);
			}

			if (GLEW_EXT_bgra && GLEW_EXT_texture_snorm)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8SNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8A8SNorm);
			}

			if (GLEW_EXT_bgra && GLEW_EXT_texture_snorm)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A8B8G8R8SNormPack32);
			}

			if (GLEW_EXT_texture_compression_s3tc)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBUNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBAUNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC3UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC5UNormBlock);
			}

			if (GLEW_ARB_ES2_compatibility)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R5G6B5UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B5G6R5UNormPack16);
			}

			if (GLEW_ARB_ES3_compatibility)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ETC2R8G8B8UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ETC2R8G8B8A1UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ETC2R8G8B8A8UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::EACR11UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::EACR11SNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::EACR11G11UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::EACR11G11SNormBlock);
			}

			if (GLEW_ARB_ES3_compatibility && GL_EXT_texture_sRGB)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ETC2R8G8B8SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ETC2R8G8B8A1SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ETC2R8G8B8A8SRGBBlock);
			}

			if (GLEW_KHR_texture_compression_astc_ldr || GLEW_KHR_texture_compression_astc_hdr)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC4x4UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC5x4UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC5x5UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC6x5UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC6x6UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC8x5UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC8x6UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC8x8UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x5UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x6UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x8UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x10UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC12x10UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC12x12UNormBlock);
			}

			if ((GLEW_KHR_texture_compression_astc_ldr || GLEW_KHR_texture_compression_astc_hdr) && GL_EXT_texture_sRGB)
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC4x4SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC5x4SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC5x5SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC6x5SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC6x6SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC8x5SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC8x6SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC8x8SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x5SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x6SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x8SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x10SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC12x10SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC12x12SRGBBlock);
			}

			return true;
		}

		bool
		GL33DeviceProperty::initTextureDimSupports() noexcept
		{
			_deviceProperties.supportTextureDims.push_back(TextureDimension::Texture2D);
			_deviceProperties.supportTextureDims.push_back(TextureDimension::Texture2DArray);
			_deviceProperties.supportTextureDims.push_back(TextureDimension::Cube);

			if (GL_ARB_texture_multisample)
			{
				_deviceProperties.supportTextureDims.push_back(TextureDimension::Texture2DMultisample);
				_deviceProperties.supportTextureDims.push_back(TextureDimension::Texture2DArrayMultisample);
			}

			if (GLEW_ARB_texture_cube_map_array)
				_deviceProperties.supportTextureDims.push_back(TextureDimension::CubeArray);

			if (GLEW_ARB_texture_storage)
				_deviceProperties.supportTextureDims.push_back(TextureDimension::Texture3D);

			return true;
		}

		bool
		GL33DeviceProperty::initVertexSupports() noexcept
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

			if (GLEW_ARB_vertex_attrib_64bit || GLEW_EXT_vertex_attrib_64bit)
			{
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::R64SInt);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::R64UInt);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::R64G64SInt);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::R64G64UInt);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::R64G64B64SInt);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::R64G64B64UInt);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::R64G64B64A64SInt);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::R64G64B64A64UInt);
			}

			if (GLEW_ARB_vertex_type_10f_11f_11f_rev)
			{
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::B10G11R11UFloatPack32);
			}

			if (GLEW_ARB_vertex_type_2_10_10_10_rev)
			{
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::A2R10G10B10SIntPack32);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::A2R10G10B10UIntPack32);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::A2R10G10B10UNormPack32);

				if (GLEW_ARB_vertex_array_bgra)
				{
					_deviceProperties.supportAttribute.push_back(GraphicsFormat::A2B10G10R10SIntPack32);
					_deviceProperties.supportAttribute.push_back(GraphicsFormat::A2B10G10R10UIntPack32);
					_deviceProperties.supportAttribute.push_back(GraphicsFormat::A2B10G10R10UNormPack32);
				}
			}

			if (GLEW_ARB_vertex_array_bgra)
			{
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::B8G8R8SInt);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::B8G8R8UInt);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::B8G8R8SNorm);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::B8G8R8UNorm);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::B8G8R8A8SInt);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::B8G8R8A8UInt);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::B8G8R8A8SNorm);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::B8G8R8A8UNorm);
			}

			return true;
		}

		bool
		GL33DeviceProperty::initShaderSupports() noexcept
		{
			_deviceProperties.supportShaders.push_back(ShaderStageFlagBits::VertexBit);
			_deviceProperties.supportShaders.push_back(ShaderStageFlagBits::FragmentBit);

			if (GLEW_ARB_geometry_shader4)
				_deviceProperties.supportShaders.push_back(ShaderStageFlagBits::GeometryBit);

			if (GLEW_ARB_compute_shader)
				_deviceProperties.supportShaders.push_back(ShaderStageFlagBits::ComputeBit);

			if (GLEW_ARB_tessellation_shader)
			{
				_deviceProperties.supportShaders.push_back(ShaderStageFlagBits::TessControlBit);
				_deviceProperties.supportShaders.push_back(ShaderStageFlagBits::TessEvaluationBit);
			}

			return true;
		}
	}
}