#ifndef OCTOON_GRAPHICS_TYPES_H_
#define OCTOON_GRAPHICS_TYPES_H_

#include <octoon/math/math.h>
#include <octoon/runtime/rtti.h>
#include <octoon/runtime/rtti_interface.h>

namespace octoon
{
	namespace graphics
	{
		using int1 = math::int1;
		using int2 = math::int2;
		using int3 = math::int3;
		using int4 = math::int4;

		using uint1 = math::uint1;
		using uint2 = math::uint2;
		using uint3 = math::uint3;
		using uint4 = math::uint4;

		using float1 = math::float1;
		using float2 = math::float2;
		using float3 = math::float3;
		using float4 = math::float4;

		using float2x2 = math::float2x2;
		using float3x3 = math::float3x3;
		using float4x4 = math::float4x4;

		using double1 = math::double1;
		using double2 = math::double2;
		using double3 = math::double3;
		using double4 = math::double4;

		using double2x2 = math::double2x2;
		using double3x3 = math::double3x3;
		using double4x4 = math::double4x4;

		typedef std::shared_ptr<class GraphicsDevice> GraphicsDevicePtr;
		typedef std::shared_ptr<class GraphicsDeviceProperty> GraphicsDevicePropertyPtr;
		typedef std::shared_ptr<class GraphicsDebug> GraphicsDebugPtr;
		typedef std::shared_ptr<class GraphicsSwapchain> GraphicsSwapchainPtr;
		typedef std::shared_ptr<class GraphicsResource> GraphicsResourcePtr;
		typedef std::shared_ptr<class GraphicsContext> GraphicsContextPtr;
		typedef std::shared_ptr<class GraphicsData> GraphicsDataPtr;
		typedef std::shared_ptr<class GraphicsInputLayout> GraphicsInputLayoutPtr;
		typedef std::shared_ptr<class GraphicsState> GraphicsStatePtr;
		typedef std::shared_ptr<class GraphicsTexture> GraphicsTexturePtr;
		typedef std::shared_ptr<class GraphicsSampler> GraphicsSamplerPtr;
		typedef std::shared_ptr<class GraphicsFramebuffer> GraphicsFramebufferPtr;
		typedef std::shared_ptr<class GraphicsFramebufferLayout> GraphicsFramebufferLayoutPtr;
		typedef std::shared_ptr<class GraphicsParam> GraphicsParamPtr;
		typedef std::shared_ptr<class GraphicsAttribute> GraphicsAttributePtr;
		typedef std::shared_ptr<class GraphicsUniform> GraphicsUniformPtr;
		typedef std::shared_ptr<class GraphicsUniformBlock> GraphicsUniformBlockPtr;
		typedef std::shared_ptr<class GraphicsShader> GraphicsShaderPtr;
		typedef std::shared_ptr<class GraphicsProgram> GraphicsProgramPtr;
		typedef std::shared_ptr<class GraphicsPipeline> GraphicsPipelinePtr;
		typedef std::shared_ptr<class GraphicsPipelineLayout> GraphicsPipelineLayoutPtr;
		typedef std::shared_ptr<class GraphicsDescriptorPool> GraphicsDescriptorPoolPtr;
		typedef std::shared_ptr<class GraphicsDescriptorSet> GraphicsDescriptorSetPtr;
		typedef std::shared_ptr<class GraphicsDescriptorSetLayout> GraphicsDescriptorSetLayoutPtr;
		typedef std::shared_ptr<class GraphicsUniformSet> GraphicsUniformSetPtr;
		typedef std::shared_ptr<class GraphicsVariant> GraphicsVariantPtr;
		typedef std::shared_ptr<class GraphicsCommandPool> GraphicsCommandPoolPtr;
		typedef std::shared_ptr<class GraphicsCommandQueue> GraphicsCommandQueuePtr;
		typedef std::shared_ptr<class GraphicsCommandList> GraphicsCommandListPtr;
		typedef std::shared_ptr<class GraphicsSemaphore> GraphicsSemaphorePtr;
		typedef std::shared_ptr<class GraphicsIndirect> GraphicsIndirectPtr;
		typedef std::shared_ptr<class GraphicsDeviceDesc> GraphicsDeviceDescPtr;
		typedef std::shared_ptr<class GraphicsSwapchainDesc> GraphicsSwapchainDescPtr;
		typedef std::shared_ptr<class GraphicsContextDesc> GraphicsContextDescPtr;
		typedef std::shared_ptr<class GraphicsInputLayoutDesc> GraphicsInputLayoutDescPtr;
		typedef std::shared_ptr<class GraphicsDataDesc> GraphicsDataDescPtr;
		typedef std::shared_ptr<class GraphicsTextureDesc> GraphicsTextureDescPtr;
		typedef std::shared_ptr<class GraphicsSamplerDesc> GraphicsSamplerDescPtr;
		typedef std::shared_ptr<class GraphicsFramebufferDesc> GraphicsFramebufferDescPtr;
		typedef std::shared_ptr<class GraphicsFramebufferLayoutDesc> GraphicsFramebufferLayoutDescPtr;
		typedef std::shared_ptr<class GraphicsStateDesc> GraphicsStateDescPtr;
		typedef std::shared_ptr<class GraphicsShaderDesc> GraphicsShaderDescPtr;
		typedef std::shared_ptr<class GraphicsProgramDesc> GraphicsProgramDescPtr;
		typedef std::shared_ptr<class GraphicsPipelineDesc> GraphicsPipelineDescPtr;
		typedef std::shared_ptr<class GraphicsPipelineLayoutDesc> GraphicsPipelineLayoutDescPtr;
		typedef std::shared_ptr<class GraphicsDescriptorSetDesc> GraphicsDescriptorSetDescPtr;
		typedef std::shared_ptr<class GraphicsDescriptorSetLayoutDesc> GraphicsDescriptorSetLayoutDescPtr;
		typedef std::shared_ptr<class GraphicsDescriptorPoolDesc> GraphicsDescriptorPoolDescPtr;
		typedef std::shared_ptr<class GraphicsCommandQueueDesc> GraphicsCommandQueueDescPtr;
		typedef std::shared_ptr<class GraphicsCommandPoolDesc> GraphicsCommandPoolDescPtr;
		typedef std::shared_ptr<class GraphicsCommandListDesc> GraphicsCommandListDescPtr;
		typedef std::shared_ptr<class GraphicsSemaphoreDesc> GraphicsSemaphoreDescPtr;
		typedef std::shared_ptr<class GraphicsVertexLayout> GraphicsVertexLayoutPtr;
		typedef std::shared_ptr<class GraphicsVertexBinding> GraphicsVertexBindingPtr;
		typedef std::shared_ptr<class GraphicsDescriptorPoolComponent> GraphicsDescriptorPoolComponentPtr;
		typedef std::shared_ptr<class GraphicsAttachmentLayout> GraphicsAttachmentLayoutPtr;
		typedef std::shared_ptr<class GraphicsAttachmentBinding> GraphicsAttachmentBindingPtr;

