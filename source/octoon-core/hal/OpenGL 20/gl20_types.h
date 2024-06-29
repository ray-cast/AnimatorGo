#ifndef OCTOON_GL20_TYPES_H_
#define OCTOON_GL20_TYPES_H_

#include "ogl_swapchain.h"

#if defined(__DEBUG__)
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_CHECK(x) do { x; GL20Check::checkError(); } while (false)
#else
#	define GL_CHECK(x) x
#endif

#if defined(__DEBUG__)
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_PLATFORM_LOG(format) GL20Check::debugOutput(format)
#else
#	define GL_PLATFORM_LOG(format)
#endif

#if defined(__DEBUG__)
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_PLATFORM_ASSERT(expr, format) if (!(expr)) { GL20Check::debugOutput(format); assert(expr); }
#else
#	define GL_PLATFORM_ASSERT(expr, format)
#endif

namespace octoon
{
	namespace hal
	{
		enum GL20Features
		{
			GL20_KHR_blend_equation_advanced,
			GL20_KHR_blend_equation_advanced_coherent,
			GL20_KHR_context_flush_control,
			GL20_KHR_debug,
			GL20_KHR_robust_buffer_access_behavior,
			GL20_KHR_robustness,
			GL20_KHR_texture_compression_astc_hdr,
			GL20_KHR_texture_compression_astc_ldr,
			GL20_OES_EGL_image,
			GL20_OES_EGL_image_external,
			GL20_OES_compressed_ETC1_RGB8_sub_texture,
			GL20_OES_compressed_ETC1_RGB8_texture,
			GL20_OES_compressed_paletted_texture,
			GL20_OES_copy_image,
			GL20_OES_depth24,
			GL20_OES_depth32,
			GL20_OES_depth_texture,
			GL20_OES_draw_buffers_indexed,
			GL20_OES_draw_elements_base_vertex,
			GL20_OES_element_index_uint,
			GL20_OES_fbo_render_mipmap,
			GL20_OES_fragment_precision_high,
			GL20_OES_geometry_point_size,
			GL20_OES_geometry_shader,
			GL20_OES_get_program_binary,
			GL20_OES_gpu_shader5,
			GL20_OES_mapbuffer,
			GL20_OES_packed_depth_stencil,
			GL20_OES_primitive_bounding_box,
			GL20_OES_required_internalformat,
			GL20_OES_rgb8_rgba8,
			GL20_OES_sample_shading,
			GL20_OES_sample_variables,
			GL20_OES_shader_image_atomic,
			GL20_OES_shader_io_blocks,
			GL20_OES_shader_multisample_interpolation,
			GL20_OES_standard_derivatives,
			GL20_OES_stencil1,
			GL20_OES_stencil4,
			GL20_OES_surfaceless_context,
			GL20_OES_tessellation_point_size,
			GL20_OES_tessellation_shader,
			GL20_OES_texture_3D,
			GL20_OES_texture_border_clamp,
			GL20_OES_texture_buffer,
			GL20_OES_texture_compression_astc,
			GL20_OES_texture_cube_map_array,
			GL20_OES_texture_float,
			GL20_OES_texture_float_linear,
			GL20_OES_texture_half_float,
			GL20_OES_texture_half_float_linear,
			GL20_OES_texture_npot,
			GL20_OES_texture_stencil8,
			GL20_OES_texture_storage_multisample_2d_array,
			GL20_OES_texture_view,
			GL20_OES_vertex_array_object,
			GL20_OES_vertex_half_float,
			GL20_OES_vertex_type_10_10_10_2,
			GL20_AMD_compressed_3DC_texture,
			GL20_AMD_compressed_ATC_texture,
			GL20_AMD_performance_monitor,
			GL20_AMD_program_binary_Z400,
			GL20_ANDROID_extension_pack_es31a,
			GL20_ANGLE_depth_texture,
			GL20_ANGLE_framebuffer_blit,
			GL20_ANGLE_framebuffer_multisample,
			GL20_ANGLE_instanced_arrays,
			GL20_ANGLE_pack_reverse_row_order,
			GL20_ANGLE_program_binary,
			GL20_ANGLE_texture_compression_dxt3,
			GL20_ANGLE_texture_compression_dxt5,
			GL20_ANGLE_texture_usage,
			GL20_ANGLE_translated_shader_source,
			GL20_APPLE_clip_distance,
			GL20_APPLE_color_buffer_packed_float,
			GL20_APPLE_copy_texture_levels,
			GL20_APPLE_framebuffer_multisample,
			GL20_APPLE_rgb_422,
			GL20_APPLE_sync,
			GL20_APPLE_texture_format_BGRA8888,
			GL20_APPLE_texture_max_level,
			GL20_APPLE_texture_packed_float,
			GL20_ARM_mali_program_binary,
			GL20_ARM_mali_shader_binary,
			GL20_ARM_rgba8,
			GL20_ARM_shader_framebuffer_fetch,
			GL20_ARM_shader_framebuffer_fetch_depth_stencil,
			GL20_DMP_program_binary,
			GL20_DMP_shader_binary,
			GL20_EXT_base_instance,
			GL20_EXT_blend_minmax,
			GL20_EXT_color_buffer_half_float,
			GL20_EXT_color_buffer_float,
			GL20_EXT_copy_image,
			GL20_EXT_debug_label,
			GL20_EXT_debug_marker,
			GL20_EXT_discard_framebuffer,
			GL20_EXT_disjoint_timer_query,
			GL20_EXT_draw_buffers,
			GL20_EXT_draw_buffers_indexed,
			GL20_EXT_draw_elements_base_vertex,
			GL20_EXT_draw_instanced,
			GL20_EXT_geometry_point_size,
			GL20_EXT_geometry_shader,
			GL20_EXT_gpu_shader5,
			GL20_EXT_instanced_arrays,
			GL20_EXT_map_buffer_range,
			GL20_EXT_multi_draw_arrays,
			GL20_EXT_multi_draw_indirect,
			GL20_EXT_multisampled_render_to_texture,
			GL20_EXT_multiview_draw_buffers,
			GL20_EXT_occlusion_query_boolean,
			GL20_EXT_primitive_bounding_box,
			GL20_EXT_pvrtc_sRGB,
			GL20_EXT_read_format_bgra,
			GL20_EXT_render_snorm,
			GL20_EXT_robustness,
			GL20_EXT_sRGB,
			GL20_EXT_sRGB_write_control,
			GL20_EXT_separate_shader_objects,
			GL20_EXT_shader_framebuffer_fetch,
			GL20_EXT_shader_implicit_conversions,
			GL20_EXT_shader_integer_mix,
			GL20_EXT_shader_io_blocks,
			GL20_EXT_shader_pixel_local_storage,
			GL20_EXT_shader_texture_lod,
			GL20_EXT_shadow_samplers,
			GL20_EXT_tessellation_point_size,
			GL20_EXT_tessellation_shader,
			GL20_EXT_texture_border_clamp,
			GL20_EXT_texture_buffer,
			GL20_EXT_texture_compression_dxt1,
			GL20_EXT_texture_compression_s3tc,
			GL20_EXT_texture_cube_map_array,
			GL20_EXT_texture_filter_anisotropic,
			GL20_EXT_texture_format_BGRA8888,
			GL20_EXT_texture_norm16,
			GL20_EXT_texture_rg,
			GL20_EXT_texture_sRGB_decode,
			GL20_EXT_texture_storage,
			GL20_EXT_texture_type_2_10_10_10_REV,
			GL20_EXT_texture_view,
			GL20_EXT_unpack_subimage,
			GL20_FJ_shader_binary_GCCSO,
			GL20_IMG_multisampled_render_to_texture,
			GL20_IMG_program_binary,
			GL20_IMG_read_format,
			GL20_IMG_shader_binary,
			GL20_IMG_texture_compression_pvrtc,
			GL20_IMG_texture_compression_pvrtc2,
			GL20_INTEL_performance_query,
			GL20_NV_bindless_texture,
			GL20_NV_blend_equation_advanced,
			GL20_NV_blend_equation_advanced_coherent,
			GL20_NV_conditional_render,
			GL20_NV_copy_buffer,
			GL20_NV_coverage_sample,
			GL20_NV_depth_nonlinear,
			GL20_NV_draw_buffers,
			GL20_NV_draw_instanced,
			GL20_NV_explicit_attrib_location,
			GL20_NV_fbo_color_attachments,
			GL20_NV_fence,
			GL20_NV_framebuffer_blit,
			GL20_NV_framebuffer_multisample,
			GL20_NV_generate_mipmap_sRGB,
			GL20_NV_image_formats,
			GL20_NV_instanced_arrays,
			GL20_NV_internalformat_sample_query,
			GL20_NV_non_square_matrices,
			GL20_NV_path_rendering,
			GL20_NV_read_buffer,
			GL20_NV_read_buffer_front,
			GL20_NV_read_depth,
			GL20_NV_read_depth_stencil,
			GL20_NV_read_stencil,
			GL20_NV_sRGB_formats,
			GL20_NV_shader_noperspective_interpolation,
			GL20_NV_shadow_samplers_array,
			GL20_NV_shadow_samplers_cube,
			GL20_NV_texture_border_clamp,
			GL20_NV_texture_compression_s3tc_update,
			GL20_NV_texture_npot_2D_mipmap,
			GL20_NV_viewport_array,
			GL20_QCOM_alpha_test,
			GL20_QCOM_binning_control,
			GL20_QCOM_driver_control,
			GL20_QCOM_extended_get,
			GL20_QCOM_extended_get2,
			GL20_QCOM_perfmon_global_mode,
			GL20_QCOM_tiled_rendering,
			GL20_QCOM_writeonly_rendering,
			GL20_VIV_shader_binary,
			GL20_BeginRange = GL20_KHR_blend_equation_advanced,
			GL20_EndRange = GL20_VIV_shader_binary,
			GL20_RangeSize = (GL20_EndRange - GL20_BeginRange + 1),
			GL20_MaxEnum = 0x7FFFFFFF
		};

