#ifndef OCTOON_EGL2_TYPES_H_
#define OCTOON_EGL2_TYPES_H_

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

#include <EGL\egl.h>
#include <GLES2\gl2.h>
#include <GLES2\gl2ext.h>

#if defined(GL_KHR_debug)
#	define GL_DEBUG_OUTPUT_SYNCHRONOUS   GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR
#   define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_KHR
#   define GL_DEBUG_CALLBACK_FUNCTION    GL_DEBUG_CALLBACK_FUNCTION_KHR
#   define GL_DEBUG_CALLBACK_USER_PARAM  GL_DEBUG_CALLBACK_USER_PARAM_KHR
#   define GL_DEBUG_SOURCE_API           GL_DEBUG_SOURCE_API_KHR
#   define GL_DEBUG_SOURCE_WINDOW_SYSTEM GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR
#   define GL_DEBUG_SOURCE_SHADER_COMPILER GL_DEBUG_SOURCE_SHADER_COMPILER_KHR
#   define GL_DEBUG_SOURCE_THIRD_PARTY   GL_DEBUG_SOURCE_THIRD_PARTY_KHR
#   define GL_DEBUG_SOURCE_APPLICATION   GL_DEBUG_SOURCE_APPLICATION_KHR
#   define GL_DEBUG_SOURCE_OTHER         GL_DEBUG_SOURCE_OTHER_KHR
#   define GL_DEBUG_TYPE_ERROR           GL_DEBUG_TYPE_ERROR_KHR
#   define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR
#   define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR
#   define GL_DEBUG_TYPE_PORTABILITY     GL_DEBUG_TYPE_PORTABILITY_KHR
#   define GL_DEBUG_TYPE_PERFORMANCE     GL_DEBUG_TYPE_PERFORMANCE_KHR
#   define GL_DEBUG_TYPE_OTHER           GL_DEBUG_TYPE_OTHER_KHR
#   define GL_DEBUG_TYPE_MARKER          GL_DEBUG_TYPE_MARKER_KHR
#   define GL_DEBUG_TYPE_PUSH_GROUP      GL_DEBUG_TYPE_PUSH_GROUP_KHR
#   define GL_DEBUG_TYPE_POP_GROUP       GL_DEBUG_TYPE_POP_GROUP_KHR
#   define GL_DEBUG_SEVERITY_NOTIFICATION GL_DEBUG_SEVERITY_NOTIFICATION_KHR
#   define GL_MAX_DEBUG_GROUP_STACK_DEPTH GL_MAX_DEBUG_GROUP_STACK_DEPTH_KHR
#   define GL_DEBUG_GROUP_STACK_DEPTH    GL_DEBUG_GROUP_STACK_DEPTH_KHR
#   define GL_BUFFER                     GL_BUFFER_KHR
#   define GL_SHADER                     GL_SHADER_KHR
#   define GL_PROGRAM                    GL_PROGRAM_KHR
#   define GL_VERTEX_ARRAY               GL_VERTEX_ARRAY_KHR
#   define GL_QUERY                      GL_QUERY_KHR
#   define GL_MAX_LABEL_LENGTH           GL_MAX_LABEL_LENGTH_KHR
#   define GL_MAX_DEBUG_MESSAGE_LENGTH   GL_MAX_DEBUG_MESSAGE_LENGTH_KHR
#   define GL_MAX_DEBUG_LOGGED_MESSAGES  GL_MAX_DEBUG_LOGGED_MESSAGES_KHR
#   define GL_DEBUG_LOGGED_MESSAGES      GL_DEBUG_LOGGED_MESSAGES_KHR
#   define GL_DEBUG_SEVERITY_HIGH        GL_DEBUG_SEVERITY_HIGH_KHR
#   define GL_DEBUG_SEVERITY_MEDIUM      GL_DEBUG_SEVERITY_MEDIUM_KHR
#   define GL_DEBUG_SEVERITY_LOW         GL_DEBUG_SEVERITY_LOW_KHR
#   define GL_DEBUG_OUTPUT               GL_DEBUG_OUTPUT_KHR
#   define GL_CONTEXT_FLAG_DEBUG_BIT     GL_CONTEXT_FLAG_DEBUG_BIT_KHR
#   define GL_STACK_OVERFLOW             GL_STACK_OVERFLOW_KHR
#   define GL_STACK_UNDERFLOW            GL_STACK_UNDERFLOW_KHR
#   define GL_FRAMEBUFFER_SRGB           GL_FRAMEBUFFER_SRGB_EXT

