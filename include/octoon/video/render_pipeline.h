#ifndef OCTOON_RENDER_PIPELINE_H_
#define OCTOON_RENDER_PIPELINE_H_

#include <octoon/video/render_types.h>
#include <octoon/hal/graphics_types.h>
#include <octoon/material/material.h>
#include <octoon/video/geometry.h>
#include <octoon/camera/camera.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT RenderPipeline
		{
		public:
			RenderPipeline() noexcept;
			RenderPipeline(const material::MaterialPtr& material) noexcept;
			virtual ~RenderPipeline() noexcept;

			void setMaterial(const material::MaterialPtr& material) noexcept;
			const material::MaterialPtr& getMaterial() const noexcept;

			void setTransform(const math::float4x4& vp) noexcept;
			void setViewProjection(const math::float4x4& vp) noexcept;

			const hal::GraphicsPipelinePtr& getPipeline() const noexcept;
			const hal::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept;

			void update(const camera::Camera& camera, const geometry::Geometry& geometry) noexcept;

			std::shared_ptr<RenderPipeline> clone() const noexcept;

		private:
			void updateParameters() noexcept;

		private:
			virtual void updateMaterial(const material::MaterialPtr& material) noexcept(false);

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
}

#endif