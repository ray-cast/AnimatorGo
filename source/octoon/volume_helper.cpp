#include <octoon/volume_helper.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/mesh/volume_mesh.h>

namespace octoon
{
	GameObjectPtr
	VolumeHelper::create(float fovy, float znear, float zfar) noexcept(false)
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(VolumeMesh::create(fovy, znear, zfar));
		object->addComponent<MeshRendererComponent>(std::make_shared<Material>());
		return object;
	}
}