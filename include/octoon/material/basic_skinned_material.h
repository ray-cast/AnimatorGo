#ifndef OCTOON_BASIC_SKINNED_MATERIAL_H_
#define OCTOON_BASIC_SKINNED_MATERIAL_H_

#include <octoon/video/render_pipeline.h>
#include <octoon/hal/graphics.h>

namespace octoon::material
{
	class OCTOON_EXPORT BasicSkinnedMaterial final : public video::RenderPipeline
	{
	public:
		BasicSkinnedMaterial() except;
		BasicSkinnedMaterial(const hal::GraphicsTexturePtr& texture) except;
		~BasicSkinnedMaterial() noexcept;

		void setup() except;

		void setTransform(const math::float4x4& m) noexcept override;
		void setViewProjection(const math::float4x4& vp) noexcept override;
		void setBaseColor(const math::float4& texture) noexcept;
		void setTexture(const hal::GraphicsTexturePtr& texture) noexcept;

		const hal::GraphicsPipelinePtr& getPipeline() const noexcept override;
		const hal::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept override;

		video::RenderPipelinePtr clone() const noexcept override;

	private:
		BasicSkinnedMaterial(const BasicSkinnedMaterial&) = delete;
		BasicSkinnedMaterial& operator=(const BasicSkinnedMaterial&) = delete;

	private:
		hal::GraphicsPipelinePtr pipeline_;
		hal::GraphicsDescriptorSetPtr descriptorSet_;

		hal::GraphicsUniformSetPtr proj_;
		hal::GraphicsUniformSetPtr model_;
		hal::GraphicsUniformSetPtr color_;
		hal::GraphicsUniformSetPtr decal_;
		hal::GraphicsUniformSetPtr hasTexture_;
	};
}

#endif