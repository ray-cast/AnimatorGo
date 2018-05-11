#ifndef OCTOON_GL32_TYPES_H_
#define OCTOON_GL32_TYPES_H_

#include "ogl_swapchain.h"

#if GL_DEBUG
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_CHECK(x) do { x; GL32Check::checkError(); } while (false)
#else
#	define GL_CHECK(x) x
#endif

#if defined(__DEBUG__)
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_PLATFORM_LOG(format, ...) { GL32Check::debugOutput(format); }
#else
#	define GL_PLATFORM_LOG(format, ...)
#endif

#if defined(__DEBUG__)
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_PLATFORM_ASSERT(expr, format) if (!(expr)) { GL32Check::debugOutput(format); assert(expr); }
#else
#	define GL_PLATFORM_ASSERT(expr, format)
#endif

namespace octoon
{
	namespace graphics
	{
		enum GL32Features
		{
			GL32_KHR_blend_equation_advanced,
			GL32_KHR_blend_equation_advanced_coherent,
			GL32_KHR_context_flush_control,
			GL32_KHR_debug,
			GL32_KHR_robust_buffer_access_behavior,
			GL32_KHR_robustness,
			GL32_KHR_texture_compression_astc_hdr,
			GL32_KHR_texture_compression_astc_ldr,
			GL32_OES_EGL_image,
			GL32_OES_EGL_image_external,
			GL32_OES_compressed_ETC1_RGB8_sub_texture,
			GL32_OES_compressed_ETC1_RGB8_texture,
			GL32_OES_compressed_paletted_texture,
			GL32_OES_copy_image,
			GL32_OES_depth24,
			GL32_OES_depth32,
			GL32_OES_depth_texture,
			GL32_OES_draw_buffers_indexed,
			GL32_OES_draw_elements_base_vertex,
			GL32_OES_element_index_uint,
			GL32_OES_fbo_render_mipmap,
			GL32_OES_fragment_precision_high,
			GL32_OES_geometry_point_size,
			GL32_OES_geometry_shader,
			GL32_OES_get_program_binary,
			GL32_OES_gpu_shader5,
			GL32_OES_mapbuffer,
			GL32_OES_packed_depth_stencil,
			GL32_OES_primitive_bounding_box,
			GL32_OES_required_internalformat,
			GL32_OES_rgb8_rgba8,
			GL32_OES_sample_shading,
			GL32_OES_sample_variables,
			GL32_OES_shader_image_atomic,
			GL32_OES_shader_io_blocks,
			GL32_OES_shader_multisample_interpolation,
			GL32_OES_standard_derivatives,
			GL32_OES_stencil1,
			GL32_OES_stencil4,
			GL32_OES_surfaceless_context,
			GL32_OES_tessellation_point_size,
			GL32_OES_tessellation_shader,
			GL32_OES_texture_3D,
			GL32_OES_texture_border_clamp,
			GL32_OES_texture_buffer,
			GL32_OES_texture_compression_astc,
			GL32_OES_texture_cube_map_array,
			GL32_OES_texture_float,
			GL32_OES_texture_float_linear,
			GL32_OES_texture_half_float,
			GL32_OES_texture_half_float_linear,
			GL32_OES_texture_npot,
			GL32_OES_texture_stencil8,
			GL32_OES_texture_storage_multisample_2d_array,
			GL32_OES_texture_view,
			GL32_OES_vertex_array_object,
			GL32_OES_vertex_half_float,
			GL32_OES_vertex_type_10_10_10_2,
			GL32_AMD_compressed_3DC_texture,
			GL32_AMD_compressed_ATC_texture,
			GL32_AMD_performance_monitor,
			GL32_AMD_program_binary_Z400,
			GL32_ANDROID_extension_pack_es31a,
			GL32_ANGLE_depth_texture,
			GL32_ANGLE_framebuffer_blit,
			GL32_ANGLE_framebuffer_multisample,
			GL32_ANGLE_instanced_arrays,
			GL32_ANGLE_pack_reverse_row_order,
			GL32_ANGLE_program_binary,
			GL32_ANGLE_texture_compression_dxt3,
			GL32_ANGLE_texture_compression_dxt5,
			GL32_ANGLE_texture_usage,
			GL32_ANGLE_translated_shader_source,
			GL32_APPLE_clip_distance,
			GL32_APPLE_color_buffer_packed_float,
			GL32_APPLE_copy_texture_levels,
			GL32_APPLE_framebuffer_multisample,
			GL32_APPLE_rgb_422,
			GL32_APPLE_sync,
			GL32_APPLE_texture_format_BGRA8888,
			GL32_APPLE_texture_max_level,
			GL32_APPLE_texture_packed_float,
			GL32_ARM_mali_program_binary,
			GL32_ARM_mali_shader_binary,
			GL32_ARM_rgba8,
			GL32_ARM_shader_framebuffer_fetch,
			GL32_ARM_shader_framebuffer_fetch_depth_stencil,
			GL32_DMP_program_binary,
			GL32_DMP_shader_binary,
			GL32_EXT_base_instance,
			GL32_EXT_blend_minmax,
			GL32_EXT_color_buffer_half_float,
			GL32_EXT_color_buffer_float,
			GL32_EXT_copy_image,
			GL32_EXT_debug_label,
			GL32_EXT_debug_marker,
			GL32_EXT_discard_framebuffer,
			GL32_EXT_disjoint_timer_query,
			GL32_EXT_draw_buffers,
			GL32_EXT_draw_buffers_indexed,
			GL32_EXT_draw_elements_base_vertex,
			GL32_EXT_draw_instanced,
			GL32_EXT_geometry_point_size,
			GL32_EXT_geometry_shader,
			GL32_EXT_gpu_shader5,
			GL32_EXT_instanced_arrays,
			GL32_EXT_map_buffer_range,
			GL32_EXT_multi_draw_arrays,
			GL32_EXT_multi_draw_indirect,
			GL32_EXT_multisampled_render_to_texture,
			GL32_EXT_multiview_draw_buffers,
			GL32_EXT_occlusion_query_boolean,
			GL32_EXT_primitive_bounding_box,
			GL32_EXT_pvrtc_sRGB,
			GL32_EXT_read_format_bgra,
			GL32_EXT_render_snorm,
			GL32_EXT_robustness,
			GL32_EXT_sRGB,
			GL32_EXT_sRGB_write_control,
			GL32_EXT_separate_shader_objects,
			GL32_EXT_shader_framebuffer_fetch,
			GL32_EXT_shader_implicit_conversions,
			GL32_EXT_shader_integer_mix,
			GL32_EXT_shader_io_blocks,
			GL32_EXT_shader_pixel_local_storage,
			GL32_EXT_shader_texture_lod,
			GL32_EXT_shadow_samplers,
			GL32_EXT_tessellation_point_size,
			GL32_EXT_tessellation_shader,
			GL32_EXT_texture_border_clamp,
			GL32_EXT_texture_buffer,
			GL32_EXT_texture_compression_dxt1,
			GL32_EXT_texture_compression_s3tc,
			GL32_EXT_texture_cube_map_array,
			GL32_EXT_texture_filter_anisotropic,
			GL32_EXT_texture_format_BGRA8888,
			GL32_EXT_texture_norm16,
			GL32_EXT_texture_rg,
			GL32_EXT_texture_sRGB_decode,
			GL32_EXT_texture_storage,
			GL32_EXT_texture_type_2_10_10_10_REV,
			GL32_EXT_texture_view,
			GL32_EXT_unpack_subimage,
			GL32_FJ_shader_binary_GCCSO,
			GL32_IMG_multisampled_render_to_texture,
			GL32_IMG_program_binary,
			GL32_IMG_read_format,
			GL32_IMG_shader_binary,
			GL32_IMG_texture_compression_pvrtc,
			GL32_IMG_texture_compression_pvrtc2,
			GL32_INTEL_performance_query,
			GL32_NV_bindless_texture,
			GL32_NV_blend_equation_advanced,
			GL32_NV_blend_equation_advanced_coherent,
			GL32_NV_conditional_render,
			GL32_NV_copy_buffer,
			GL32_NV_coverage_sample,
			GL32_NV_depth_nonlinear,
			GL32_NV_draw_buffers,
			GL32_NV_draw_instanced,
			GL32_NV_explicit_attrib_location,
			GL32_NV_fbo_color_attachments,
			GL32_NV_fence,
			GL32_NV_framebuffer_blit,
			GL32_NV_framebuffer_multisample,
			GL32_NV_generate_mipmap_sRGB,
			GL32_NV_image_formats,
			GL32_NV_instanced_arrays,
			GL32_NV_internalformat_sample_query,
			GL32_NV_non_square_matrices,
			GL32_NV_path_rendering,
			GL32_NV_read_buffer,
			GL32_NV_read_buffer_front,
			GL32_NV_read_depth,
			GL32_NV_read_depth_stencil,
			GL32_NV_read_stencil,
			GL32_NV_sRGB_formats,
			GL32_NV_shader_noperspective_interpolation,
			GL32_NV_shadow_samplers_array,
			GL32_NV_shadow_samplers_cube,
			GL32_NV_texture_border_clamp,
			GL32_NV_texture_compression_s3tc_update,
			GL32_NV_texture_npot_2D_mipmap,
			GL32_NV_viewport_array,
			GL32_QCOM_alpha_test,
			GL32_QCOM_binning_control,
			GL32_QCOM_driver_control,
			GL32_QCOM_extended_get,
			GL32_QCOM_extended_get2,
			GL32_QCOM_perfmon_global_mode,
			GL32_QCOM_tiled_rendering,
			GL32_QCOM_writeonly_rendering,
			GL32_VIV_shader_binary,
			GL32_BeginRange = GL32_KHR_blend_equation_advanced,
			GL32_EndRange = GL32_VIV_shader_binary,
			GL32_RangeSize = (GL32_EndRange - GL32_BeginRange + 1),
			GL32_MaxEnum = 0x7FFFFFFF
		};

