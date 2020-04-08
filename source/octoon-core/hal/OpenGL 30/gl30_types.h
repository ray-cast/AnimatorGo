#ifndef OCTOON_GL30_TYPES_H_
#define OCTOON_GL30_TYPES_H_

#include "ogl_swapchain.h"

#if GL_DEBUG
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_CHECK(x) do { x; GL30Check::checkError(); } while (false)
#else
#	define GL_CHECK(x) x
#endif

#if defined(__DEBUG__)
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_PLATFORM_LOG(format, ...) { GL30Check::debugOutput(format); }
#else
#	define GL_PLATFORM_LOG(format, ...)
#endif

#if defined(__DEBUG__)
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_PLATFORM_ASSERT(expr, format) if (!(expr)) { GL30Check::debugOutput(format); assert(expr); }
#else
#	define GL_PLATFORM_ASSERT(expr, format)
#endif

namespace octoon
{
	namespace hal
	{
		enum GL30Features
		{
			GL30_KHR_blend_equation_advanced,
			GL30_KHR_blend_equation_advanced_coherent,
			GL30_KHR_context_flush_control,
			GL30_KHR_debug,
			GL30_KHR_robust_buffer_access_behavior,
			GL30_KHR_robustness,
			GL30_KHR_texture_compression_astc_hdr,
			GL30_KHR_texture_compression_astc_ldr,
			GL30_OES_EGL_image,
			GL30_OES_EGL_image_external,
			GL30_OES_compressed_ETC1_RGB8_sub_texture,
			GL30_OES_compressed_ETC1_RGB8_texture,
			GL30_OES_compressed_paletted_texture,
			GL30_OES_copy_image,
			GL30_OES_depth24,
			GL30_OES_depth32,
			GL30_OES_depth_texture,
			GL30_OES_draw_buffers_indexed,
			GL30_OES_draw_elements_base_vertex,
			GL30_OES_element_index_uint,
			GL30_OES_fbo_render_mipmap,
			GL30_OES_fragment_precision_high,
			GL30_OES_geometry_point_size,
			GL30_OES_geometry_shader,
			GL30_OES_get_program_binary,
			GL30_OES_gpu_shader5,
			GL30_OES_mapbuffer,
			GL30_OES_packed_depth_stencil,
			GL30_OES_primitive_bounding_box,
			GL30_OES_required_internalformat,
			GL30_OES_rgb8_rgba8,
			GL30_OES_sample_shading,
			GL30_OES_sample_variables,
			GL30_OES_shader_image_atomic,
			GL30_OES_shader_io_blocks,
			GL30_OES_shader_multisample_interpolation,
			GL30_OES_standard_derivatives,
			GL30_OES_stencil1,
			GL30_OES_stencil4,
			GL30_OES_surfaceless_context,
			GL30_OES_tessellation_point_size,
			GL30_OES_tessellation_shader,
			GL30_OES_texture_3D,
			GL30_OES_texture_border_clamp,
			GL30_OES_texture_buffer,
			GL30_OES_texture_compression_astc,
			GL30_OES_texture_cube_map_array,
			GL30_OES_texture_float,
			GL30_OES_texture_float_linear,
			GL30_OES_texture_half_float,
			GL30_OES_texture_half_float_linear,
			GL30_OES_texture_npot,
			GL30_OES_texture_stencil8,
			GL30_OES_texture_storage_multisample_2d_array,
			GL30_OES_texture_view,
			GL30_OES_vertex_array_object,
			GL30_OES_vertex_half_float,
			GL30_OES_vertex_type_10_10_10_2,
			GL30_AMD_compressed_3DC_texture,
			GL30_AMD_compressed_ATC_texture,
			GL30_AMD_performance_monitor,
			GL30_AMD_program_binary_Z400,
			GL30_ANDROID_extension_pack_es31a,
			GL30_ANGLE_depth_texture,
			GL30_ANGLE_framebuffer_blit,
			GL30_ANGLE_framebuffer_multisample,
			GL30_ANGLE_instanced_arrays,
			GL30_ANGLE_pack_reverse_row_order,
			GL30_ANGLE_program_binary,
			GL30_ANGLE_texture_compression_dxt3,
			GL30_ANGLE_texture_compression_dxt5,
			GL30_ANGLE_texture_usage,
			GL30_ANGLE_translated_shader_source,
			GL30_APPLE_clip_distance,
			GL30_APPLE_color_buffer_packed_float,
			GL30_APPLE_copy_texture_levels,
			GL30_APPLE_framebuffer_multisample,
			GL30_APPLE_rgb_422,
			GL30_APPLE_sync,
			GL30_APPLE_texture_format_BGRA8888,
			GL30_APPLE_texture_max_level,
			GL30_APPLE_texture_packed_float,
			GL30_ARM_mali_program_binary,
			GL30_ARM_mali_shader_binary,
			GL30_ARM_rgba8,
			GL30_ARM_shader_framebuffer_fetch,
			GL30_ARM_shader_framebuffer_fetch_depth_stencil,
			GL30_DMP_program_binary,
			GL30_DMP_shader_binary,
			GL30_EXT_base_instance,
			GL30_EXT_blend_minmax,
			GL30_EXT_color_buffer_half_float,
			GL30_EXT_color_buffer_float,
			GL30_EXT_copy_image,
			GL30_EXT_debug_label,
			GL30_EXT_debug_marker,
			GL30_EXT_discard_framebuffer,
			GL30_EXT_disjoint_timer_query,
			GL30_EXT_draw_buffers,
			GL30_EXT_draw_buffers_indexed,
			GL30_EXT_draw_elements_base_vertex,
			GL30_EXT_draw_instanced,
			GL30_EXT_geometry_point_size,
			GL30_EXT_geometry_shader,
			GL30_EXT_gpu_shader5,
			GL30_EXT_instanced_arrays,
			GL30_EXT_map_buffer_range,
			GL30_EXT_multi_draw_arrays,
			GL30_EXT_multi_draw_indirect,
			GL30_EXT_multisampled_render_to_texture,
			GL30_EXT_multiview_draw_buffers,
			GL30_EXT_occlusion_query_boolean,
			GL30_EXT_primitive_bounding_box,
			GL30_EXT_pvrtc_sRGB,
			GL30_EXT_read_format_bgra,
			GL30_EXT_render_snorm,
			GL30_EXT_robustness,
			GL30_EXT_sRGB,
			GL30_EXT_sRGB_write_control,
			GL30_EXT_separate_shader_objects,
			GL30_EXT_shader_framebuffer_fetch,
			GL30_EXT_shader_implicit_conversions,
			GL30_EXT_shader_integer_mix,
			GL30_EXT_shader_io_blocks,
			GL30_EXT_shader_pixel_local_storage,
			GL30_EXT_shader_texture_lod,
			GL30_EXT_shadow_samplers,
			GL30_EXT_tessellation_point_size,
			GL30_EXT_tessellation_shader,
			GL30_EXT_texture_border_clamp,
			GL30_EXT_texture_buffer,
			GL30_EXT_texture_compression_dxt1,
			GL30_EXT_texture_compression_s3tc,
			GL30_EXT_texture_cube_map_array,
			GL30_EXT_texture_filter_anisotropic,
			GL30_EXT_texture_format_BGRA8888,
			GL30_EXT_texture_norm16,
			GL30_EXT_texture_rg,
			GL30_EXT_texture_sRGB_decode,
			GL30_EXT_texture_storage,
			GL30_EXT_texture_type_2_10_10_10_REV,
			GL30_EXT_texture_view,
			GL30_EXT_unpack_subimage,
			GL30_FJ_shader_binary_GCCSO,
			GL30_IMG_multisampled_render_to_texture,
			GL30_IMG_program_binary,
			GL30_IMG_read_format,
			GL30_IMG_shader_binary,
			GL30_IMG_texture_compression_pvrtc,
			GL30_IMG_texture_compression_pvrtc2,
			GL30_INTEL_performance_query,
			GL30_NV_bindless_texture,
			GL30_NV_blend_equation_advanced,
			GL30_NV_blend_equation_advanced_coherent,
			GL30_NV_conditional_render,
			GL30_NV_copy_buffer,
			GL30_NV_coverage_sample,
			GL30_NV_depth_nonlinear,
			GL30_NV_draw_buffers,
			GL30_NV_draw_instanced,
			GL30_NV_explicit_attrib_location,
			GL30_NV_fbo_color_attachments,
			GL30_NV_fence,
			GL30_NV_framebuffer_blit,
			GL30_NV_framebuffer_multisample,
			GL30_NV_generate_mipmap_sRGB,
			GL30_NV_image_formats,
			GL30_NV_instanced_arrays,
			GL30_NV_internalformat_sample_query,
			GL30_NV_non_square_matrices,
			GL30_NV_path_rendering,
			GL30_NV_read_buffer,
			GL30_NV_read_buffer_front,
			GL30_NV_read_depth,
			GL30_NV_read_depth_stencil,
			GL30_NV_read_stencil,
			GL30_NV_sRGB_formats,
			GL30_NV_shader_noperspective_interpolation,
			GL30_NV_shadow_samplers_array,
			GL30_NV_shadow_samplers_cube,
			GL30_NV_texture_border_clamp,
			GL30_NV_texture_compression_s3tc_update,
			GL30_NV_texture_npot_2D_mipmap,
			GL30_NV_viewport_array,
			GL30_QCOM_alpha_test,
			GL30_QCOM_binning_control,
			GL30_QCOM_driver_control,
			GL30_QCOM_extended_get,
			GL30_QCOM_extended_get2,
			GL30_QCOM_perfmon_global_mode,
			GL30_QCOM_tiled_rendering,
			GL30_QCOM_writeonly_rendering,
			GL30_VIV_shader_binary,
			GL30_BeginRange = GL30_KHR_blend_equation_advanced,
			GL30_EndRange = GL30_VIV_shader_binary,
			GL30_RangeSize = (GL30_EndRange - GL30_BeginRange + 1),
			GL30_MaxEnum = 0x7FFFFFFF
		};

