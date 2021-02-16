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
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(mesh::SphereMesh::create(radius, widthSegments, heightSegments, phiStart, phiLength, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<material::MeshBasicMaterial>());
		return object;
	}
}