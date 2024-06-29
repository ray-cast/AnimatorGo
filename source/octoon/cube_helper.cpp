#include <octoon/cube_helper.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/mesh/cube_mesh.h>

namespace octoon
{
	GameObjectPtr
	CubeHelper::create(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments)
	{
		auto object = std::make_shared<GameObject>(std::string_view("GameObject"));
		object->addComponent<MeshFilterComponent>(std::make_shared<CubeMesh>(width, height, depth, widthSegments, heightSegments, depthSegments));
		object->addComponent<MeshRendererComponent>(std::make_shared<Material>());
		return object;
	}
}