		typedef std::weak_ptr<class GraphicsDevice> GraphicsDeviceWeakPtr;
		typedef std::weak_ptr<class GraphicsDeviceProperty> GraphicsDevicePropertyWeakPtr;
		typedef std::weak_ptr<class GraphicsDebug> GraphicsDebugWeakPtr;
		typedef std::weak_ptr<class GraphicsSwapchain> GraphicsSwapchainWeakPtr;
		typedef std::weak_ptr<class GraphicsResource> GraphicsResourceWeakPtr;
		typedef std::weak_ptr<class GraphicsContext> GraphicsContextWeakPtr;
		typedef std::weak_ptr<class GraphicsData> GraphicsDataWeakPtr;
		typedef std::weak_ptr<class GraphicsInputLayout> GraphicsInputLayoutWeakPtr;
		typedef std::weak_ptr<class GraphicsState> GraphicsStateWeakPtr;
		typedef std::weak_ptr<class GraphicsTexture> GraphicsTextureWeakPtr;
		typedef std::weak_ptr<class GraphicsSampler> GraphicsSamplerWeakPtr;
		typedef std::weak_ptr<class GraphicsFramebuffer> GraphicsFramebufferWeakPtr;
		typedef std::weak_ptr<class GraphicsFramebufferLayout> GraphicsFramebufferLayoutWeakPtr;
		typedef std::weak_ptr<class GraphicsShader> GraphicsShaderWeakPtr;
		typedef std::weak_ptr<class GraphicsProgram> GraphicsProgramWeakPtr;
		typedef std::weak_ptr<class GraphicsPipeline> GraphicsPipelineWeakPtr;
		typedef std::weak_ptr<class GraphicsPipelineLayout> GraphicsPipelineLayoutWeakPtr;
		typedef std::weak_ptr<class GraphicsDescriptorPool> GraphicsDescriptorPoolWeakPtr;
		typedef std::weak_ptr<class GraphicsDescriptorSet> GraphicsDescriptorSetWeakPtr;
		typedef std::weak_ptr<class GraphicsDescriptorSetLayout> GraphicsDescriptorSetLayoutWeakPtr;
		typedef std::weak_ptr<class GraphicsUniformSet> GraphicsUniformWeakPtr;
		typedef std::weak_ptr<class GraphicsVariant> GraphicsVariantWeakPtr;
		typedef std::weak_ptr<class GraphicsCommandPool> GraphicsCommandPoolWeakPtr;
		typedef std::weak_ptr<class GraphicsCommandQueue> GraphicsCommandQueueWeakPtr;
		typedef std::weak_ptr<class GraphicsCommandList> GraphicsCommandPoolListWeakPtr;
		typedef std::weak_ptr<class GraphicsSemaphoreDesc> GraphicsSemaphoreDescWeakPtr;
		typedef std::weak_ptr<class GraphicsDeviceDesc> GraphicsDeviceDescWeakPtr;
		typedef std::weak_ptr<class GraphicsSwapchainDesc> GraphicsSwapchainDescWeakPtr;
		typedef std::weak_ptr<class GraphicsContextDesc> GraphicsContextDescWeakPtr;
		typedef std::weak_ptr<class GraphicsInputLayoutDesc> GraphicsInputLayoutDescWeakPtr;
		typedef std::weak_ptr<class GraphicsDataDesc> GraphicsDataDescWeakPtr;
		typedef std::weak_ptr<class GraphicsTextureDesc> GraphicsTextureDescWeakPtr;
		typedef std::weak_ptr<class GraphicsSamplerDesc> GraphicsSamplerDescWeakPtr;
		typedef std::weak_ptr<class GraphicsFramebufferDesc> GraphicsFramebufferDescWeakPtr;
		typedef std::weak_ptr<class GraphicsFramebufferLayoutDesc> GraphicsFramebufferLayoutDescWeakPtr;
		typedef std::weak_ptr<class GraphicsStateDesc> GraphicsStateDescWeakPtr;
		typedef std::weak_ptr<class GraphicsShaderDesc> GraphicsShaderDescWeakPtr;
		typedef std::weak_ptr<class GraphicsProgramDesc> GraphicsProgramDescWeakPtr;
		typedef std::weak_ptr<class GraphicsPipelineDesc> GraphicsPipelineDescWeakPtr;
		typedef std::weak_ptr<class GraphicsPipelineLayoutDesc> GraphicsPipelineLayoutDescWeakPtr;
		typedef std::weak_ptr<class GraphicsDescriptorSetDesc> GraphicsDescriptorSetDescWeakPtr;
		typedef std::weak_ptr<class GraphicsDescriptorSetLayoutDesc> GraphicsDescriptorSetLayoutDescWeakPtr;
		typedef std::weak_ptr<class GraphicsDescriptorPoolDesc> GraphicsDescriptorPoolDescWeakPtr;
		typedef std::weak_ptr<class GraphicsCommandQueueDesc> GraphicsCommandQueueDescWeakPtr;
		typedef std::weak_ptr<class GraphicsCommandPoolDesc> GraphicsCommandPoolDescWeakPtr;
		typedef std::weak_ptr<class GraphicsCommandListDesc> GraphicsCommandListDescWeakPtr;
		typedef std::weak_ptr<class GraphicsSemaphoreDesc> GraphicsSemaphoreDescWeakPtr;
		typedef std::weak_ptr<class GraphicsVertexLayout> GraphicsVertexLayoutWeakPtr;
		typedef std::weak_ptr<class GraphicsVertexBinding> GraphicsVertexBindingWeakPtr;
		typedef std::weak_ptr<class GraphicsDescriptorPoolComponent> GraphicsDescriptorPoolComponentWeakPtr;
		typedef std::weak_ptr<class GraphicsAttachmentLayout> GraphicsAttachmentLayoutWeakPtr;
		typedef std::weak_ptr<class GraphicsAttachmentBinding> GraphicsAttachmentBindingWeakPtr;

