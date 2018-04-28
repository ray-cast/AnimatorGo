#ifndef OCTOON_TEXT_MATERIAL_H_
#define OCTOON_TEXT_MATERIAL_H_

#include <octoon/video/material.h>
#include <octoon/graphics/graphics.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT LineMaterial final : public Material
		{
		public:
			LineMaterial() except;
			~LineMaterial() noexcept;

			void setup() except;

			void setTransform(const math::float4x4& m) noexcept override;
			void setViewProjection(const math::float4x4& vp) noexcept override;

			const graphics::GraphicsPipelinePtr& getPipeline() const noexcept override;
			const graphics::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept override;

			void setColor(const math::float3& colors) noexcept;
			const math::float3& getColor() const noexcept;

			MaterialPtr clone() const noexcept override;

		private:
			LineMaterial(const LineMaterial&) = delete;
			LineMaterial& operator=(const LineMaterial&) = delete;

		private:
			graphics::GraphicsPipelinePtr pipeline_;
			graphics::GraphicsDescriptorSetPtr descriptorSet_;

			graphics::GraphicsUniformSetPtr proj_;
			graphics::GraphicsUniformSetPtr model_;
			graphics::GraphicsUniformSetPtr color_;
		};
	}
}

#endif