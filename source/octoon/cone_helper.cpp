#include <octoon/cone_helper.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>

namespace octoon
{
	GameObjectPtr
	ConeHelper::create(float radius, float height, std::uint32_t segments, float thetaStart, float thetaLength)
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeCone(radius, height, segments, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>());
		return object;
	}
}