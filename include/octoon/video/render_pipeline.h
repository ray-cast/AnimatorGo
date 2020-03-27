#ifndef OCTOON_RENDER_PIPELINE_H_
#define OCTOON_RENDER_PIPELINE_H_

#include <octoon/camera/camera.h>
#include <octoon/geometry/geometry.h>
#include <octoon/material/material.h>

#include <octoon/video/render_context.h>

namespace octoon::video
{
	class OCTOON_EXPORT RenderPipeline final
	{
	public:
		RenderPipeline() noexcept;
		RenderPipeline(const material::MaterialPtr& material, const RenderContext& context) noexcept;
		virtual ~RenderPipeline() noexcept;

		void setMaterial(const material::MaterialPtr& material, const RenderContext& context) noexcept;
		const material::MaterialPtr& getMaterial() const noexcept;

		void setTransform(const math::float4x4& vp) noexcept;
		void setViewProjection(const math::float4x4& vp) noexcept;

		const hal::GraphicsPipelinePtr& getPipeline() const noexcept;
		const hal::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept;

		void update(const camera::Camera& camera, const geometry::Geometry& geometry) noexcept;

	private:
		void updateParameters() noexcept;
		void updateMaterial(const material::MaterialPtr& material, const RenderContext& context) noexcept(false);

	private:
		RenderPipeline(const RenderPipeline&) = delete;
		RenderPipeline& operator=(const RenderPipeline&) = delete;

	private:
		material::MaterialPtr material_;

		hal::GraphicsPipelinePtr pipeline_;
		hal::GraphicsStatePtr renderState_;
		hal::GraphicsDescriptorSetPtr descriptorSet_;

		hal::GraphicsUniformSetPtr proj_;
		hal::GraphicsUniformSetPtr model_;
	};
}

#endif