		typedef std::shared_ptr<class GL32Device> GL32DevicePtr;
		typedef std::shared_ptr<class GL32Surface> GL32SurfacePtr;
		typedef std::shared_ptr<class OGLSwapchain> GL32SwapchainPtr;
		typedef std::shared_ptr<class GL32DeviceContext> GL32DeviceContextPtr;
		typedef std::shared_ptr<class GL32FramebufferLayout> GL32FramebufferLayoutPtr;
		typedef std::shared_ptr<class GL32Framebuffer> GL32FramebufferPtr;
		typedef std::shared_ptr<class GL32Shader> GL32ShaderPtr;
		typedef std::shared_ptr<class GL32Program> GL32ProgramPtr;
		typedef std::shared_ptr<class GL32GraphicsData> GL32GraphicsDataPtr;
		typedef std::shared_ptr<class GL32InputLayout> GL32InputLayoutPtr;
		typedef std::shared_ptr<class GL32GraphicsState> GL32GraphicsStatePtr;
		typedef std::shared_ptr<class GL32Texture> GL32TexturePtr;
		typedef std::shared_ptr<class GL32Sampler> GL32SamplerPtr;
		typedef std::shared_ptr<class GL32Pipeline> GL32PipelinePtr;
		typedef std::shared_ptr<class GL32DescriptorSetPool> GL32DescriptorSetPoolPtr;
		typedef std::shared_ptr<class GL32DescriptorSet> GL32DescriptorSetPtr;
		typedef std::shared_ptr<class GL32DescriptorSetLayout> GL32DescriptorSetLayoutPtr;
		typedef std::shared_ptr<class GL32GraphicsAttribute> GL32GraphicsAttributePtr;
		typedef std::shared_ptr<class GL32GraphicsUniform> GL32GraphicsUniformPtr;
		typedef std::shared_ptr<class GL32GraphicsUniformBlock> GL32GraphicsUniformBlockPtr;