#   define GLAPIENTRY GL_APIENTRY

#   define glDebugMessageCallback glDebugMessageCallbackKHR
#   define glDebugMessageControl glDebugMessageControlKHR
#endif

#ifndef GL_INVALID_INDEX
#	define GL_INVALID_INDEX 0xFFFFFFFFu
#endif

#if GL_DEBUG
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_CHECK(x) do { x; EGL2Check::checkError(); } while (false)
#else
#	define GL_CHECK(x) x
#endif

#if defined(__DEBUG__)
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_PLATFORM_LOG(format) EGL2Check::debugOutput(format)
#else
#	define GL_PLATFORM_LOG(format)
#endif

#if defined(__DEBUG__)
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_PLATFORM_ASSERT(expr, format) if (!(expr)) { EGL2Check::debugOutput(format); assert(expr); }
#else
#	define GL_PLATFORM_ASSERT(expr, format)
#endif

namespace octoon 
{
	namespace graphics
	{
		enum EGL2Features
		{
			EGL2_KHR_blend_equation_advanced,
			EGL2_KHR_blend_equation_advanced_coherent,
			EGL2_KHR_context_flush_control,
			EGL2_KHR_debug,
			EGL2_KHR_robust_buffer_access_behavior,
			EGL2_KHR_robustness,
			EGL2_KHR_texture_compression_astc_hdr,
			EGL2_KHR_texture_compression_astc_ldr,
			EGL2_OES_EGL_image,
			EGL2_OES_EGL_image_external,
			EGL2_OES_compressed_ETC1_RGB8_sub_texture,
			EGL2_OES_compressed_ETC1_RGB8_texture,
			EGL2_OES_compressed_paletted_texture,
			EGL2_OES_copy_image,
			EGL2_OES_depth24,
			EGL2_OES_depth32,
			EGL2_OES_depth_texture,
			EGL2_OES_draw_buffers_indexed,
			EGL2_OES_draw_elements_base_vertex,
			EGL2_OES_element_index_uint,
			EGL2_OES_fbo_render_mipmap,
			EGL2_OES_fragment_precision_high,
			EGL2_OES_geometry_point_size,
			EGL2_OES_geometry_shader,
			EGL2_OES_get_program_binary,
			EGL2_OES_gpu_shader5,
			EGL2_OES_mapbuffer,
			EGL2_OES_packed_depth_stencil,
			EGL2_OES_primitive_bounding_box,
			EGL2_OES_required_internalformat,
			EGL2_OES_rgb8_rgba8,
			EGL2_OES_sample_shading,
			EGL2_OES_sample_variables,
			EGL2_OES_shader_image_atomic,
			EGL2_OES_shader_io_blocks,
			EGL2_OES_shader_multisample_interpolation,
			EGL2_OES_standard_derivatives,
			EGL2_OES_stencil1,
			EGL2_OES_stencil4,
			EGL2_OES_surfaceless_context,
			EGL2_OES_tessellation_point_size,
			EGL2_OES_tessellation_shader,
			EGL2_OES_texture_3D,
			EGL2_OES_texture_border_clamp,
			EGL2_OES_texture_buffer,
			EGL2_OES_texture_compression_astc,
			EGL2_OES_texture_cube_map_array,
			EGL2_OES_texture_float,
			EGL2_OES_texture_float_linear,
			EGL2_OES_texture_half_float,
			EGL2_OES_texture_half_float_linear,
			EGL2_OES_texture_npot,
			EGL2_OES_texture_stencil8,
			EGL2_OES_texture_storage_multisample_2d_array,
			EGL2_OES_texture_view,
			EGL2_OES_vertex_array_object,
			EGL2_OES_vertex_half_float,
			EGL2_OES_vertex_type_10_10_10_2,
			EGL2_AMD_compressed_3DC_texture,
			EGL2_AMD_compressed_ATC_texture,
			EGL2_AMD_performance_monitor,
			EGL2_AMD_program_binary_Z400,
			EGL2_ANDROID_extension_pack_es31a,
			EGL2_ANGLE_depth_texture,
			EGL2_ANGLE_framebuffer_blit,
			EGL2_ANGLE_framebuffer_multisample,
			EGL2_ANGLE_instanced_arrays,
			EGL2_ANGLE_pack_reverse_row_order,
			EGL2_ANGLE_program_binary,
			EGL2_ANGLE_texture_compression_dxt3,
			EGL2_ANGLE_texture_compression_dxt5,
			EGL2_ANGLE_texture_usage,
			EGL2_ANGLE_translated_shader_source,
			EGL2_APPLE_clip_distance,
			EGL2_APPLE_color_buffer_packed_float,
			EGL2_APPLE_copy_texture_levels,
			EGL2_APPLE_framebuffer_multisample,
			EGL2_APPLE_rgb_422,
			EGL2_APPLE_sync,
			EGL2_APPLE_texture_format_BGRA8888,
			EGL2_APPLE_texture_max_level,
			EGL2_APPLE_texture_packed_float,
			EGL2_ARM_mali_program_binary,
			EGL2_ARM_mali_shader_binary,
			EGL2_ARM_rgba8,
			EGL2_ARM_shader_framebuffer_fetch,
			EGL2_ARM_shader_framebuffer_fetch_depth_stencil,
			EGL2_DMP_program_binary,
			EGL2_DMP_shader_binary,
			EGL2_EXT_base_instance,
			EGL2_EXT_blend_minmax,
			EGL2_EXT_color_buffer_half_float,
			EGL2_EXT_color_buffer_float,
			EGL2_EXT_copy_image,
			EGL2_EXT_debug_label,
			EGL2_EXT_debug_marker,
			EGL2_EXT_discard_framebuffer,
			EGL2_EXT_disjoint_timer_query,
			EGL2_EXT_draw_buffers,
			EGL2_EXT_draw_buffers_indexed,
			EGL2_EXT_draw_elements_base_vertex,
			EGL2_EXT_draw_instanced,
			EGL2_EXT_geometry_point_size,
			EGL2_EXT_geometry_shader,
			EGL2_EXT_gpu_shader5,
			EGL2_EXT_instanced_arrays,
			EGL2_EXT_map_buffer_range,
			EGL2_EXT_multi_draw_arrays,
			EGL2_EXT_multi_draw_indirect,
			EGL2_EXT_multisampled_render_to_texture,
			EGL2_EXT_multiview_draw_buffers,
			EGL2_EXT_occlusion_query_boolean,
			EGL2_EXT_primitive_bounding_box,
			EGL2_EXT_pvrtc_sRGB,
			EGL2_EXT_read_format_bgra,
			EGL2_EXT_render_snorm,
			EGL2_EXT_robustness,
			EGL2_EXT_sRGB,
			EGL2_EXT_sRGB_write_control,
			EGL2_EXT_separate_shader_objects,
			EGL2_EXT_shader_framebuffer_fetch,
			EGL2_EXT_shader_implicit_conversions,
			EGL2_EXT_shader_integer_mix,
			EGL2_EXT_shader_io_blocks,
			EGL2_EXT_shader_pixel_local_storage,
			EGL2_EXT_shader_texture_lod,
			EGL2_EXT_shadow_samplers,
			EGL2_EXT_tessellation_point_size,
			EGL2_EXT_tessellation_shader,
			EGL2_EXT_texture_border_clamp,
			EGL2_EXT_texture_buffer,
			EGL2_EXT_texture_compression_dxt1,
			EGL2_EXT_texture_compression_s3tc,
			EGL2_EXT_texture_cube_map_array,
			EGL2_EXT_texture_filter_anisotropic,
			EGL2_EXT_texture_format_BGRA8888,
			EGL2_EXT_texture_norm16,
			EGL2_EXT_texture_rg,
			EGL2_EXT_texture_sRGB_decode,
			EGL2_EXT_texture_storage,
			EGL2_EXT_texture_type_2_10_10_10_REV,
			EGL2_EXT_texture_view,
			EGL2_EXT_unpack_subimage,
			EGL2_FJ_shader_binary_GCCSO,
			EGL2_IMG_multisampled_render_to_texture,
			EGL2_IMG_program_binary,
			EGL2_IMG_read_format,
			EGL2_IMG_shader_binary,
			EGL2_IMG_texture_compression_pvrtc,
			EGL2_IMG_texture_compression_pvrtc2,
			EGL2_INTEL_performance_query,
			EGL2_NV_bindless_texture,
			EGL2_NV_blend_equation_advanced,
			EGL2_NV_blend_equation_advanced_coherent,
			EGL2_NV_conditional_render,
			EGL2_NV_copy_buffer,
			EGL2_NV_coverage_sample,
			EGL2_NV_depth_nonlinear,
			EGL2_NV_draw_buffers,
			EGL2_NV_draw_instanced,
			EGL2_NV_explicit_attrib_location,
			EGL2_NV_fbo_color_attachments,
			EGL2_NV_fence,
			EGL2_NV_framebuffer_blit,
			EGL2_NV_framebuffer_multisample,
			EGL2_NV_generate_mipmap_sRGB,
			EGL2_NV_image_formats,
			EGL2_NV_instanced_arrays,
			EGL2_NV_internalformat_sample_query,
			EGL2_NV_non_square_matrices,
			EGL2_NV_path_rendering,
			EGL2_NV_read_buffer,
			EGL2_NV_read_buffer_front,
			EGL2_NV_read_depth,
			EGL2_NV_read_depth_stencil,
			EGL2_NV_read_stencil,
			EGL2_NV_sRGB_formats,
			EGL2_NV_shader_noperspective_interpolation,
			EGL2_NV_shadow_samplers_array,
			EGL2_NV_shadow_samplers_cube,
			EGL2_NV_texture_border_clamp,
			EGL2_NV_texture_compression_s3tc_update,
			EGL2_NV_texture_npot_2D_mipmap,
			EGL2_NV_viewport_array,
			EGL2_QCOM_alpha_test,
			EGL2_QCOM_binning_control,
			EGL2_QCOM_driver_control,
			EGL2_QCOM_extended_get,
			EGL2_QCOM_extended_get2,
			EGL2_QCOM_perfmon_global_mode,
			EGL2_QCOM_tiled_rendering,
			EGL2_QCOM_writeonly_rendering,
			EGL2_VIV_shader_binary,
			EGL2_BeginRange = EGL2_KHR_blend_equation_advanced,
			EGL2_EndRange = EGL2_VIV_shader_binary,
			EGL2_RangeSize = (EGL2_EndRange - EGL2_BeginRange + 1),
			EGL2_MaxEnum = 0x7FFFFFFF
		};