		typedef std::vector<GraphicsDevicePtr> GraphicsDevices;
		typedef std::vector<GraphicsDevicePropertyPtr> GraphicsDevicePropertys;
		typedef std::vector<GraphicsShaderPtr> GraphicsShaders;
		typedef std::vector<GraphicsVariantPtr> GraphicsVariants;
		typedef std::vector<GraphicsFramebufferPtr> GraphicsFramebuffers;
		typedef std::vector<GraphicsIndirectPtr> GraphicsIndirects;
		typedef std::vector<GraphicsUniformSetPtr> GraphicsUniformSets;
		typedef std::vector<GraphicsTexturePtr> GraphicsTextures;
		typedef std::vector<GraphicsSamplerPtr> GraphicsSamplers;
		typedef std::vector<GraphicsAttributePtr> GraphicsAttributes;
		typedef std::vector<GraphicsParamPtr> GraphicsParams;
		typedef std::vector<GraphicsUniformPtr> GraphicsUniforms;
		typedef std::vector<GraphicsUniformBlockPtr> GraphicsUniformBlocks;
		typedef std::vector<GraphicsDescriptorSetLayoutPtr> GraphicsDescriptorSetLayouts;
		typedef std::vector<GraphicsInputLayoutPtr> GraphicsInputLayouts;
		typedef std::vector<GraphicsDeviceDescPtr> GraphicsDeviceDescs;
		typedef std::vector<GraphicsSwapchainDescPtr> GraphicsSwapchainDescs;
		typedef std::vector<GraphicsContextDescPtr> GraphicsContextDescs;
		typedef std::vector<GraphicsInputLayoutDescPtr> GraphicsInputLayoutDescs;
		typedef std::vector<GraphicsDataDescPtr> GraphicsDataDescs;
		typedef std::vector<GraphicsTextureDescPtr> GraphicsTextureDescs;
		typedef std::vector<GraphicsSamplerDescPtr> GraphicsSamplerDescs;
		typedef std::vector<GraphicsFramebufferDescPtr> GraphicsFramebufferDescs;
		typedef std::vector<GraphicsFramebufferLayoutDescPtr> GraphicsFramebufferLayoutDescs;
		typedef std::vector<GraphicsStateDescPtr> GraphicsStateDescs;
		typedef std::vector<GraphicsShaderDescPtr> GraphicsShaderDescs;
		typedef std::vector<GraphicsProgramDescPtr> GraphicsProgramDescs;
		typedef std::vector<GraphicsPipelineDescPtr> GraphicsPipelineDescs;
		typedef std::vector<GraphicsPipelineLayoutDescPtr> GraphicsPipelineLayoutDescs;
		typedef std::vector<GraphicsDescriptorSetDescPtr> GraphicsDescriptorSetDescs;
		typedef std::vector<GraphicsDescriptorSetLayoutDescPtr> GraphicsDescriptorSetLayoutDescs;
		typedef std::vector<GraphicsDescriptorPoolDescPtr> GraphicsDescriptorPoolDescs;
		typedef std::vector<GraphicsCommandQueueDescPtr> GraphicsCommandQueueDescs;
		typedef std::vector<GraphicsCommandPoolDescPtr> GraphicsCommandPoolDescs;
		typedef std::vector<GraphicsCommandListDescPtr> GraphicsCommandListDescs;
		typedef std::vector<GraphicsSemaphoreDescPtr> GraphicsSemaphoreDescs;

