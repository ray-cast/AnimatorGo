#ifndef OCTOON_MESH_COLOR_MATERIAL_H_
#define OCTOON_MESH_COLOR_MATERIAL_H_

#include <octoon/material/material.h>

namespace octoon
{
	class OCTOON_EXPORT MeshColorMaterial final : public Material
	{
		OctoonDeclareSubClass(MeshColorMaterial, Material);
	public:
		MeshColorMaterial() noexcept;
		MeshColorMaterial(const math::float3& color) noexcept;
		virtual ~MeshColorMaterial() noexcept;

		void setColor(const math::float3& color) noexcept;
		const math::float3& getColor() const noexcept;

		void setOpacity(float opacity) noexcept;
		float getOpacity() const noexcept;

		void setColorMap(const std::shared_ptr<Texture>& map) noexcept;
		const std::shared_ptr<Texture>& getColorMap() const noexcept;

		std::shared_ptr<Material> clone() const noexcept override;

	private:
		MeshColorMaterial(const MeshColorMaterial&) = delete;
		MeshColorMaterial& operator=(const MeshColorMaterial&) = delete;

	private:
		float opacity_;
		math::float3 color_;
		std::shared_ptr<Texture> edgeTexture_;
	};
}

#endif