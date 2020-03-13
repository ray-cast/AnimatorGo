#include <octoon/plane_helper.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>

namespace octoon
{
	GameObjectPtr
	PlaneHelper::create(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept(false)
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makePlane(width, height, widthSegments, heightSegments));
		object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>());
		return object;
	}

	GameObjectPtr
	PlaneHelper::create(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir) noexcept(false)
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makePlane(width, height, depth, widthSegments, heightSegments, depthSegments, u, v, udir, vdir));
		object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>());
		return object;
	}
}