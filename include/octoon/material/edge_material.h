#ifndef OCTOON_EDGE_MATERIAL_H_
#define OCTOON_EDGE_MATERIAL_H_

#include <octoon/material/material.h>

namespace octoon
{
	class OCTOON_EXPORT EdgeMaterial final : public Material
	{
		OctoonDeclareSubClass(EdgeMaterial, Material);
	public:
		EdgeMaterial() noexcept;
		EdgeMaterial(const math::float3& color) noexcept;
		virtual ~EdgeMaterial() noexcept;

		void setColor(const math::float3& color) noexcept;
		const math::float3& getColor() const noexcept;

		void setOpacity(float opacity) noexcept;
		float getOpacity() const noexcept;

		void setColorMap(const std::shared_ptr<GraphicsTexture>& map) noexcept;
		const std::shared_ptr<GraphicsTexture>& getColorMap() const noexcept;

		std::shared_ptr<Material> clone() const noexcept override;

	private:
		EdgeMaterial(const EdgeMaterial&) = delete;
		EdgeMaterial& operator=(const EdgeMaterial&) = delete;

	private:
		float opacity_;
		math::float3 color_;
		std::shared_ptr<GraphicsTexture> edgeTexture_;
	};
}

#endif