		typedef std::vector<GraphicsVertexLayout> GraphicsVertexLayouts;
		typedef std::vector<GraphicsVertexBinding> GraphicsVertexBindings;
		typedef std::vector<GraphicsDescriptorPoolComponent> GraphicsDescriptorPoolComponents;
		typedef std::vector<GraphicsAttachmentLayout> GraphicsAttachmentLayouts;
		typedef std::vector<GraphicsAttachmentBinding> GraphicsAttachmentBindings;
		typedef std::vector<class GraphicsColorBlend> GraphicsColorBlends;

		typedef std::vector<GraphicsDeviceWeakPtr> GraphicsDeviceWeaks;
		typedef std::vector<GraphicsContextWeakPtr> GraphicsContextWeaks;

		typedef void* WindHandle;

		enum class GraphicsDeviceType : std::uint8_t
		{
			D3D9 = 1,
			D3D11 = 1,
			D3D12 = 2,
			OpenGL = 3,
			OpenGLCore = 4,
			OpenGLES2 = 5,
			OpenGLES3 = 6,
			OpenGLES31 = 7,
			OpenGLES32 = 8,
			Vulkan = 9,
		};

		enum class GraphicsSwapInterval : std::uint8_t
		{
			Free = 0,
			Vsync = 1,
			Fps30 = 2,
			Fps15 = 3,
		};

		enum class GraphicsCompareFunc : std::uint8_t
		{
			None = 0,
			Lequal = 1,
			Equal = 2,
			Greater = 3,
			Less = 4,
			Gequal = 5,
			NotEqual = 6,
			Always = 7,
			Never = 8,
		};

