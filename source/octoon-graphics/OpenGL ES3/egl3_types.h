#ifndef OCTOON_EGL3_TYPES_H_
#define OCTOON_EGL3_TYPES_H_

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
#include <GLES3\gl3.h>
#include <GLES3\gl31.h>

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

#if GL_DEBUG
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_CHECK(x) do { x; EGL3Check::checkError(); } while (false)
#else
#	define GL_CHECK(x) x
#endif

#if defined(__DEBUG__)
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_PLATFORM_LOG(format, ...) { EGL3Check::debugOutput(format); } 
#else
#	define GL_PLATFORM_LOG(format, ...)
#endif

#if defined(__DEBUG__)
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_PLATFORM_ASSERT(expr, format) if (!(expr)) { EGL3Check::debugOutput(format); assert(expr); }
#else
#	define GL_PLATFORM_ASSERT(expr, format)
#endif

namespace octoon 
{
	namespace graphics
	{
		enum EGL3Features
		{
			EGL3_KHR_blend_equation_advanced,
			EGL3_KHR_blend_equation_advanced_coherent,
			EGL3_KHR_context_flush_control,
			EGL3_KHR_debug,
			EGL3_KHR_robust_buffer_access_behavior,
			EGL3_KHR_robustness,
			EGL3_KHR_texture_compression_astc_hdr,
			EGL3_KHR_texture_compression_astc_ldr,
			EGL3_OES_EGL_image,
			EGL3_OES_EGL_image_external,
			EGL3_OES_compressed_ETC1_RGB8_sub_texture,
			EGL3_OES_compressed_ETC1_RGB8_texture,
			EGL3_OES_compressed_paletted_texture,
			EGL3_OES_copy_image,
			EGL3_OES_depth24,
			EGL3_OES_depth32,
			EGL3_OES_depth_texture,
			EGL3_OES_draw_buffers_indexed,
			EGL3_OES_draw_elements_base_vertex,
			EGL3_OES_element_index_uint,
			EGL3_OES_fbo_render_mipmap,
			EGL3_OES_fragment_precision_high,
			EGL3_OES_geometry_point_size,
			EGL3_OES_geometry_shader,
			EGL3_OES_get_program_binary,
			EGL3_OES_gpu_shader5,
			EGL3_OES_mapbuffer,
			EGL3_OES_packed_depth_stencil,
			EGL3_OES_primitive_bounding_box,
			EGL3_OES_required_internalformat,
			EGL3_OES_rgb8_rgba8,
			EGL3_OES_sample_shading,
			EGL3_OES_sample_variables,
			EGL3_OES_shader_image_atomic,
			EGL3_OES_shader_io_blocks,
			EGL3_OES_shader_multisample_interpolation,
			EGL3_OES_standard_derivatives,
			EGL3_OES_stencil1,
			EGL3_OES_stencil4,
			EGL3_OES_surfaceless_context,
			EGL3_OES_tessellation_point_size,
			EGL3_OES_tessellation_shader,
			EGL3_OES_texture_3D,
			EGL3_OES_texture_border_clamp,
			EGL3_OES_texture_buffer,
			EGL3_OES_texture_compression_astc,
			EGL3_OES_texture_cube_map_array,
			EGL3_OES_texture_float,
			EGL3_OES_texture_float_linear,
			EGL3_OES_texture_half_float,
			EGL3_OES_texture_half_float_linear,
			EGL3_OES_texture_npot,
			EGL3_OES_texture_stencil8,
			EGL3_OES_texture_storage_multisample_2d_array,
			EGL3_OES_texture_view,
			EGL3_OES_vertex_array_object,
			EGL3_OES_vertex_half_float,
			EGL3_OES_vertex_type_10_10_10_2,
			EGL3_AMD_compressed_3DC_texture,
			EGL3_AMD_compressed_ATC_texture,
			EGL3_AMD_performance_monitor,
			EGL3_AMD_program_binary_Z400,
			EGL3_ANDROID_extension_pack_es31a,
			EGL3_ANGLE_depth_texture,
			EGL3_ANGLE_framebuffer_blit,
			EGL3_ANGLE_framebuffer_multisample,
			EGL3_ANGLE_instanced_arrays,
			EGL3_ANGLE_pack_reverse_row_order,
			EGL3_ANGLE_program_binary,
			EGL3_ANGLE_texture_compression_dxt3,
			EGL3_ANGLE_texture_compression_dxt5,
			EGL3_ANGLE_texture_usage,
			EGL3_ANGLE_translated_shader_source,
			EGL3_APPLE_clip_distance,
			EGL3_APPLE_color_buffer_packed_float,
			EGL3_APPLE_copy_texture_levels,
			EGL3_APPLE_framebuffer_multisample,
			EGL3_APPLE_rgb_422,
			EGL3_APPLE_sync,
			EGL3_APPLE_texture_format_BGRA8888,
			EGL3_APPLE_texture_max_level,
			EGL3_APPLE_texture_packed_float,
			EGL3_ARM_mali_program_binary,
			EGL3_ARM_mali_shader_binary,
			EGL3_ARM_rgba8,
			EGL3_ARM_shader_framebuffer_fetch,
			EGL3_ARM_shader_framebuffer_fetch_depth_stencil,
			EGL3_DMP_program_binary,
			EGL3_DMP_shader_binary,
			EGL3_EXT_base_instance,
			EGL3_EXT_blend_minmax,
			EGL3_EXT_color_buffer_half_float,
			EGL3_EXT_color_buffer_float,
			EGL3_EXT_copy_image,
			EGL3_EXT_debug_label,
			EGL3_EXT_debug_marker,
			EGL3_EXT_discard_framebuffer,
			EGL3_EXT_disjoint_timer_query,
			EGL3_EXT_draw_buffers,
			EGL3_EXT_draw_buffers_indexed,
			EGL3_EXT_draw_elements_base_vertex,
			EGL3_EXT_draw_instanced,
			EGL3_EXT_geometry_point_size,
			EGL3_EXT_geometry_shader,
			EGL3_EXT_gpu_shader5,
			EGL3_EXT_instanced_arrays,
			EGL3_EXT_map_buffer_range,
			EGL3_EXT_multi_draw_arrays,
			EGL3_EXT_multi_draw_indirect,
			EGL3_EXT_multisampled_render_to_texture,
			EGL3_EXT_multiview_draw_buffers,
			EGL3_EXT_occlusion_query_boolean,
			EGL3_EXT_primitive_bounding_box,
			EGL3_EXT_pvrtc_sRGB,
			EGL3_EXT_read_format_bgra,
			EGL3_EXT_render_snorm,
			EGL3_EXT_robustness,
			EGL3_EXT_sRGB,
			EGL3_EXT_sRGB_write_control,
			EGL3_EXT_separate_shader_objects,
			EGL3_EXT_shader_framebuffer_fetch,
			EGL3_EXT_shader_implicit_conversions,
			EGL3_EXT_shader_integer_mix,
			EGL3_EXT_shader_io_blocks,
			EGL3_EXT_shader_pixel_local_storage,
			EGL3_EXT_shader_texture_lod,
			EGL3_EXT_shadow_samplers,
			EGL3_EXT_tessellation_point_size,
			EGL3_EXT_tessellation_shader,
			EGL3_EXT_texture_border_clamp,
			EGL3_EXT_texture_buffer,
			EGL3_EXT_texture_compression_dxt1,
			EGL3_EXT_texture_compression_s3tc,
			EGL3_EXT_texture_cube_map_array,
			EGL3_EXT_texture_filter_anisotropic,
			EGL3_EXT_texture_format_BGRA8888,
			EGL3_EXT_texture_norm16,
			EGL3_EXT_texture_rg,
			EGL3_EXT_texture_sRGB_decode,
			EGL3_EXT_texture_storage,
			EGL3_EXT_texture_type_2_10_10_10_REV,
			EGL3_EXT_texture_view,
			EGL3_EXT_unpack_subimage,
			EGL3_FJ_shader_binary_GCCSO,
			EGL3_IMG_multisampled_render_to_texture,
			EGL3_IMG_program_binary,
			EGL3_IMG_read_format,
			EGL3_IMG_shader_binary,
			EGL3_IMG_texture_compression_pvrtc,
			EGL3_IMG_texture_compression_pvrtc2,
			EGL3_INTEL_performance_query,
			EGL3_NV_bindless_texture,
			EGL3_NV_blend_equation_advanced,
			EGL3_NV_blend_equation_advanced_coherent,
			EGL3_NV_conditional_render,
			EGL3_NV_copy_buffer,
			EGL3_NV_coverage_sample,
			EGL3_NV_depth_nonlinear,
			EGL3_NV_draw_buffers,
			EGL3_NV_draw_instanced,
			EGL3_NV_explicit_attrib_location,
			EGL3_NV_fbo_color_attachments,
			EGL3_NV_fence,
			EGL3_NV_framebuffer_blit,
			EGL3_NV_framebuffer_multisample,
			EGL3_NV_generate_mipmap_sRGB,
			EGL3_NV_image_formats,
			EGL3_NV_instanced_arrays,
			EGL3_NV_internalformat_sample_query,
			EGL3_NV_non_square_matrices,
			EGL3_NV_path_rendering,
			EGL3_NV_read_buffer,
			EGL3_NV_read_buffer_front,
			EGL3_NV_read_depth,
			EGL3_NV_read_depth_stencil,
			EGL3_NV_read_stencil,
			EGL3_NV_sRGB_formats,
			EGL3_NV_shader_noperspective_interpolation,
			EGL3_NV_shadow_samplers_array,
			EGL3_NV_shadow_samplers_cube,
			EGL3_NV_texture_border_clamp,
			EGL3_NV_texture_compression_s3tc_update,
			EGL3_NV_texture_npot_2D_mipmap,
			EGL3_NV_viewport_array,
			EGL3_QCOM_alpha_test,
			EGL3_QCOM_binning_control,
			EGL3_QCOM_driver_control,
			EGL3_QCOM_extended_get,
			EGL3_QCOM_extended_get2,
			EGL3_QCOM_perfmon_global_mode,
			EGL3_QCOM_tiled_rendering,
			EGL3_QCOM_writeonly_rendering,
			EGL3_VIV_shader_binary,
			EGL3_BeginRange = EGL3_KHR_blend_equation_advanced,
			EGL3_EndRange = EGL3_VIV_shader_binary,
			EGL3_RangeSize = (EGL3_EndRange - EGL3_BeginRange + 1),
			EGL3_MaxEnum = 0x7FFFFFFF
		};

