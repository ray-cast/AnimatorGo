#ifndef OCTOON_RENDER_PIPELINE_H_
#define OCTOON_RENDER_PIPELINE_H_

#include <octoon/video/render_types.h>
#include <octoon/hal/graphics_types.h>
#include <octoon/material/material.h>

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

			virtual void setMaterial(const material::MaterialPtr& material) noexcept;
			virtual const material::MaterialPtr& getMaterial() const noexcept;

			virtual void setTransform(const math::float4x4& vp) noexcept;
			virtual void setViewProjection(const math::float4x4& vp) noexcept;

			virtual const hal::GraphicsPipelinePtr& getPipeline() const noexcept;
			virtual const hal::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept;

			virtual std::shared_ptr<RenderPipeline> clone() const noexcept;

		private:
			void updateParameters() noexcept;

		private:
			virtual void onMaterialReplace(const material::MaterialPtr& material) noexcept(false);

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
			hal::GraphicsUniformSetPtr color_;
			hal::GraphicsUniformSetPtr decal_;
			hal::GraphicsUniformSetPtr hasTexture_;
		};
	}
}

#endif