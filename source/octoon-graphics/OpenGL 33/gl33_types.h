#ifndef OCTOON_GL33_TYPES_H_
#define OCTOON_GL33_TYPES_H_

#include "ogl_swapchain.h"

namespace octoon
{
	namespace graphics
	{
#if GL_DEBUG
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_CHECK(x) do { x; GL33Check::checkError(); } while (false)
#else
#	define GL_CHECK(x) x
#endif

#if defined(__DEBUG__)
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_PLATFORM_ASSERT(expr, format) if (!(expr)) { GL33Check::debugOutput(format); assert(expr); }
#else
#	define GL_PLATFORM_ASSERT(expr, format)
#endif

		typedef std::shared_ptr<class GL33Device> GL33DevicePtr;
		typedef std::shared_ptr<class GL33DeviceProperty> GL33DevicePropertyPtr;
		typedef std::shared_ptr<class GL33Surface> GL33SurfacePtr;
		typedef std::shared_ptr<class OGLSwapchain> GL33SwapchainPtr;
		typedef std::shared_ptr<class GL33DeviceContext> GL33DeviceContextPtr;
		typedef std::shared_ptr<class GL33FramebufferLayout> GL33FramebufferLayoutPtr;
		typedef std::shared_ptr<class GL33Framebuffer> GL33FramebufferPtr;
		typedef std::shared_ptr<class GL33Shader> GL33ShaderPtr;
		typedef std::shared_ptr<class GL33Program> GL33ProgramPtr;
		typedef std::shared_ptr<class GL33GraphicsData> GL33GraphicsDataPtr;
		typedef std::shared_ptr<class GL33InputLayout> GL33InputLayoutPtr;
		typedef std::shared_ptr<class GL33GraphicsState> GL33GraphicsStatePtr;
		typedef std::shared_ptr<class GL33Texture> GL33TexturePtr;
		typedef std::shared_ptr<class GL33Sampler> GL33SamplerPtr;
		typedef std::shared_ptr<class GL33Pipeline> GL33PipelinePtr;
		typedef std::shared_ptr<class GL33DescriptorSetPool> GL33DescriptorSetPoolPtr;
		typedef std::shared_ptr<class GL33DescriptorSet> GL33DescriptorSetPtr;
		typedef std::shared_ptr<class GL33DescriptorSetLayout> GL33DescriptorSetLayoutPtr;
		typedef std::shared_ptr<class GL33GraphicsAttribute> GL33GraphicsAttributePtr;
		typedef std::shared_ptr<class GL33GraphicsUniform> GL33GraphicsUniformPtr;
		typedef std::shared_ptr<class GL33GraphicsUniformBlock> GL33GraphicsUniformBlockPtr;

		typedef std::shared_ptr<class GL45DeviceContext> GL45DeviceContextPtr;
		typedef std::shared_ptr<class GL45Framebuffer> GL45FramebufferPtr;
		typedef std::shared_ptr<class GL45GraphicsData> GL45GraphicsDataPtr;
		typedef std::shared_ptr<class GL45Texture> GL45TexturePtr;
		typedef std::shared_ptr<class GL45Pipeline> GL45PipelinePtr;
		typedef std::shared_ptr<class GL45DescriptorSet> GL45DescriptorSetPtr;