		struct GPUfbconfig
		{
			int redSize;
			int greenSize;
			int blueSize;
			int alphaSize;

			int bufferSize;
			int depthSize;
			int stencilSize;

			int accumSize;
			int accumRedSize;
			int accumGreenSize;
			int accumBlueSize;
			int accumAlphaSize;

			int samples;
		};

		struct GPUctxconfig
		{
			int api;
			int major;
			int minor;
			int forward;
			int profile;
			int robustness;
			int release;
			int multithread;

			EGLContext share;
		};

		typedef std::shared_ptr<class EGL2Device> EGL2DevicePtr;
		typedef std::shared_ptr<class EGL2Surface> EGL2SurfacePtr;
		typedef std::shared_ptr<class EGL2Swapchain> EGL2SwapchainPtr;
		typedef std::shared_ptr<class EGL2DeviceContext> EGL2DeviceContextPtr;
		typedef std::shared_ptr<class EGL2FramebufferLayout> EGL2FramebufferLayoutPtr;
		typedef std::shared_ptr<class EGL2Framebuffer> EGL2FramebufferPtr;
		typedef std::shared_ptr<class EGL2Shader> EGL2ShaderPtr;
		typedef std::shared_ptr<class EGL2Program> EGL2ProgramPtr;
		typedef std::shared_ptr<class EGL2GraphicsData> EGL2GraphicsDataPtr;
		typedef std::shared_ptr<class EGL2InputLayout> EGL2InputLayoutPtr;
		typedef std::shared_ptr<class EGL2GraphicsState> EGL2GraphicsStatePtr;
		typedef std::shared_ptr<class EGL2Texture> EGL2TexturePtr;
		typedef std::shared_ptr<class EGL2Sampler> EGL2SamplerPtr;
		typedef std::shared_ptr<class EGL2Pipeline> EGL2PipelinePtr;
		typedef std::shared_ptr<class EGL2DescriptorSetPool> EGL2DescriptorSetPoolPtr;
		typedef std::shared_ptr<class EGL2DescriptorSet> EGL2DescriptorSetPtr;
		typedef std::shared_ptr<class EGL2DescriptorSetLayout> EGL2DescriptorSetLayoutPtr;
		typedef std::shared_ptr<class EGL2GraphicsAttribute> EGL2GraphicsAttributePtr;
		typedef std::shared_ptr<class EGL2GraphicsUniform> EGL2GraphicsUniformPtr;
		typedef std::shared_ptr<class EGL2GraphicsUniformBlock> EGL2GraphicsUniformBlockPtr;

