#ifndef OCTOON_MATERIAL_H_
#define OCTOON_MATERIAL_H_

#include <octoon/video/render_types.h>
#include <octoon/hal/graphics_types.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT HALMaterial
		{
		public:
			HALMaterial() noexcept;
			HALMaterial(std::string_view name) noexcept;
			~HALMaterial() noexcept;

			void setName(std::string_view name) noexcept;
			const std::string& getName() const noexcept;

			virtual void setTransform(const math::float4x4& vp) noexcept = 0;
			virtual void setViewProjection(const math::float4x4& vp) noexcept = 0;

			virtual const hal::GraphicsPipelinePtr& getPipeline() const noexcept = 0;
			virtual const hal::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept = 0;

			virtual hal::GraphicsUniformSetPtr at(std::string_view name) const;

			virtual HALMaterialPtr clone() const noexcept = 0;

		private:
			HALMaterial(const HALMaterial&) = delete;
			HALMaterial& operator=(const HALMaterial&) = delete;

		private:
			std::string _name;
		};
	}
}

#endif