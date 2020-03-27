#ifndef OCTOON_SHAPE_MESH_H_
#define OCTOON_SHAPE_MESH_H_

#include <octoon/mesh/mesh.h>
#include <octoon/model/contour.h>

namespace octoon::mesh
{
	class OCTOON_EXPORT ShapeMesh final : public Mesh
	{
		OctoonDeclareSubClass(ShapeMesh, Mesh)
	public:
		ShapeMesh() noexcept;
		ShapeMesh(const model::Contours& contours, float thickness) noexcept;
	};
}

#endif