		typedef std::shared_ptr<class GL30Device> GL30DevicePtr;
		typedef std::shared_ptr<class GL30Surface> GL30SurfacePtr;
		typedef std::shared_ptr<class OGLSwapchain> GL30SwapchainPtr;
		typedef std::shared_ptr<class GL30DeviceContext> GL30DeviceContextPtr;
		typedef std::shared_ptr<class GL30FramebufferLayout> GL30FramebufferLayoutPtr;
		typedef std::shared_ptr<class GL30Framebuffer> GL30FramebufferPtr;
		typedef std::shared_ptr<class GL30Shader> GL30ShaderPtr;
		typedef std::shared_ptr<class GL30Program> GL30ProgramPtr;
		typedef std::shared_ptr<class GL30GraphicsData> GL30GraphicsDataPtr;
		typedef std::shared_ptr<class GL30InputLayout> GL30InputLayoutPtr;
		typedef std::shared_ptr<class GL30GraphicsState> GL30GraphicsStatePtr;
		typedef std::shared_ptr<class GL30Texture> GL30TexturePtr;
		typedef std::shared_ptr<class GL30Sampler> GL30SamplerPtr;
		typedef std::shared_ptr<class GL30Pipeline> GL30PipelinePtr;
		typedef std::shared_ptr<class GL30DescriptorSetPool> GL30DescriptorSetPoolPtr;
		typedef std::shared_ptr<class GL30DescriptorSet> GL30DescriptorSetPtr;
		typedef std::shared_ptr<class GL30DescriptorSetLayout> GL30DescriptorSetLayoutPtr;
		typedef std::shared_ptr<class GL30GraphicsAttribute> GL30GraphicsAttributePtr;
		typedef std::shared_ptr<class GL30GraphicsUniform> GL30GraphicsUniformPtr;
		typedef std::shared_ptr<class GL30GraphicsUniformBlock> GL30GraphicsUniformBlockPtr;

