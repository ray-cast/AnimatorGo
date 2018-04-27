#ifndef OCTOON_PHONG_MATERIAL_H_
#define OCTOON_PHONG_MATERIAL_H_

#include <octoon/video/material.h>
#include <octoon/graphics/graphics.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT PhongMaterial final : public Material
		{
		public:
			PhongMaterial() except;
			~PhongMaterial() noexcept;

			void setup() except;

			void setTransform(const math::float4x4& m) noexcept override;
			void setViewProjection(const math::float4x4& vp) noexcept override;

			graphics::GraphicsPipelinePtr getPipeline() const noexcept override;
			graphics::GraphicsDescriptorSetPtr getDescriptorSet() const noexcept override;

			void setLean(float lean) noexcept;
			void setTextColor(TextColor::Type which, const math::float3& colors) except;
			void setTranslate(const math::float3& translate) noexcept;

			float getLean() const noexcept;
			const math::float3& getTranslate() const noexcept;

			const math::float3& getTextColor(TextColor::Type which) const except;

			MaterialPtr clone() const noexcept override;

		private:
			PhongMaterial(const PhongMaterial&) = delete;
			PhongMaterial& operator=(const PhongMaterial&) = delete;

		private:
			graphics::GraphicsPipelinePtr pipeline_;
			graphics::GraphicsDescriptorSetPtr descriptorSet_;

			graphics::GraphicsUniformSetPtr proj_;
			graphics::GraphicsUniformSetPtr model_;
			graphics::GraphicsUniformSetPtr translate_;
			graphics::GraphicsUniformSetPtr lean_;
			graphics::GraphicsUniformSetPtr frontColor_;
			graphics::GraphicsUniformSetPtr sideColor_;
		};
	}
}

#endif