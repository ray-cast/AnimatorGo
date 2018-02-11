#ifndef OCTOON_OGL_TYPES_H_
#define OCTOON_OGL_TYPES_H_

#include <octoon/graphics/graphics_system.h>
#include <octoon/graphics/graphics_device.h>
#include <octoon/graphics/graphics_device_property.h>
#include <octoon/graphics/graphics_swapchain.h>
#include <octoon/graphics/graphics_context.h>
#include <octoon/graphics/graphics_data.h>
#include <octoon/graphics/graphics_state.h>
#include <octoon/graphics/graphics_sampler.h>
#include <octoon/graphics/graphics_texture.h>
#include <octoon/graphics/graphics_framebuffer.h>
#include <octoon/graphics/graphics_shader.h>
#include <octoon/graphics/graphics_pipeline.h>
#include <octoon/graphics/graphics_descriptor.h>
#include <octoon/graphics/graphics_input_layout.h>
#include <octoon/graphics/graphics_variant.h>

#include "ogl_basic.h"

namespace octoon
{
	namespace graphics
	{
#if _BUILD_OPENGL
#	ifdef GLEW_MX
		extern GLEWContext _glewctx;
#	define glewGetContext() (&_glewctx)
#	endif

#  if defined(__MINGW32__) || defined(__CYGWIN__)
#    define GLEXT_APIENTRY __stdcall
#  elif (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED) || defined(__BORLANDC__)
#    define GLEXT_APIENTRY __stdcall
#  else
#    define GLEXT_APIENTRY
#  endif
#endif

#if GL_DEBUG
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_CHECK(x) do { x; OGLCheck::checkError(); } while (false)
#else
#	define GL_CHECK(x) x
#endif

#if defined(__DEBUG__)
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_PLATFORM_ASSERT(expr, format) if (!(expr)) { OGLCheck::debugOutput(format); assert(expr); }
#else
#	define GL_PLATFORM_ASSERT(expr, format)
#endif

		typedef std::shared_ptr<class OGLDevice> OGLDevicePtr;
		typedef std::shared_ptr<class OGLDeviceProperty> OGLDevicePropertyPtr;
		typedef std::shared_ptr<class OGLSurface> OGLSurfacePtr;
		typedef std::shared_ptr<class OGLSwapchain> OGLSwapchainPtr;
		typedef std::shared_ptr<class OGLDeviceContext> OGLDeviceContextPtr;
		typedef std::shared_ptr<class OGLFramebufferLayout> OGLFramebufferLayoutPtr;
		typedef std::shared_ptr<class OGLFramebuffer> OGLFramebufferPtr;
		typedef std::shared_ptr<class OGLShader> OGLShaderPtr;
		typedef std::shared_ptr<class OGLProgram> OGLProgramPtr;
		typedef std::shared_ptr<class OGLGraphicsData> OGLGraphicsDataPtr;
		typedef std::shared_ptr<class OGLInputLayout> OGLInputLayoutPtr;
		typedef std::shared_ptr<class OGLGraphicsState> OGLGraphicsStatePtr;
		typedef std::shared_ptr<class OGLTexture> OGLTexturePtr;
		typedef std::shared_ptr<class OGLSampler> OGLSamplerPtr;
		typedef std::shared_ptr<class OGLPipeline> OGLPipelinePtr;
		typedef std::shared_ptr<class OGLDescriptorSetPool> OGLDescriptorSetPoolPtr;
		typedef std::shared_ptr<class OGLDescriptorSet> OGLDescriptorSetPtr;
		typedef std::shared_ptr<class OGLDescriptorSetLayout> OGLDescriptorSetLayoutPtr;
		typedef std::shared_ptr<class OGLGraphicsAttribute> OGLGraphicsAttributePtr;
		typedef std::shared_ptr<class OGLGraphicsUniform> OGLGraphicsUniformPtr;
		typedef std::shared_ptr<class OGLGraphicsUniformBlock> OGLGraphicsUniformBlockPtr;

