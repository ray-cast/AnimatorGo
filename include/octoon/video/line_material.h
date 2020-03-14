#ifndef OCTOON_LINE_MATERIAL_H_
#define OCTOON_LINE_MATERIAL_H_

#include <octoon/video/material.h>
#include <octoon/hal/graphics.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT LineMaterial final : public HALMaterial
		{
		public:
			LineMaterial() except;
			LineMaterial(float lineWidth) except;
			~LineMaterial() noexcept;

			void setup() except;

			void setTransform(const math::float4x4& m) noexcept override;
			void setViewProjection(const math::float4x4& vp) noexcept override;

			const hal::GraphicsPipelinePtr& getPipeline() const noexcept override;
			const hal::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept override;

			void setColor(const math::float3& colors) noexcept;
			const math::float3& getColor() const noexcept;

			HALMaterialPtr clone() const noexcept override;

		private:
			LineMaterial(const LineMaterial&) = delete;
			LineMaterial& operator=(const LineMaterial&) = delete;

		private:
			float lineWidth_;

			hal::GraphicsPipelinePtr pipeline_;
			hal::GraphicsDescriptorSetPtr descriptorSet_;

			hal::GraphicsUniformSetPtr proj_;
			hal::GraphicsUniformSetPtr model_;
			hal::GraphicsUniformSetPtr color_;
		};
	}
}

#endif