		typedef std::weak_ptr<class EGL2Device> EGL2DeviceWeakPtr;
		typedef std::weak_ptr<class EGL2Surface> EGL2SurfaceWeakPtr;
		typedef std::weak_ptr<class EGL2Swapchain> EGL2SwapchainWeakPtr;
		typedef std::weak_ptr<class EGL2DeviceContext> EGL2DeviceContextWeakPtr;
		typedef std::weak_ptr<class EGL2FramebufferLayout> EGL2FramebufferLayoutWeakPtr;
		typedef std::weak_ptr<class EGL2Framebuffer> EGL2FramebufferWeakPtr;
		typedef std::weak_ptr<class EGL2Shader> EGL2ShaderWeakPtr;
		typedef std::weak_ptr<class EGL2Program> EGL2ProgramWeakPtr;
		typedef std::weak_ptr<class EGL2GraphicsData> EGL2GraphicsDataWeakPtr;
		typedef std::weak_ptr<class EGL2InputLayout> EGL2InputLayoutWeakPtr;
		typedef std::weak_ptr<class EGL2GraphicsState> EGL2GraphicsStateWeakPtr;
		typedef std::weak_ptr<class EGL2Texture> EGL2TextureWeakPtr;
		typedef std::weak_ptr<class EGL2Sampler> EGL2SamplerWeakPtr;
		typedef std::weak_ptr<class EGL2Pipeline> EGL2PipelineWeakPtr;
		typedef std::weak_ptr<class EGL2DescriptorSetPool> EGL2DescriptorSetPoolWeakPtr;
		typedef std::weak_ptr<class EGL2DescriptorSet> EGL2DescriptorSetWeakPtr;
		typedef std::weak_ptr<class EGL2DescriptorSetLayout> EGL2DescriptorSetLayoutWeakPtr;
		typedef std::weak_ptr<class EGL2GraphicsAttribute> EGL2GraphicsAttributeWeakPtr;
		typedef std::weak_ptr<class EGL2GraphicsUniform> EGL2GraphicsUniformWeakPtr;
		typedef std::weak_ptr<class EGL2GraphicsUniformBlock> EGL2GraphicsUniformBlockWeakPtr;

