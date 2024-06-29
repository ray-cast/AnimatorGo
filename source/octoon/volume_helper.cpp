#include <octoon/volume_helper.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/mesh/volume_mesh.h>

namespace octoon
{
	GameObjectPtr
	VolumeHelper::create(float fovy, float znear, float zfar) noexcept(false)
	{
		auto object = std::make_shared<GameObject>(std::string_view("GameObject"));
		object->addComponent<MeshFilterComponent>(std::make_shared<VolumeMesh>(fovy, znear, zfar));
		object->addComponent<MeshRendererComponent>(std::make_shared<Material>());
		return object;
	}
}