		enum class GraphicsBlendFactor : std::uint8_t
		{
			Zero = 0,
			One = 1,
			DstCol = 2,
			SrcColor = 3,
			SrcAlpha = 4,
			DstAlpha = 5,
			OneMinusSrcCol = 6,
			OneMinusDstCol = 7,
			OneMinusSrcAlpha = 8,
			OneMinusDstAlpha = 9,
			ConstantColor = 10,
			ConstantAlpha = 11,
			OneMinusConstantColor = 12,
			OneMinusConstantAlpha = 13,
			SrcAlphaSaturate = 14,
		};

		enum class GraphicsBlendOp : std::uint8_t
		{
			Add = 0,
			Subtract = 1,
			RevSubtract = 2,
		};

		struct GraphicsColorMaskFlagBits
		{
			enum Flags
			{
				RedBit = 1,
				GreendBit = 2,
				BlurBit = 4,
				AlphaBit = 8,
				RGBBit = RedBit | GreendBit | BlurBit,
				RGBABit = RedBit | GreendBit | BlurBit | AlphaBit
			};
		};

		typedef std::uint32_t GraphicsColorMaskFlags;

		enum class GraphicsCullMode : std::uint8_t
		{
			None = 0,
			Front = 1,
			Back = 2,
			FrontBack = 3,
		};

		enum class GraphicsFrontFace : std::uint8_t
		{
			CW = 0,
			CCW = 1,
		};

		enum class GraphicsPolygonMode : std::uint8_t
		{
			Point = 0,
			Wireframe = 1,
			Solid = 2,
		};

		enum class GraphicsStencilOp : std::uint8_t
		{
			Keep = 0,
			Replace = 1,
			Incr = 2,
			Decr = 3,
			Zero = 4,
			IncrWrap = 5,
			DecrWrap = 6,
		};

		struct GraphicsStencilFaceFlagBits
		{
			enum Flags
			{
				FrontBit = 1,
				BackBit = 2,
				AllBit = 0x7FFFFFFF
			};
		};

		typedef std::uint32_t GraphicsStencilFaceFlags;

		struct GraphicsClearFlagBits
		{
			enum Flags
			{
				ColorBit = 1,
				DepthBit = 2,
				StencilBit = 4,
				ColorDepthBit = ColorBit | DepthBit,
				ColorStencilBit = ColorBit | StencilBit,
				DepthStencilBit = DepthBit | StencilBit,
				AllBit = ColorBit | DepthBit | StencilBit
			};
		};

		typedef std::uint32_t GraphicsClearFlags;