		typedef std::shared_ptr<class GL20Device> GL20DevicePtr;
		typedef std::shared_ptr<class OGLSwapchain> GL20SwapchainPtr;
		typedef std::shared_ptr<class GL20Surface> GL20SurfacePtr;
		typedef std::shared_ptr<class GL20DeviceContext> GL20DeviceContextPtr;
		typedef std::shared_ptr<class GL20FramebufferLayout> GL20FramebufferLayoutPtr;
		typedef std::shared_ptr<class GL20Framebuffer> GL20FramebufferPtr;
		typedef std::shared_ptr<class GL20Shader> GL20ShaderPtr;
		typedef std::shared_ptr<class GL20Program> GL20ProgramPtr;
		typedef std::shared_ptr<class GL20GraphicsData> GL20GraphicsDataPtr;
		typedef std::shared_ptr<class GL20InputLayout> GL20InputLayoutPtr;
		typedef std::shared_ptr<class GL20GraphicsState> GL20GraphicsStatePtr;
		typedef std::shared_ptr<class GL20Texture> GL20TexturePtr;
		typedef std::shared_ptr<class GL20Sampler> GL20SamplerPtr;
		typedef std::shared_ptr<class GL20Pipeline> GL20PipelinePtr;
		typedef std::shared_ptr<class GL20DescriptorSetPool> GL20DescriptorSetPoolPtr;
		typedef std::shared_ptr<class GL20DescriptorSet> GL20DescriptorSetPtr;
		typedef std::shared_ptr<class GL20DescriptorSetLayout> GL20DescriptorSetLayoutPtr;
		typedef std::shared_ptr<class GL20GraphicsAttribute> GL20GraphicsAttributePtr;
		typedef std::shared_ptr<class GL20GraphicsUniform> GL20GraphicsUniformPtr;
		typedef std::shared_ptr<class GL20GraphicsUniformBlock> GL20GraphicsUniformBlockPtr;

