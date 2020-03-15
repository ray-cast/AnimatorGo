#ifndef OCTOON_TEXT_MATERIAL_H_
#define OCTOON_TEXT_MATERIAL_H_

#include <octoon/video/render_pipeline.h>
#include <octoon/hal/graphics.h>

namespace octoon::material
{
	struct TextColor
	{
		enum Type
		{
			FrontColor,
			SideColor,
			BeginRange_ = FrontColor,
			EndRange_ = SideColor,
			RangeSize_ = (EndRange_ - BeginRange_) + 1,
		};
	};

	class OCTOON_EXPORT TextMaterial final : public video::RenderPipeline
	{
	public:
		TextMaterial() except;
		~TextMaterial() noexcept;

		void setup() except;

		void setTransform(const math::float4x4& m) noexcept override;
		void setViewProjection(const math::float4x4& vp) noexcept override;

		const hal::GraphicsPipelinePtr& getPipeline() const noexcept override;
		const hal::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept override;

		void setLean(float lean) noexcept;
		void setTextColor(TextColor::Type which, const math::float3& colors) except;
		void setTranslate(const math::float3& translate) noexcept;

		float getLean() const noexcept;
		const math::float3& getTranslate() const noexcept;

		const math::float3& getTextColor(TextColor::Type which) const except;

		video::RenderPipelinePtr clone() const noexcept override;

	private:
		TextMaterial(const TextMaterial&) = delete;
		TextMaterial& operator=(const TextMaterial&) = delete;

	private:
		hal::GraphicsPipelinePtr pipeline_;
		hal::GraphicsDescriptorSetPtr descriptorSet_;

		hal::GraphicsUniformSetPtr proj_;
		hal::GraphicsUniformSetPtr model_;
		hal::GraphicsUniformSetPtr translate_;
		hal::GraphicsUniformSetPtr lean_;
		hal::GraphicsUniformSetPtr frontColor_;
		hal::GraphicsUniformSetPtr sideColor_;
	};
}

#endif