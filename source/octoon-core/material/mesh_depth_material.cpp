#include <octoon/material/mesh_depth_material.h>

static const char* depth_vert = R"(
#include <common>
out vec3 vViewPosition;
void main() {
#include <begin_vertex>
#include <project_vertex>
vViewPosition = -mvPosition.xyz;
})";
static const char* depth_frag = R"(
#include <common>
#include <packing>
in vec3 vViewPosition;
void main() {
	fragColor = packDepthToRGBA( gl_FragCoord.z );
})";

namespace octoon
{
	OctoonImplementSubClass(MeshDepthMaterial, Material, "MeshDepthMaterial");

	MeshDepthMaterial::MeshDepthMaterial() noexcept
	{
		this->setShader(std::make_shared<Shader>(depth_vert, depth_frag));
	}

	MeshDepthMaterial::~MeshDepthMaterial() noexcept
	{
	}

	std::shared_ptr<Material>
	MeshDepthMaterial::clone() const noexcept
	{
		auto instance = std::make_shared<MeshDepthMaterial>();
		return instance;
	}
}