#ifndef OCTOON_GGX_MATERIAL_H_
#define OCTOON_GGX_MATERIAL_H_

#include <octoon/video/material.h>
#include <octoon/hal/graphics.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT GGXMaterial final : public HALMaterial
		{
		public:
			GGXMaterial() except;
			GGXMaterial(const hal::GraphicsTexturePtr& texture) except;
			~GGXMaterial() noexcept;

			void setup() except;

			void setTransform(const math::float4x4& m) noexcept override;
			void setViewProjection(const math::float4x4& vp) noexcept override;

			const hal::GraphicsPipelinePtr& getPipeline() const noexcept override;
			const hal::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept override;

			void setLightDir(const math::float3& translate) noexcept;
			void setBaseColor(const math::float3& colors) noexcept;
			void setAmbientColor(const math::float3& colors) noexcept;
			void setSpecularColor(const math::float3& colors) noexcept;
			void setTexture(const hal::GraphicsTexturePtr& texture) noexcept;

			void setSmoothness(float smoothness) noexcept;
			void setMetalness(float metalness) noexcept;

			const math::float3& getLightDir() const noexcept;
			const math::float3& getBaseColor() const noexcept;
			const math::float3& getAmbientColor() const noexcept;
			const math::float3& getSpecularColor() const noexcept;
			const hal::GraphicsTexturePtr& getTexture() noexcept;

			float getSmoothness() const noexcept;
			float getMetalness() const noexcept;

			HALMaterialPtr clone() const noexcept override;

		private:
			GGXMaterial(const GGXMaterial&) = delete;
			GGXMaterial& operator=(const GGXMaterial&) = delete;

		private:
			hal::GraphicsPipelinePtr pipeline_;
			hal::GraphicsDescriptorSetPtr descriptorSet_;

			hal::GraphicsUniformSetPtr proj_;
			hal::GraphicsUniformSetPtr model_;
			hal::GraphicsUniformSetPtr lightDir_;

			hal::GraphicsUniformSetPtr baseColor_;
			hal::GraphicsUniformSetPtr ambientColor_;
			hal::GraphicsUniformSetPtr specularColor_;

			hal::GraphicsUniformSetPtr smoothness_;
			hal::GraphicsUniformSetPtr metalness_;
			hal::GraphicsUniformSetPtr decal_;
		};
	}
}

#endif