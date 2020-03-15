#ifndef OCTOON_LINE_BASIC_MATERIAL_H_
#define OCTOON_LINE_BASIC_MATERIAL_H_

#include <octoon/material/material.h>

namespace octoon::material
{
	class OCTOON_EXPORT LineBasicMaterial final : public Material
	{
		OctoonDeclareSubClass(LineBasicMaterial, Material);
	public:
		LineBasicMaterial() noexcept;
		LineBasicMaterial(const math::float3& color) noexcept;
		virtual ~LineBasicMaterial() noexcept;

		void setColor(const math::float3& color) noexcept;
		const math::float3& getColor() const noexcept;

		void setOpacity(float opacity) noexcept;
		float getOpacity() const noexcept;

		std::shared_ptr<Material> clone() const noexcept override;

	private:
		LineBasicMaterial(const LineBasicMaterial&) = delete;
		LineBasicMaterial& operator=(const LineBasicMaterial&) = delete;

	private:
		float opacity_;
		math::float3 color_;
	};
}

#endif