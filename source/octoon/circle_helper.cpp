#include <octoon/circle_helper.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/mesh/circle_mesh.h>

namespace octoon
{
	GameObjectPtr
	CircleHelper::create(float radius, std::uint32_t segments, float thetaStart, float thetaLength)
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(mesh::CircleMesh::create(radius, segments, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<material::Material>());
		return object;
	}
}