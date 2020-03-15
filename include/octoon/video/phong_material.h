#ifndef OCTOON_PHONG_MATERIAL_H_
#define OCTOON_PHONG_MATERIAL_H_

#include <octoon/video/render_pipeline.h>
#include <octoon/hal/graphics.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT PhongMaterial final : public RenderPipeline
		{
		public:
			PhongMaterial() except;
			~PhongMaterial() noexcept;

			void setup() except;

			void setTransform(const math::float4x4& m) noexcept override;
			void setViewProjection(const math::float4x4& vp) noexcept override;

			const hal::GraphicsPipelinePtr& getPipeline() const noexcept override;
			const hal::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept override;

			void setLightDir(const math::float3& translate) noexcept;
			void setLightIntensity(const math::float1& intensity) noexcept;
			void setBaseColor(const math::float3& color) noexcept;
			void setAmbient(const math::float1& weight) noexcept;
			void setAmbientColor(const math::float3& color) noexcept;
			void setSpecularColor(const math::float3& color) noexcept;
			void setDarkColor(const math::float3& color) noexcept;
			void setShininess(float shininess) noexcept;

			math::float1 getAmbient() const noexcept;
			math::float1 getLightIntensity() const noexcept;

			const math::float3& getLightDir() const noexcept;
			const math::float3& getBaseColor() const noexcept;
			const math::float3& getAmbientColor() const noexcept;
			const math::float3& getSpecularColor() const noexcept;
			const math::float3& getDarkColor() const noexcept;
			float getShininess() const noexcept;

			RenderPipelinePtr clone() const noexcept override;

		private:
			PhongMaterial(const PhongMaterial&) = delete;
			PhongMaterial& operator=(const PhongMaterial&) = delete;

		private:
			hal::GraphicsPipelinePtr pipeline_;
			hal::GraphicsDescriptorSetPtr descriptorSet_;

			hal::GraphicsUniformSetPtr proj_;
			hal::GraphicsUniformSetPtr model_;
			hal::GraphicsUniformSetPtr lightDir_;
			hal::GraphicsUniformSetPtr lightIntensity_;

			hal::GraphicsUniformSetPtr baseColor_;
			hal::GraphicsUniformSetPtr ambient_;
			hal::GraphicsUniformSetPtr ambientColor_;
			hal::GraphicsUniformSetPtr specularColor_;
			hal::GraphicsUniformSetPtr darkColor_;

			hal::GraphicsUniformSetPtr shininess_;
		};
	}
}

#endif