		typedef std::weak_ptr<class GL30Device> GL30DeviceWeakPtr;
		typedef std::weak_ptr<class GL30Surface> GL30SurfaceWeakPtr;
		typedef std::weak_ptr<class OGLSwapchain> GL30SwapchainWeakPtr;
		typedef std::weak_ptr<class GL30DeviceContext> GL30DeviceContextWeakPtr;
		typedef std::weak_ptr<class GL30FramebufferLayout> GL30FramebufferLayoutWeakPtr;
		typedef std::weak_ptr<class GL30Framebuffer> GL30FramebufferWeakPtr;
		typedef std::weak_ptr<class GL30Shader> GL30ShaderWeakPtr;
		typedef std::weak_ptr<class GL30Program> GL30ProgramWeakPtr;
		typedef std::weak_ptr<class GL30GraphicsData> GL30GraphicsDataWeakPtr;
		typedef std::weak_ptr<class GL30InputLayout> GL30InputLayoutWeakPtr;
		typedef std::weak_ptr<class GL30GraphicsState> GL30GraphicsStateWeakPtr;
		typedef std::weak_ptr<class GL30Texture> GL30TextureWeakPtr;
		typedef std::weak_ptr<class GL30Sampler> GL30SamplerWeakPtr;
		typedef std::weak_ptr<class GL30Pipeline> GL30PipelineWeakPtr;
		typedef std::weak_ptr<class GL30DescriptorSetPool> GL30DescriptorSetPoolWeakPtr;
		typedef std::weak_ptr<class GL30DescriptorSet> GL30DescriptorSetWeakPtr;
		typedef std::weak_ptr<class GL30DescriptorSetLayout> GL30DescriptorSetLayoutWeakPtr;
		typedef std::weak_ptr<class GL30GraphicsAttribute> GL30GraphicsAttributeWeakPtr;
		typedef std::weak_ptr<class GL30GraphicsUniform> GL30GraphicsUniformWeakPtr;
		typedef std::weak_ptr<class GL30GraphicsUniformBlock> GL30GraphicsUniformBlockWeakPtr;

		typedef std::vector<GL30ShaderPtr> GL30Shaders;

		struct GL30VertexBuffer
		{
			bool setDirty;
			std::intptr_t offset;
			GL30GraphicsDataPtr vbo;
		};

		typedef std::vector<GL30VertexBuffer> GL30VertexBuffers;

		class GL30Types
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
			static GLboolean isSupportFeature(GL30Features features) noexcept;
			static GLboolean isStencilFormat(GraphicsFormat format) noexcept;
			static GLboolean isDepthFormat(GraphicsFormat format) noexcept;
			static GLboolean isDepthStencilFormat(GraphicsFormat format) noexcept;
			static GLboolean isCompressedTexture(GraphicsFormat format) noexcept;

		private:
			static GLboolean _egl3init;
			static GLboolean _egl3Features[GL30Features::GL30_RangeSize];
		};

		class GL30Check
		{
		public:
			static bool checkError() noexcept;

			static void debugOutput(const char* message, ...) noexcept;
		};
	}
}

#endif