		typedef std::weak_ptr<class GL32Device> GL32DeviceWeakPtr;
		typedef std::weak_ptr<class GL32Surface> GL32SurfaceWeakPtr;
		typedef std::weak_ptr<class OGLSwapchain> GL32SwapchainWeakPtr;
		typedef std::weak_ptr<class GL32DeviceContext> GL32DeviceContextWeakPtr;
		typedef std::weak_ptr<class GL32FramebufferLayout> GL32FramebufferLayoutWeakPtr;
		typedef std::weak_ptr<class GL32Framebuffer> GL32FramebufferWeakPtr;
		typedef std::weak_ptr<class GL32Shader> GL32ShaderWeakPtr;
		typedef std::weak_ptr<class GL32Program> GL32ProgramWeakPtr;
		typedef std::weak_ptr<class GL32GraphicsData> GL32GraphicsDataWeakPtr;
		typedef std::weak_ptr<class GL32InputLayout> GL32InputLayoutWeakPtr;
		typedef std::weak_ptr<class GL32GraphicsState> GL32GraphicsStateWeakPtr;
		typedef std::weak_ptr<class GL32Texture> GL32TextureWeakPtr;
		typedef std::weak_ptr<class GL32Sampler> GL32SamplerWeakPtr;
		typedef std::weak_ptr<class GL32Pipeline> GL32PipelineWeakPtr;
		typedef std::weak_ptr<class GL32DescriptorSetPool> GL32DescriptorSetPoolWeakPtr;
		typedef std::weak_ptr<class GL32DescriptorSet> GL32DescriptorSetWeakPtr;
		typedef std::weak_ptr<class GL32DescriptorSetLayout> GL32DescriptorSetLayoutWeakPtr;
		typedef std::weak_ptr<class GL32GraphicsAttribute> GL32GraphicsAttributeWeakPtr;
		typedef std::weak_ptr<class GL32GraphicsUniform> GL32GraphicsUniformWeakPtr;
		typedef std::weak_ptr<class GL32GraphicsUniformBlock> GL32GraphicsUniformBlockWeakPtr;

		typedef std::vector<GL32ShaderPtr> GL32Shaders;

		struct GL32VertexBuffer
		{
			bool needUpdate;
			std::intptr_t offset;
			GL32GraphicsDataPtr vbo;
		};

		typedef std::vector<GL32VertexBuffer> GL32VertexBuffers;

		class GL32Types
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
			static GLboolean isSupportFeature(GL32Features features) noexcept;
			static GLboolean isStencilFormat(GraphicsFormat format) noexcept;
			static GLboolean isDepthFormat(GraphicsFormat format) noexcept;
			static GLboolean isDepthStencilFormat(GraphicsFormat format) noexcept;
			static GLboolean isCompressedTexture(GraphicsFormat format) noexcept;

		private:
			static GLboolean _egl3init;
			static GLboolean _egl3Features[GL32Features::GL32_RangeSize];
		};

		class GL32Check
		{
		public:
			static bool checkError() noexcept;

			static void debugOutput(const char* message, ...) noexcept;
		};
	}
}

#endif