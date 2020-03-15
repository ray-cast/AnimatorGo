#ifndef OCTOON_BLINN_MATERIAL_H_
#define OCTOON_BLINN_MATERIAL_H_

#include <octoon/video/render_pipeline.h>
#include <octoon/hal/graphics.h>

namespace octoon::material
{
	class OCTOON_EXPORT BlinnMaterial final : public video::RenderPipeline
	{
	public:
		BlinnMaterial() except;
		~BlinnMaterial() noexcept;

		void setup() except;

		void setTransform(const math::float4x4& m) noexcept override;
		void setViewProjection(const math::float4x4& vp) noexcept override;

		const hal::GraphicsPipelinePtr& getPipeline() const noexcept override;
		const hal::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept override;

		void setLightDir(const math::float3& translate) noexcept;
		void setBaseColor(const math::float3& colors) noexcept;
		void setAmbientColor(const math::float3& colors) noexcept;
		void setShininess(float shininess) noexcept;

		const math::float3& getLightDir() const noexcept;
		const math::float3& getBaseColor() const noexcept;
		const math::float3& getAmbientColor() const noexcept;
		float getShininess() const noexcept;

		video::RenderPipelinePtr clone() const noexcept override;

	private:
		BlinnMaterial(const BlinnMaterial&) = delete;
		BlinnMaterial& operator=(const BlinnMaterial&) = delete;

	private:
		hal::GraphicsPipelinePtr pipeline_;
		hal::GraphicsDescriptorSetPtr descriptorSet_;

		hal::GraphicsUniformSetPtr proj_;
		hal::GraphicsUniformSetPtr model_;
		hal::GraphicsUniformSetPtr lightDir_;

		hal::GraphicsUniformSetPtr baseColor_;
		hal::GraphicsUniformSetPtr ambientColor_;

		hal::GraphicsUniformSetPtr shininess_;
	};
}

#endif