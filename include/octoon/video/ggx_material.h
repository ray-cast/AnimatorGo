#ifndef OCTOON_GGX_MATERIAL_H_
#define OCTOON_GGX_MATERIAL_H_

#include <octoon/video/material.h>
#include <octoon/hal/graphics.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT GGXMaterial final : public Material
		{
		public:
			GGXMaterial() except;
			GGXMaterial(const graphics::GraphicsTexturePtr& texture) except;
			~GGXMaterial() noexcept;

			void setup() except;

			void setTransform(const math::float4x4& m) noexcept override;
			void setViewProjection(const math::float4x4& vp) noexcept override;

			const graphics::GraphicsPipelinePtr& getPipeline() const noexcept override;
			const graphics::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept override;

			void setLightDir(const math::float3& translate) noexcept;
			void setBaseColor(const math::float3& colors) noexcept;
			void setAmbientColor(const math::float3& colors) noexcept;
			void setSpecularColor(const math::float3& colors) noexcept;
			void setTexture(const graphics::GraphicsTexturePtr& texture) noexcept;

			void setSmoothness(float smoothness) noexcept;
			void setMetalness(float metalness) noexcept;

			const math::float3& getLightDir() const noexcept;
			const math::float3& getBaseColor() const noexcept;
			const math::float3& getAmbientColor() const noexcept;
			const math::float3& getSpecularColor() const noexcept;
			const graphics::GraphicsTexturePtr& getTexture() noexcept;

			float getSmoothness() const noexcept;
			float getMetalness() const noexcept;

			MaterialPtr clone() const noexcept override;

		private:
			GGXMaterial(const GGXMaterial&) = delete;
			GGXMaterial& operator=(const GGXMaterial&) = delete;

		private:
			graphics::GraphicsPipelinePtr pipeline_;
			graphics::GraphicsDescriptorSetPtr descriptorSet_;

			graphics::GraphicsUniformSetPtr proj_;
			graphics::GraphicsUniformSetPtr model_;
			graphics::GraphicsUniformSetPtr lightDir_;

			graphics::GraphicsUniformSetPtr baseColor_;
			graphics::GraphicsUniformSetPtr ambientColor_;
			graphics::GraphicsUniformSetPtr specularColor_;

			graphics::GraphicsUniformSetPtr smoothness_;
			graphics::GraphicsUniformSetPtr metalness_;
			graphics::GraphicsUniformSetPtr decal_;
		};
	}
}

#endif