#include <octoon/plane_helper.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/mesh/plane_mesh.h>

namespace octoon
{
	GameObjectPtr
	PlaneHelper::create(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept(false)
	{
		auto object = std::make_shared<GameObject>(std::string_view("GameObject"));
		object->addComponent<MeshFilterComponent>(std::make_shared<PlaneMesh>(width, height, widthSegments, heightSegments));
		object->addComponent<MeshRendererComponent>(std::make_shared<Material>());
		return object;
	}
}