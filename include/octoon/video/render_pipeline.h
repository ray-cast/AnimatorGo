#ifndef OCTOON_MATERIAL_H_
#define OCTOON_MATERIAL_H_

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
			RenderPipeline(std::string_view name) noexcept;
			virtual ~RenderPipeline() noexcept;

			void setName(std::string_view name) noexcept;
			const std::string& getName() const noexcept;

			virtual void setMaterial(const material::MaterialPtr& material) noexcept;
			virtual const material::MaterialPtr& getMaterial() const noexcept;

			virtual void setTransform(const math::float4x4& vp) noexcept;
			virtual void setViewProjection(const math::float4x4& vp) noexcept;

			virtual const hal::GraphicsPipelinePtr& getPipeline() const noexcept;
			virtual const hal::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept;

			void setBaseColor(const math::float4& texture) noexcept;
			void setTexture(const hal::GraphicsTexturePtr& texture) noexcept;

			hal::GraphicsUniformSetPtr at(std::string_view name) const;

			virtual std::shared_ptr<RenderPipeline> clone() const noexcept;

		private:
			virtual void onMaterialReplace(const material::MaterialPtr& material) noexcept(false);

		private:
			RenderPipeline(const RenderPipeline&) = delete;
			RenderPipeline& operator=(const RenderPipeline&) = delete;

		private:
			std::string name_;
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