		typedef std::vector<EGL2ShaderPtr> EGL2Shaders;

		struct EGL2VertexBuffer
		{
			bool needUpdate;
			std::intptr_t offset;
			EGL2GraphicsDataPtr vbo;
		};

		typedef std::vector<EGL2VertexBuffer> EGL2VertexBuffers;

		class EGL2Types
		{
		public:
			static bool setup() noexcept;

			static GLenum asVertexType(GraphicsVertexType type) noexcept;
			static GLenum asVertexFormat(GraphicsFormat format) noexcept;
			static GLenum asIndexType(GraphicsIndexType type) noexcept;
			static GLenum asShaderStage(GraphicsShaderStageFlags stage) noexcept;
			static GLenum asTextureTarget(GraphicsTextureDim dim) noexcept;
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
			static GLboolean isSupportFeature(EGL2Features features) noexcept;
			static GLboolean isStencilFormat(GraphicsFormat format) noexcept;
			static GLboolean isDepthFormat(GraphicsFormat format) noexcept;
			static GLboolean isDepthStencilFormat(GraphicsFormat format) noexcept;
			static GLboolean isCompressedTexture(GraphicsFormat format) noexcept;

		private:
			static GLboolean _egl2init;
			static GLboolean _egl2Features[EGL2Features::EGL2_RangeSize];
		};

		class EGL2Check
		{
		public:
			static bool checkError() noexcept;

			static void debugOutput(const char* message, ...) noexcept;
		};
	}

}

#endif