#include <octoon/sphere_helper.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/mesh/sphere_mesh.h>
#include <octoon/material/mesh_basic_material.h>

namespace octoon
{
	GameObjectPtr
	SphereHelper::create(float radius, std::uint32_t widthSegments, std::uint32_t heightSegments, float phiStart, float phiLength, float thetaStart, float thetaLength) noexcept(false)
	{
		auto object = std::make_shared<GameObject>(std::string_view("GameObject"));
		object->addComponent<MeshFilterComponent>(std::make_shared<SphereMesh>(radius, widthSegments, heightSegments, phiStart, phiLength, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<MeshBasicMaterial>());
		return object;
	}
}