		enum class GraphicsFormat : std::uint8_t
		{
			Undefined = 0,
			R4G4UNormPack8 = 1,
			R4G4B4A4UNormPack16 = 2,
			B4G4R4A4UNormPack16 = 3,
			R5G6B5UNormPack16 = 4,
			B5G6R5UNormPack16 = 5,
			R5G5B5A1UNormPack16 = 6,
			B5G5R5A1UNormPack16 = 7,
			A1R5G5B5UNormPack16 = 8,
			R8UNorm = 9,
			R8SNorm = 10,
			R8UScaled = 11,
			R8SScaled = 12,
			R8UInt = 13,
			R8SInt = 14,
			R8SRGB = 15,
			R8G8UNorm = 16,
			R8G8SNorm = 17,
			R8G8UScaled = 18,
			R8G8SScaled = 19,
			R8G8UInt = 20,
			R8G8SInt = 21,
			R8G8SRGB = 22,
			R8G8B8UNorm = 23,
			R8G8B8SNorm = 24,
			R8G8B8UScaled = 25,
			R8G8B8SScaled = 26,
			R8G8B8UInt = 27,
			R8G8B8SInt = 28,
			R8G8B8SRGB = 29,
			B8G8R8UNorm = 30,
			B8G8R8SNorm = 31,
			B8G8R8UScaled = 32,
			B8G8R8SScaled = 33,
			B8G8R8UInt = 34,
			B8G8R8SInt = 35,
			B8G8R8SRGB = 36,
			R8G8B8A8UNorm = 37,
			R8G8B8A8SNorm = 38,
			R8G8B8A8UScaled = 39,
			R8G8B8A8SScaled = 40,
			R8G8B8A8UInt = 41,
			R8G8B8A8SInt = 42,
			R8G8B8A8SRGB = 43,
			B8G8R8A8UNorm = 44,
			B8G8R8A8SNorm = 45,
			B8G8R8A8UScaled = 46,
			B8G8R8A8SScaled = 47,
			B8G8R8A8UInt = 48,
			B8G8R8A8SInt = 49,
			B8G8R8A8SRGB = 50,
			A8B8G8R8UNormPack32 = 51,
			A8B8G8R8SNormPack32 = 52,
			A8B8G8R8UScaledPack32 = 53,
			A8B8G8R8SScaledPack32 = 54,
			A8B8G8R8UIntPack32 = 55,
			A8B8G8R8SIntPack32 = 56,
			A8B8G8R8SRGBPack32 = 57,
			A2R10G10B10UNormPack32 = 58,
			A2R10G10B10SNormPack32 = 59,
			A2R10G10B10UScaledPack32 = 60,
			A2R10G10B10SScaledPack32 = 61,
			A2R10G10B10UIntPack32 = 62,
			A2R10G10B10SIntPack32 = 63,
			A2B10G10R10UNormPack32 = 64,
			A2B10G10R10SNormPack32 = 65,
			A2B10G10R10UScaledPack32 = 66,
			A2B10G10R10SScaledPack32 = 67,
			A2B10G10R10UIntPack32 = 68,
			A2B10G10R10SIntPack32 = 69,
			R16UNorm = 70,
			R16SNorm = 71,
			R16UScaled = 72,
			R16SScaled = 73,
			R16UInt = 74,
			R16SInt = 75,
			R16SFloat = 76,
			R16G16UNorm = 77,
			R16G16SNorm = 78,
			R16G16UScaled = 79,
			R16G16SScaled = 80,
			R16G16UInt = 81,
			R16G16SInt = 82,
			R16G16SFloat = 83,
			R16G16B16UNorm = 84,
			R16G16B16SNorm = 85,
			R16G16B16UScaled = 86,
			R16G16B16SScaled = 87,
			R16G16B16UInt = 88,
			R16G16B16SInt = 89,
			R16G16B16SFloat = 90,
			R16G16B16A16UNorm = 91,
			R16G16B16A16SNorm = 92,
			R16G16B16A16UScaled = 93,
			R16G16B16A16SScaled = 94,
			R16G16B16A16UInt = 95,
			R16G16B16A16SInt = 96,
			R16G16B16A16SFloat = 97,
			R32UInt = 98,
			R32SInt = 99,
			R32SFloat = 100,
			R32G32UInt = 101,
			R32G32SInt = 102,
			R32G32SFloat = 103,
			R32G32B32UInt = 104,
			R32G32B32SInt = 105,
			R32G32B32SFloat = 106,
			R32G32B32A32UInt = 107,
			R32G32B32A32SInt = 108,
			R32G32B32A32SFloat = 109,
			R64UInt = 110,
			R64SInt = 111,
			R64SFloat = 112,
			R64G64UInt = 113,
			R64G64SInt = 114,
			R64G64SFloat = 115,
			R64G64B64UInt = 116,
			R64G64B64SInt = 117,
			R64G64B64SFloat = 118,
			R64G64B64A64UInt = 119,
			R64G64B64A64SInt = 120,
			R64G64B64A64SFloat = 121,
			B10G11R11UFloatPack32 = 122,
			E5B9G9R9UFloatPack32 = 123,
			D16UNorm = 124,
			X8_D24UNormPack32 = 125,
			D32_SFLOAT = 126,
			S8UInt = 127,
			D16UNorm_S8UInt = 128,
			D24UNorm_S8UInt = 129,
			D32_SFLOAT_S8UInt = 130,
			BC1RGBUNormBlock = 131,
			BC1RGBSRGBBlock = 132,
			BC1RGBAUNormBlock = 133,
			BC1RGBASRGBBlock = 134,
			BC2UNormBlock = 135,
			BC2SRGBBlock = 136,
			BC3UNormBlock = 137,
			BC3SRGBBlock = 138,
			BC4UNormBlock = 139,
			BC4SNormBlock = 140,
			BC5UNormBlock = 141,
			BC5SNormBlock = 142,
			BC6HUFloatBlock = 143,
			BC6HSFloatBlock = 144,
			BC7UNormBlock = 145,
			BC7SRGBBlock = 146,
			ETC2R8G8B8UNormBlock = 147,
			ETC2R8G8B8SRGBBlock = 148,
			ETC2R8G8B8A1UNormBlock = 149,
			ETC2R8G8B8A1SRGBBlock = 150,
			ETC2R8G8B8A8UNormBlock = 151,
			ETC2R8G8B8A8SRGBBlock = 152,
			EACR11UNormBlock = 153,
			EACR11SNormBlock = 154,
			EACR11G11UNormBlock = 155,
			EACR11G11SNormBlock = 156,
			ASTC4x4UNormBlock = 157,
			ASTC4x4SRGBBlock = 158,
			ASTC5x4UNormBlock = 159,
			ASTC5x4SRGBBlock = 160,
			ASTC5x5UNormBlock = 161,
			ASTC5x5SRGBBlock = 162,
			ASTC6x5UNormBlock = 163,
			ASTC6x5SRGBBlock = 164,
			ASTC6x6UNormBlock = 165,
			ASTC6x6SRGBBlock = 166,
			ASTC8x5UNormBlock = 167,
			ASTC8x5SRGBBlock = 168,
			ASTC8x6UNormBlock = 169,
			ASTC8x6SRGBBlock = 170,
			ASTC8x8UNormBlock = 171,
			ASTC8x8SRGBBlock = 172,
			ASTC10x5UNormBlock = 173,
			ASTC10x5SRGBBlock = 174,
			ASTC10x6UNormBlock = 175,
			ASTC10x6SRGBBlock = 176,
			ASTC10x8UNormBlock = 177,
			ASTC10x8SRGBBlock = 178,
			ASTC10x10UNormBlock = 179,
			ASTC10x10SRGBBlock = 180,
			ASTC12x10UNormBlock = 181,
			ASTC12x10SRGBBlock = 182,
			ASTC12x12UNormBlock = 183,
			ASTC12x12SRGBBlock = 184,
		};

