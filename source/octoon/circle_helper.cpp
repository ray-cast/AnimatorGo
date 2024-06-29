#include <octoon/circle_helper.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/mesh/circle_mesh.h>

namespace octoon
{
	GameObjectPtr
	CircleHelper::create(float radius, std::uint32_t segments, float thetaStart, float thetaLength)
	{
		auto object = std::make_shared<GameObject>(std::string_view("GameObject"));
		object->addComponent<MeshFilterComponent>(std::make_shared<CircleMesh>(radius, segments, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<Material>());
		return object;
	}
}