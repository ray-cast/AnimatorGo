#ifndef OCTOON_LAMBERT_MATERIAL_H_
#define OCTOON_LAMBERT_MATERIAL_H_

#include <octoon/material/material.h>

namespace octoon::material
{
	class OCTOON_EXPORT LambertMaterial final : public Material
	{
		OctoonDeclareSubClass(LambertMaterial, Material);
	public:
		LambertMaterial() noexcept;
		LambertMaterial(const math::float3& color) noexcept;
		virtual ~LambertMaterial() noexcept;

		void setColor(const math::float3& color) noexcept;
		const math::float3& getColor() const noexcept;

		void setOpacity(float opacity) noexcept;
		float getOpacity() const noexcept;

		void setColorMap(const hal::GraphicsTexturePtr& map) noexcept;
		const hal::GraphicsTexturePtr& getColorMap() const noexcept;

		std::shared_ptr<Material> clone() const noexcept override;

	private:
		LambertMaterial(const LambertMaterial&) = delete;
		LambertMaterial& operator=(const LambertMaterial&) = delete;

	private:
		float opacity_;
		math::float3 color_;
		hal::GraphicsTexturePtr colorTexture_;
	};
}

#endif