		enum class GraphicsTextureDim : std::uint8_t
		{
			Texture2D = 0,
			Texture2DArray = 1,
			Texture3D = 2,
			Cube = 3,
			CubeArray = 4,
		};

		enum class GraphicsSamplerAnis : std::uint8_t
		{
			Anis0 = 0,
			Anis1 = 1,
			Anis2 = 2,
			Anis4 = 3,
			Anis8 = 4,
			Anis16 = 5,
			Anis32 = 6,
			Anis64 = 7,
		};

		enum class GraphicsSamplerWrap : std::uint8_t
		{
			None = 0,
			Repeat = 1,
			Mirror = 2,
			ClampToEdge = 3,
		};

		enum class GraphicsSamplerFilter : std::uint8_t
		{
			Nearest = 0,
			Linear = 1,
			NearestMipmapLinear = 2,
			NearestMipmapNearest = 3,
			LinearMipmapNearest = 4,
			LinearMipmapLinear = 5,
		};

		struct GraphicsSampleFlagBits
		{
			enum Flags
			{
				bit1 = 0x00000001,
				bit2 = 0x00000002,
				bit4 = 0x00000004,
				bit8 = 0x00000008,
				bit16 = 0x00000010,
				bit32 = 0x00000020,
				bit64 = 0x00000040,
			};
		};

		typedef std::uint32_t GraphicsSampleFlags;

		enum class GraphicsDataType : std::uint8_t
		{
			None = 0,
			TransferSrc = 1,
			TransferDst = 2,
			UniformTexelBuffer = 3,
			UniformBuffer = 4,
			StorageTexelBuffer = 5,
			StorageBuffer = 6,
			StorageVertexBuffer = 7,
			StorageIndexBuffer = 8,
			IndirectBiffer = 9,
		};

		enum class GraphicsVertexType : std::uint8_t
		{
			PointList = 0,
			LineList = 1,
			LineStrip = 2,
			TriangleList = 3,
			TriangleStrip = 4,
			TriangleFan = 5,
			LineListWithAdjacency = 6,
			LineStripWithAdjacency = 7,
			TriangleListWithAdjacency = 8,
			TriangleStripWithAdjacency = 9,
			PatchList = 10,
		};

		enum class GraphicsVertexDivisor : std::uint8_t
		{
			Vertex = 0,
			Instance = 1,
		};

		enum class GraphicsIndexType : std::uint8_t
		{
			UInt16 = 0,
			UInt32 = 2,
		};

		struct GraphicsShaderStageFlagBits
		{
			enum Flags
			{
				NoneBit = 0x0,
				VertexBit = 0x1,
				FragmentBit = 0x2,
				GeometryBit = 0x4,
				ComputeBit = 0x8,
				TessEvaluationBit = 0x10,
				TessControlBit = 0x20,
				All = 0x7FFFFFFF,
				BitCount = 7,
				BeginBit = NoneBit,
				EndBit = TessControlBit,
				FlagBitsMaxEnum = 0x7FFFFFFF
			};
		};

