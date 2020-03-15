#ifndef OCTOON_MESH_STANDARD_MATERIAL_H_
#define OCTOON_MESH_STANDARD_MATERIAL_H_

#include <octoon/material/material.h>

namespace octoon::material
{
	class OCTOON_EXPORT MeshStandardMaterial final : public Material
	{
		OctoonDeclareSubClass(MeshStandardMaterial, Material);
	public:
		MeshStandardMaterial() noexcept;
		MeshStandardMaterial(const math::float3& color) noexcept;
		virtual ~MeshStandardMaterial() noexcept;

		void setColor(const math::float3& color) noexcept;
		const math::float3& getColor() const noexcept;

		void setOpacity(float opacity) noexcept;
		float getOpacity() const noexcept;

		void setColorTexture(const hal::GraphicsTexturePtr& map) noexcept;
		const hal::GraphicsTexturePtr& getColorTexture() const noexcept;

		std::shared_ptr<Material> clone() const noexcept override;

	private:
		MeshStandardMaterial(const MeshStandardMaterial&) = delete;
		MeshStandardMaterial& operator=(const MeshStandardMaterial&) = delete;

	private:
		float opacity_;
		math::float3 color_;
		hal::GraphicsTexturePtr colorTexture_;
	};
}

#endif