		typedef std::weak_ptr<class GL33Device> GL33DeviceWeakPtr;
		typedef std::weak_ptr<class GL33Surface> GL33SurfaceWeakPtr;
		typedef std::weak_ptr<class OGLSwapchain> GL33SwapchainWeakPtr;
		typedef std::weak_ptr<class GL33DeviceContext> GL33DeviceContextWeakPtr;
		typedef std::weak_ptr<class GL33FramebufferLayout> GL33FramebufferLayoutWeakPtr;
		typedef std::weak_ptr<class GL33Framebuffer> GL33FramebufferWeakPtr;
		typedef std::weak_ptr<class GL33Shader> GL33ShaderWeakPtr;
		typedef std::weak_ptr<class GL33Program> GL33ProgramWeakPtr;
		typedef std::weak_ptr<class GL33GraphicsData> GL33GraphicsDataWeakPtr;
		typedef std::weak_ptr<class GL33InputLayout> GL33InputLayoutWeakPtr;
		typedef std::weak_ptr<class GL33GraphicsState> GL33GraphicsStateWeakPtr;
		typedef std::weak_ptr<class GL33Texture> GL33TextureWeakPtr;
		typedef std::weak_ptr<class GL33Sampler> GL33SamplerWeakPtr;
		typedef std::weak_ptr<class GL33Pipeline> GL33PipelineWeakPtr;
		typedef std::weak_ptr<class GL33DescriptorSetPool> GL33DescriptorSetPoolWeakPtr;
		typedef std::weak_ptr<class GL33DescriptorSet> GL33DescriptorSetWeakPtr;
		typedef std::weak_ptr<class GL33DescriptorSetLayout> GL33DescriptorSetLayoutWeakPtr;
		typedef std::weak_ptr<class GL33GraphicsAttribute> GL33GraphicsAttributeWeakPtr;
		typedef std::weak_ptr<class GL33GraphicsUniform> GL33GraphicsUniformWeakPtr;
		typedef std::weak_ptr<class GL33GraphicsUniformBlock> GL33GraphicsUniformBlockWeakPtr;

		typedef std::vector<GL33ShaderPtr> GL33Shaders;
		typedef std::vector<GL33GraphicsDataPtr> GL33GraphicsDatas;

		struct GL33VertexBuffer
		{
			bool needUpdate;
			std::intptr_t offset;
			GL33GraphicsDataPtr vbo;
		};

		struct GL45VertexBuffer
		{
			bool needUpdate;
			std::intptr_t offset;
			GL45GraphicsDataPtr vbo;
		};

		typedef std::vector<GL33VertexBuffer> GL33VertexBuffers;
		typedef std::vector<GL45VertexBuffer> GL45VertexBuffers;

		class GL33Types
		{
		public:
			static GLenum asVertexType(GraphicsVertexType type) noexcept;
			static GLenum asVertexFormat(GraphicsFormat format) noexcept;
			static GLenum asIndexType(GraphicsIndexType type) noexcept;
			static GLenum asShaderStage(GraphicsShaderStageFlags type) noexcept;
			static GLenum asTextureTarget(GraphicsTextureDim mapping) noexcept;
			static GLenum asTextureFormat(GraphicsFormat format) noexcept;
			static GLenum asTextureType(GraphicsFormat format) noexcept;
			static GLenum asTextureInternalFormat(GraphicsFormat format) noexcept;
			static GLenum asCompareFunction(GraphicsCompareFunc func) noexcept;
			static GLenum asBlendFactor(GraphicsBlendFactor func) noexcept;
			static GLenum asBlendOperation(GraphicsBlendOp op) noexcept;
			static GLenum asCullMode(GraphicsCullMode mode) noexcept;
			static GLenum asFrontFace(GraphicsFrontFace face) noexcept;
			static GLenum asFillMode(GraphicsPolygonMode mode) noexcept;
			static GLenum asStencilOperation(GraphicsStencilOp stencilop) noexcept;
			static GLenum asSamplerWrap(GraphicsSamplerWrap wrap) noexcept;
			static GLenum asSamplerMinFilter(GraphicsSamplerFilter filter) noexcept;
			static GLenum asSamplerMagFilter(GraphicsSamplerFilter filter) noexcept;

			static GLsizei getFormatNum(GLenum format, GLenum type) noexcept;
			static GLsizei getCompressedTextureSize(GLsizei width, GLsizei height, GLsizei depth, GLenum internalFormat) noexcept;

			static GLboolean isNormFormat(GraphicsFormat format) noexcept;
			static GLboolean isStencilFormat(GraphicsFormat format) noexcept;
			static GLboolean isDepthFormat(GraphicsFormat format) noexcept;
			static GLboolean isDepthStencilFormat(GraphicsFormat format) noexcept;
			static GLboolean isCompressedTexture(GraphicsFormat format) noexcept;
		};

		class GL33Check
		{
		public:
			static bool checkError() noexcept;
		};
	}
}

#endif