		typedef std::shared_ptr<class EGL3Device> EGL3DevicePtr;
		typedef std::shared_ptr<class EGL3Surface> EGL3SurfacePtr;
		typedef std::shared_ptr<class EGL3Swapchain> EGL3SwapchainPtr;
		typedef std::shared_ptr<class EGL3DeviceContext> EGL3DeviceContextPtr;
		typedef std::shared_ptr<class EGL3FramebufferLayout> EGL3FramebufferLayoutPtr;
		typedef std::shared_ptr<class EGL3Framebuffer> EGL3FramebufferPtr;
		typedef std::shared_ptr<class EGL3Shader> EGL3ShaderPtr;
		typedef std::shared_ptr<class EGL3Program> EGL3ProgramPtr;
		typedef std::shared_ptr<class EGL3GraphicsData> EGL3GraphicsDataPtr;
		typedef std::shared_ptr<class EGL3InputLayout> EGL3InputLayoutPtr;
		typedef std::shared_ptr<class EGL3GraphicsState> EGL3GraphicsStatePtr;
		typedef std::shared_ptr<class EGL3Texture> EGL3TexturePtr;
		typedef std::shared_ptr<class EGL3Sampler> EGL3SamplerPtr;
		typedef std::shared_ptr<class EGL3Pipeline> EGL3PipelinePtr;
		typedef std::shared_ptr<class EGL3DescriptorSetPool> EGL3DescriptorSetPoolPtr;
		typedef std::shared_ptr<class EGL3DescriptorSet> EGL3DescriptorSetPtr;
		typedef std::shared_ptr<class EGL3DescriptorSetLayout> EGL3DescriptorSetLayoutPtr;
		typedef std::shared_ptr<class EGL3GraphicsAttribute> EGL3GraphicsAttributePtr;
		typedef std::shared_ptr<class EGL3GraphicsUniform> EGL3GraphicsUniformPtr;
		typedef std::shared_ptr<class EGL3GraphicsUniformBlock> EGL3GraphicsUniformBlockPtr;

