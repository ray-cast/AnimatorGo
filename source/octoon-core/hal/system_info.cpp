#include <octoon/hal/system_info.h>

namespace octoon
{
	SystemInfo::SystemInfo() noexcept
		: maxImageDimension1D(0)
		, maxImageDimension2D(0)
		, maxImageDimension3D(0)
		, maxCubemapSize(0)
		, maxImageArrayLayers(2048)
		, maxTextureSize(65536)
		, maxUniformBufferRange(16384)
		, maxStorageBufferRange(0x1000000)
		, maxPushConstantsSize(0)
		, maxMemoryAllocationCount(1000)
		, maxSamplerAllocationCount(1000)
		, bufferImageGranularity(0)
		, sparseAddressSpaceSize(0)
		, maxBoundDescriptorSets(0)
		, maxPerStageDescriptorSamplers(0)
		, maxPerStageDescriptorUniformBuffers(0)
		, maxPerStageDescriptorStorageBuffers(0)
		, maxPerStageDescriptorSampledImages(0)
		, maxPerStageDescriptorStorageImages(0)
		, maxPerStageDescriptorInputAttachments(0)
		, maxPerStageResources(0)
		, maxDescriptorSetSamplers(0)
		, maxDescriptorSetUniformBuffers(0)
		, maxDescriptorSetUniformBuffersDynamic(0)
		, maxDescriptorSetStorageBuffers(0)
		, maxDescriptorSetStorageBuffersDynamic(0)
		, maxDescriptorSetSampledImages(0)
		, maxDescriptorSetStorageImages(0)
		, maxDescriptorSetInputAttachments(0)
		, maxVertexInputAttributes(16)
		, maxVertexInputBindings(16)
		, maxVertexInputAttributeOffset(2048)
		, maxVertexInputBindingStride(2048)
		, maxVertexOutputComponents(0)
		, maxTessellationGenerationLevel(0)
		, maxTessellationPatchSize(0)
		, maxTessellationControlPerVertexInputComponents(0)
		, maxTessellationControlPerVertexOutputComponents(0)
		, maxTessellationControlPerPatchOutputComponents(0)
		, maxTessellationControlTotalOutputComponents(0)
		, maxTessellationEvaluationInputComponents(0)
		, maxTessellationEvaluationOutputComponents(0)
		, maxGeometryShaderInvocations(0)
		, maxGeometryInputComponents(0)
		, maxGeometryOutputComponents(0)
		, maxGeometryOutputVertices(0)
		, maxGeometryTotalOutputComponents(0)
		, maxFragmentInputComponents(0)
		, maxFragmentOutputAttachments(1)
		, maxFragmentDualSrcAttachments(0)
		, maxFragmentCombinedOutputResources(0)
		, maxComputeSharedMemorySize(0)
		, maxComputeWorkGroupSize(0)
		, subPixelPrecisionBits(0)
		, subTexelPrecisionBits(0)
		, mipmapPrecisionBits(0)
		, maxDrawIndexedIndexValue(65535)
		, maxDrawIndirectCount(0)
		, maxSamplerLodBias(2.0)
		, maxSamplerAnisotropy(0)
		, maxViewports(1)
		, maxViewportDimensionsW(0)
		, maxViewportDimensionsH(0)
		, minViewportBoundsRange(-32768)
		, maxViewportBoundsRange(32767)
		, viewportSubPixelBits(0)
		, minMemoryMapAlignment(0)
		, minTexelBufferOffsetAlignment(0)
		, minUniformBufferOffsetAlignment(0)
		, minStorageBufferOffsetAlignment(0)
		, minTexelOffset(-8)
		, maxTexelOffset(7)
		, minTexelGatherOffset(-8)
		, maxTexelGatherOffset(7)
		, minInterpolationOffset(-0.5)
		, maxInterpolationOffset(0.5)
		, subPixelInterpolationOffsetBits(4)
		, maxFramebufferWidth(16384)
		, maxFramebufferHeight(16384)
		, maxFramebufferLayers(2048)
		, maxFramebufferColorAttachments(1)
		, framebufferColorSampleCounts(15)
		, framebufferDepthSampleCounts(15)
		, framebufferStencilSampleCounts(15)
		, framebufferNoAttachmentsSampleCounts(15)
		, sampledImageColorSampleCounts(15)
		, sampledImageIntegerSampleCounts(15)
		, sampledImageDepthSampleCounts(15)
		, sampledImageStencilSampleCounts(15)
		, storageImageSampleCounts(15)
		, maxSampleMaskWords(0)
		, timestampComputeAndGraphics(0)
		, timestampPeriod(1.0)
		, maxClipDistances(0)
		, maxCullDistances(0)
		, maxCombinedClipAndCullDistances(0)
		, discreteQueuePriorities(0)
		, minPointSizeRange(1.0)
		, maxPointSizeRange(1.0)
		, pointSizeGranularity(0)
		, minLineWidthRange(1.0)
		, maxLineWidthRange(1.0)
		, lineWidthGranularity(0)
		, strictLines(0)
		, standardSampleLocations(0)
		, optimalBufferCopyOffsetAlignment(0)
		, optimalBufferCopyRowPitchAlignment(0)
		, nonCoherentAtomSize(0)
		, maxComputeWorkGroupSizeX(0)
		, maxComputeWorkGroupSizeY(0)
		, maxComputeWorkGroupSizeZ(0)
		, maxComputeWorkGroupCountX(0)
		, maxComputeWorkGroupCountY(0)
		, maxComputeWorkGroupCountZ(0)
	{
	}

	SystemInfo::~SystemInfo() noexcept
	{
	}

	bool
	SystemInfo::isTextureSupport(GraphicsFormat format) const noexcept
	{
		return std::find(supportTextures.begin(), supportTextures.end(), format) != supportTextures.end();
	}

	bool
	SystemInfo::isTextureDimSupport(TextureDimension dimension) const noexcept
	{
		return std::find(supportTextureDims.begin(), supportTextureDims.end(), dimension) != supportTextureDims.end();
	}

	bool
	SystemInfo::isVertexSupport(GraphicsFormat format) const noexcept
	{
		return std::find(supportAttribute.begin(), supportAttribute.end(), format) != supportAttribute.end();
	}

	bool
	SystemInfo::isShaderSupport(ShaderStageFlags stage) const noexcept
	{
		return std::find(supportShaders.begin(), supportShaders.end(), stage) != supportShaders.end();
	}
}