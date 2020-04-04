#ifndef OCTOON_MESH_DEPTH_MATERIAL_H_
#define OCTOON_MESH_DEPTH_MATERIAL_H_

#include <octoon/material/material.h>

namespace octoon::material
{
	class OCTOON_EXPORT MeshDepthMaterial final : public Material
	{
		OctoonDeclareSubClass(MeshDepthMaterial, Material);
	public:
		MeshDepthMaterial() noexcept;
		virtual ~MeshDepthMaterial() noexcept;

		std::shared_ptr<Material> clone() const noexcept override;

	private:
		MeshDepthMaterial(const MeshDepthMaterial&) = delete;
		MeshDepthMaterial& operator=(const MeshDepthMaterial&) = delete;
	};
}

#endif