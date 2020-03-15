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
			~RenderPipeline() noexcept;

			void setName(std::string_view name) noexcept;
			const std::string& getName() const noexcept;

			void setMaterial(const material::MaterialPtr& material) noexcept;
			const material::MaterialPtr& getMaterial() const noexcept;

			virtual void setTransform(const math::float4x4& vp) noexcept = 0;
			virtual void setViewProjection(const math::float4x4& vp) noexcept = 0;

			virtual const hal::GraphicsPipelinePtr& getPipeline() const noexcept = 0;
			virtual const hal::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept = 0;

			virtual hal::GraphicsUniformSetPtr at(std::string_view name) const;

			virtual std::shared_ptr<RenderPipeline> clone() const noexcept = 0;

		private:
			virtual void onMaterialReplace(const material::MaterialPtr& material) noexcept(false);

		private:
			RenderPipeline(const RenderPipeline&) = delete;
			RenderPipeline& operator=(const RenderPipeline&) = delete;

		private:
			std::string name_;
			material::MaterialPtr material_;
		};
	}
}

#endif