		typedef std::weak_ptr<class GL20Device> GL20DeviceWeakPtr;
		typedef std::weak_ptr<class GL20Surface> GL20SurfaceWeakPtr;
		typedef std::weak_ptr<class GL20Swapchain> GL20SwapchainWeakPtr;
		typedef std::weak_ptr<class GL20DeviceContext> GL20DeviceContextWeakPtr;
		typedef std::weak_ptr<class GL20FramebufferLayout> GL20FramebufferLayoutWeakPtr;
		typedef std::weak_ptr<class GL20Framebuffer> GL20FramebufferWeakPtr;
		typedef std::weak_ptr<class GL20Shader> GL20ShaderWeakPtr;
		typedef std::weak_ptr<class GL20Program> GL20ProgramWeakPtr;
		typedef std::weak_ptr<class GL20GraphicsData> GL20GraphicsDataWeakPtr;
		typedef std::weak_ptr<class GL20InputLayout> GL20InputLayoutWeakPtr;
		typedef std::weak_ptr<class GL20GraphicsState> GL20GraphicsStateWeakPtr;
		typedef std::weak_ptr<class GL20Texture> GL20TextureWeakPtr;
		typedef std::weak_ptr<class GL20Sampler> GL20SamplerWeakPtr;
		typedef std::weak_ptr<class GL20Pipeline> GL20PipelineWeakPtr;
		typedef std::weak_ptr<class GL20DescriptorSetPool> GL20DescriptorSetPoolWeakPtr;
		typedef std::weak_ptr<class GL20DescriptorSet> GL20DescriptorSetWeakPtr;
		typedef std::weak_ptr<class GL20DescriptorSetLayout> GL20DescriptorSetLayoutWeakPtr;
		typedef std::weak_ptr<class GL20GraphicsAttribute> GL20GraphicsAttributeWeakPtr;
		typedef std::weak_ptr<class GL20GraphicsUniform> GL20GraphicsUniformWeakPtr;
		typedef std::weak_ptr<class GL20GraphicsUniformBlock> GL20GraphicsUniformBlockWeakPtr;

