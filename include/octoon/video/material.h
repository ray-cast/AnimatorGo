#ifndef OCTOON_MATERIAL_H_
#define OCTOON_MATERIAL_H_

#include <octoon/video/render_types.h>
#include <octoon/graphics/graphics_types.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT Material
		{
		public:
			Material() noexcept;
			~Material() noexcept;

			virtual void setTransform(const math::float4x4& vp) noexcept = 0;
			virtual void setViewProjection(const math::float4x4& vp) noexcept = 0;

			virtual graphics::GraphicsPipelinePtr getPipeline() const noexcept = 0;
			virtual graphics::GraphicsDescriptorSetPtr getDescriptorSet() const noexcept = 0;

			virtual MaterialPtr clone() const noexcept = 0;

		private:
			Material(const Material&) = delete;
			Material& operator=(const Material&) = delete;
		};
	}
}

#endif