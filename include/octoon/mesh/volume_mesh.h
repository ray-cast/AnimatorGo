#ifndef OCTOON_VOLUME_MESH_H_
#define OCTOON_VOLUME_MESH_H_

#include <octoon/mesh/mesh.h>

namespace octoon
{
	class OCTOON_EXPORT VolumeMesh final : public Mesh
	{
		OctoonDeclareSubClass(VolumeMesh, Mesh)
	public:
		VolumeMesh() noexcept;
		VolumeMesh(float fovy, float znear, float zfar) noexcept;
	};
}

#endif