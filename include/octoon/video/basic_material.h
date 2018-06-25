#ifndef OCTOON_BASIC_MATERIAL_H_
#define OCTOON_BASIC_MATERIAL_H_

#include <octoon/video/material.h>
#include <octoon/graphics/graphics.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT BasicMaterial final : public Material
		{
		public:
			BasicMaterial() except;
			BasicMaterial(const graphics::GraphicsTexturePtr& texture) except;
			~BasicMaterial() noexcept;

			void setup() except;

			void setTransform(const math::float4x4& m) noexcept override;
			void setViewProjection(const math::float4x4& vp) noexcept override;
			void setBaseColor(const math::float4& texture) noexcept;
			void setTexture(const graphics::GraphicsTexturePtr& texture) noexcept;

			const graphics::GraphicsPipelinePtr& getPipeline() const noexcept override;
			const graphics::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept override;

			MaterialPtr clone() const noexcept override;

		private:
			BasicMaterial(const BasicMaterial&) = delete;
			BasicMaterial& operator=(const BasicMaterial&) = delete;

		private:
			graphics::GraphicsPipelinePtr pipeline_;
			graphics::GraphicsDescriptorSetPtr descriptorSet_;

			graphics::GraphicsUniformSetPtr proj_;
			graphics::GraphicsUniformSetPtr model_;
			graphics::GraphicsUniformSetPtr color_;
			graphics::GraphicsUniformSetPtr decal_;
			graphics::GraphicsUniformSetPtr hasTexture_;
		};
	}
}

#endif