		typedef std::vector<GL20ShaderPtr> GL20Shaders;

		struct GL20VertexBuffer
		{
			bool setDirty;
			std::intptr_t offset;
			GL20GraphicsDataPtr vbo;
		};

		typedef std::vector<GL20VertexBuffer> GL20VertexBuffers;

		class GL20Types
		{
		public:
			static bool setup() noexcept;

			static GLenum asVertexType(VertexType type) noexcept;
			static GLenum asVertexFormat(GraphicsFormat format) noexcept;
			static GLenum asIndexType(IndexFormat type) noexcept;
			static GLenum asShaderStage(ShaderStageFlags stage) noexcept;
			static GLenum asTextureTarget(TextureDimension dim) noexcept;
			static GLenum asTextureFormat(GraphicsFormat format) noexcept;
			static GLenum asTextureType(GraphicsFormat format) noexcept;
			static GLenum asTextureInternalFormat(GraphicsFormat format) noexcept;
			static GLenum asCompareFunction(CompareFunction func) noexcept;
			static GLenum asBlendFactor(BlendMode func) noexcept;
			static GLenum asBlendOperation(BlendOp op) noexcept;
			static GLenum asCullMode(CullMode mode) noexcept;
			static GLenum asFrontFace(FrontFace face) noexcept;
			static GLenum asFillMode(PolygonMode mode) noexcept;
			static GLenum asStencilOperation(StencilOp stencilop) noexcept;
			static GLenum asSamplerWrap(SamplerWrap wrap) noexcept;
			static GLenum asSamplerMinFilter(SamplerFilter filter) noexcept;
			static GLenum asSamplerMagFilter(SamplerFilter filter) noexcept;

			static GLsizei getFormatNum(GLenum format, GLenum type) noexcept;
			static GLsizei getCompressedTextureSize(GLsizei width, GLsizei height, GLsizei depth, GLenum internalFormat) noexcept;

			static GLboolean isNormFormat(GraphicsFormat format) noexcept;
			static GLboolean isSupportFeature(GL20Features features) noexcept;
			static GLboolean isStencilFormat(GraphicsFormat format) noexcept;
			static GLboolean isDepthFormat(GraphicsFormat format) noexcept;
			static GLboolean isDepthStencilFormat(GraphicsFormat format) noexcept;
			static GLboolean isCompressedTexture(GraphicsFormat format) noexcept;

		private:
			static GLboolean _gl20init;
			static GLboolean _gl20Features[GL20Features::GL20_RangeSize];
		};

		class GL20Check
		{
		public:
			static bool checkError() noexcept;

			static void debugOutput(const char* message, ...) noexcept;
		};
	}
}

#endif