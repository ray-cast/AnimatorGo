#ifndef OCTOON_POINTS_MATERIAL_H_
#define OCTOON_POINTS_MATERIAL_H_

#include <octoon/material/material.h>

namespace octoon
{
	class OCTOON_EXPORT PointsMaterial final : public Material
	{
		OctoonDeclareSubClass(PointsMaterial, Material);
	public:
		PointsMaterial() noexcept;
		PointsMaterial(const math::float3& color) noexcept;
		virtual ~PointsMaterial() noexcept;

		void setColor(const math::float3& color) noexcept;
		const math::float3& getColor() const noexcept;

		void setOpacity(float opacity) noexcept;
		float getOpacity() const noexcept;

		std::shared_ptr<Material> clone() const noexcept override;

	private:
		PointsMaterial(const PointsMaterial&) = delete;
		PointsMaterial& operator=(const PointsMaterial&) = delete;

	private:
		float opacity_;
		math::float3 color_;
	};
}

#endif