		typedef std::uint32_t GraphicsShaderStageFlags;

		enum class GraphicsShaderLang : std::uint8_t
		{
			None = 0,
			HLSL = 1,
			HLSLbytecodes = 2,
			GLSL = 3,
		};

		enum class GraphicsShaderModel : std::uint8_t
		{
			Level2X = 0,
			Level3X = 1,
			Level4X = 2,
			Level5X = 3,
			Level6X = 4,
		};

		enum class GraphicsUniformType : std::uint8_t
		{
			Null = 0,
			Boolean = 1,
			Int = 5,
			Int2 = 6,
			Int3 = 7,
			Int4 = 8,
			UInt = 9,
			UInt2 = 10,
			UInt3 = 11,
			UInt4 = 12,
			Float = 13,
			Float2 = 14,
			Float3 = 15,
			Float4 = 16,
			Float2x2 = 17,
			Float3x3 = 18,
			Float4x4 = 19,
			IntArray = 24,
			Int2Array = 25,
			Int3Array = 26,
			Int4Array = 27,
			UIntArray = 28,
			UInt2Array = 29,
			UInt3Array = 30,
			UInt4Array = 31,
			FloatArray = 32,
			Float2Array = 33,
			Float3Array = 34,
			Float4Array = 35,
			Float2x2Array = 36,
			Float3x3Array = 37,
			Float4x4Array = 38,
			Sampler = 39,
			SamplerImage = 40,
			CombinedImageSampler = 41,
			StorageImage = 42,
			StorageTexelBuffer = 43,
			StorageBuffer = 44,
			StorageBufferDynamic = 45,
			UniformTexelBuffer = 46,
			UniformBuffer = 47,
			UniformBufferDynamic = 48,
			InputAttachment = 49,
		};

		enum class GraphicsImageLayout : std::uint8_t
		{
			Undefined = 0,
			General = 1,
			ColorAttachmentOptimal = 2,
			DepthStencilAttachmentOptimal = 3,
			DepthStencilReadOnlyOptimal = 4,
			ShaderReadOnlyOptimal = 5,
			TransferSrcOptimal = 6,
			TransferDstOptimal = 7,
			Preinitialized = 8,
			PresentSrcKhr = 9,
		};

		enum class GraphicsImageTiling : std::uint8_t
		{
			Undefined = 0,
			Optimal = 1,
			Linear = 2,
		};

		struct GraphicsViewUsageFlagBits
		{
			enum Flags
			{
				TransferSrcBit = 0x00000001,
				TransferDstBit = 0x00000002,
				SampledBit = 0x00000004,
				StorageBit = 0x00000008,
				ColorAttachmentBit = 0x00000010,
				DepthStencilAttachmentBit = 0x00000020,
				TransientAttachmentBit = 0x00000040,
				InputAttachmentBit = 0x00000080,
			};
		};

		typedef std::uint32_t GraphicsViewUsageFlags;

		struct GraphicsUsageFlagBits
		{
			enum Flags
			{
				ReadBit = 0x00000001,
				WriteBit = 0x00000002,
				PersistentBit = 0x00000004,
				CoherentBit = 0x00000008,
				FlushExplicitBit = 0x00000010,
				DynamicStorageBit = 0x00000020,
				ClientStorageBit = 0x00000040
			};
		};

		typedef std::uint32_t GraphicsUsageFlags;

		struct GraphicsAccessFlagBits
		{
			enum Flags
			{
				MapReadBit = 0x00000001,
				MapWriteBit = 0x00000002,
				UnsynchronizedBit = 0x00000004
			};
		};

		typedef std::uint32_t GraphicsAccessFlags;

		enum class GraphicsCommandType : std::uint8_t
		{
			Graphics = 0,
			Compute = 1,
			Bundle = 2,
			Copy = 3,
		};

		struct GraphicsCommandQueueFlagBits
		{
			enum Flags
			{
				NoneBit = 0,
				DisableGpuTimeOutBit = 0x1
			};
		};

		typedef std::uint32_t GraphicsCommandQueueFlags;

		enum class GraphicsCommandQueuePriority : std::uint8_t
		{
			Normal = 0,
			High = 1,
		};

		struct GraphicsCommandPoolFlagBits
		{
			enum Flags
			{
				TransientBit = 0x00000001,
				ResetCommandBufferBit = 0x00000002
			};
		};

		typedef std::uint32_t GraphicsCommandPoolFlags;
	}
}

#endif