		typedef std::weak_ptr<class EGL3Device> EGL3DeviceWeakPtr;
		typedef std::weak_ptr<class EGL3Surface> EGL3SurfaceWeakPtr;
		typedef std::weak_ptr<class EGL3Swapchain> EGL3SwapchainWeakPtr;
		typedef std::weak_ptr<class EGL3DeviceContext> EGL3DeviceContextWeakPtr;
		typedef std::weak_ptr<class EGL3FramebufferLayout> EGL3FramebufferLayoutWeakPtr;
		typedef std::weak_ptr<class EGL3Framebuffer> EGL3FramebufferWeakPtr;
		typedef std::weak_ptr<class EGL3Shader> EGL3ShaderWeakPtr;
		typedef std::weak_ptr<class EGL3Program> EGL3ProgramWeakPtr;
		typedef std::weak_ptr<class EGL3GraphicsData> EGL3GraphicsDataWeakPtr;
		typedef std::weak_ptr<class EGL3InputLayout> EGL3InputLayoutWeakPtr;
		typedef std::weak_ptr<class EGL3GraphicsState> EGL3GraphicsStateWeakPtr;
		typedef std::weak_ptr<class EGL3Texture> EGL3TextureWeakPtr;
		typedef std::weak_ptr<class EGL3Sampler> EGL3SamplerWeakPtr;
		typedef std::weak_ptr<class EGL3Pipeline> EGL3PipelineWeakPtr;
		typedef std::weak_ptr<class EGL3DescriptorSetPool> EGL3DescriptorSetPoolWeakPtr;
		typedef std::weak_ptr<class EGL3DescriptorSet> EGL3DescriptorSetWeakPtr;
		typedef std::weak_ptr<class EGL3DescriptorSetLayout> EGL3DescriptorSetLayoutWeakPtr;
		typedef std::weak_ptr<class EGL3GraphicsAttribute> EGL3GraphicsAttributeWeakPtr;
		typedef std::weak_ptr<class EGL3GraphicsUniform> EGL3GraphicsUniformWeakPtr;
		typedef std::weak_ptr<class EGL3GraphicsUniformBlock> EGL3GraphicsUniformBlockWeakPtr;

