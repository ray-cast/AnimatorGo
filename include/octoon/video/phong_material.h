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

			const graphics::GraphicsPipelinePtr& getPipeline() const noexcept override;
			const graphics::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept override;

			void setLightDir(const math::float3& translate) noexcept;
			void setBaseColor(const math::float3& colors) noexcept;
			void setAmbientColor(const math::float3& colors) noexcept;
			void setShininess(float shininess) noexcept;

			const math::float3& getLightDir() const noexcept;
			const math::float3& getBaseColor() const noexcept;
			const math::float3& getAmbientColor() const noexcept;
			float getShininess() const noexcept;

			MaterialPtr clone() const noexcept override;

		private:
			PhongMaterial(const PhongMaterial&) = delete;
			PhongMaterial& operator=(const PhongMaterial&) = delete;

		private:
			graphics::GraphicsPipelinePtr pipeline_;
			graphics::GraphicsDescriptorSetPtr descriptorSet_;

			graphics::GraphicsUniformSetPtr proj_;
			graphics::GraphicsUniformSetPtr model_;
			graphics::GraphicsUniformSetPtr lightDir_;

			graphics::GraphicsUniformSetPtr baseColor_;
			graphics::GraphicsUniformSetPtr ambientColor_;

			graphics::GraphicsUniformSetPtr shininess_;
		};
	}
}

#endif