#ifndef OCTOON_BASIC_MATERIAL_H_
#define OCTOON_BASIC_MATERIAL_H_

#include <octoon/video/material.h>
#include <octoon/hal/graphics.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT BasicMaterial final : public Material
		{
		public:
			BasicMaterial() except;
			BasicMaterial(const hal::GraphicsTexturePtr& texture) except;
			~BasicMaterial() noexcept;

			void setup() except;

			void setTransform(const math::float4x4& m) noexcept override;
			void setViewProjection(const math::float4x4& vp) noexcept override;
			void setBaseColor(const math::float4& texture) noexcept;
			void setTexture(const hal::GraphicsTexturePtr& texture) noexcept;

			const hal::GraphicsPipelinePtr& getPipeline() const noexcept override;
			const hal::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept override;

			MaterialPtr clone() const noexcept override;

		private:
			BasicMaterial(const BasicMaterial&) = delete;
			BasicMaterial& operator=(const BasicMaterial&) = delete;

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
}

#endif