		typedef std::vector<EGL3ShaderPtr> EGL3Shaders;

		struct EGL3VertexBuffer
		{
			bool needUpdate;
			std::intptr_t offset;
			EGL3GraphicsDataPtr vbo;
		};

		typedef std::vector<EGL3VertexBuffer> EGL3VertexBuffers;

		class EGL3Types
		{
		public:
			static bool setup() noexcept;

			static GLenum asVertexType(GraphicsVertexType type) noexcept;
			static GLenum asVertexFormat(GraphicsFormat format) noexcept;
			static GLenum asIndexType(GraphicsIndexType type) noexcept;
			static GLenum asShaderStage(GraphicsShaderStageFlags flags) noexcept;
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
			static GLboolean isSupportFeature(EGL3Features features) noexcept;
			static GLboolean isStencilFormat(GraphicsFormat format) noexcept;
			static GLboolean isDepthFormat(GraphicsFormat format) noexcept;
			static GLboolean isDepthStencilFormat(GraphicsFormat format) noexcept;
			static GLboolean isCompressedTexture(GraphicsFormat format) noexcept;

		private:
			static GLboolean _egl3init;
			static GLboolean _egl3Features[EGL3Features::EGL3_RangeSize];
		};

		class EGL3Check
		{
		public:
			static bool checkError() noexcept;

			static void debugOutput(const char* message, ...) noexcept;
		};
	}
}

#endif