		typedef std::shared_ptr<class OGLCoreDeviceContext> OGLCoreDeviceContextPtr;
		typedef std::shared_ptr<class OGLCoreFramebuffer> OGLCoreFramebufferPtr;
		typedef std::shared_ptr<class OGLCoreGraphicsData> OGLCoreGraphicsDataPtr;
		typedef std::shared_ptr<class OGLCoreTexture> OGLCoreTexturePtr;
		typedef std::shared_ptr<class OGLCorePipeline> OGLCorePipelinePtr;
		typedef std::shared_ptr<class OGLCoreDescriptorSet> OGLCoreDescriptorSetPtr;

		typedef std::weak_ptr<class OGLDevice> OGLDeviceWeakPtr;
		typedef std::weak_ptr<class OGLSurface> OGLSurfaceWeakPtr;
		typedef std::weak_ptr<class OGLSwapchain> OGLSwapchainWeakPtr;
		typedef std::weak_ptr<class OGLDeviceContext> OGLDeviceContextWeakPtr;
		typedef std::weak_ptr<class OGLFramebufferLayout> OGLFramebufferLayoutWeakPtr;
		typedef std::weak_ptr<class OGLFramebuffer> OGLFramebufferWeakPtr;
		typedef std::weak_ptr<class OGLShader> OGLShaderWeakPtr;
		typedef std::weak_ptr<class OGLProgram> OGLProgramWeakPtr;
		typedef std::weak_ptr<class OGLGraphicsData> OGLGraphicsDataWeakPtr;
		typedef std::weak_ptr<class OGLInputLayout> OGLInputLayoutWeakPtr;
		typedef std::weak_ptr<class OGLGraphicsState> OGLGraphicsStateWeakPtr;
		typedef std::weak_ptr<class OGLTexture> OGLTextureWeakPtr;
		typedef std::weak_ptr<class OGLSampler> OGLSamplerWeakPtr;
		typedef std::weak_ptr<class OGLPipeline> OGLPipelineWeakPtr;
		typedef std::weak_ptr<class OGLDescriptorSetPool> OGLDescriptorSetPoolWeakPtr;
		typedef std::weak_ptr<class OGLDescriptorSet> OGLDescriptorSetWeakPtr;
		typedef std::weak_ptr<class OGLDescriptorSetLayout> OGLDescriptorSetLayoutWeakPtr;
		typedef std::weak_ptr<class OGLGraphicsAttribute> OGLGraphicsAttributeWeakPtr;
		typedef std::weak_ptr<class OGLGraphicsUniform> OGLGraphicsUniformWeakPtr;
		typedef std::weak_ptr<class OGLGraphicsUniformBlock> OGLGraphicsUniformBlockWeakPtr;

		typedef std::vector<OGLShaderPtr> OGLShaders;
		typedef std::vector<OGLGraphicsDataPtr> OGLGraphicsDatas;

		struct OGLVertexBuffer
		{
			bool needUpdate;
			std::intptr_t offset;
			OGLGraphicsDataPtr vbo;
		};

		struct OGLCoreVertexBuffer
		{
			bool needUpdate;
			std::intptr_t offset;
			OGLCoreGraphicsDataPtr vbo;
		};

		typedef std::vector<OGLVertexBuffer> OGLVertexBuffers;
		typedef std::vector<OGLCoreVertexBuffer> OGLCoreVertexBuffers;

		class OGLTypes
		{
		public:
			static GLenum asVertexType(GraphicsVertexType type) noexcept;
			static GLenum asVertexFormat(GraphicsFormat format) noexcept;
			static GLenum asIndexType(GraphicsIndexType type) noexcept;
			static GLenum asShaderStage(GraphicsShaderStageFlagBits type) noexcept;
			static GLenum asTextureTarget(GraphicsTextureDim mapping, bool multisampler) noexcept;
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

		class OGLCheck
		{
		public:
			static bool checkError() noexcept;
		};
	}
}

#endif