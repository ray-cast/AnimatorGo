#include <octoon/material/mesh_copy_material.h>

static const char* depth_vert = R"(
#include <common>
out vec2 vUv;
out vec3 vViewPosition;
void main() {
#include <begin_vertex>
vUv = TEXCOORD0;
gl_Position = vec4( transformed, 1.0 );
})";
static const char* depth_frag = R"(
#include <common>
#include <packing>
in vec2 vUv;
uniform sampler2D map;
void main() {
	fragColor = texture2D( map, vUv );
})";

namespace octoon
{
	OctoonImplementSubClass(MeshCopyMaterial, Material, "MeshCopyMaterial");

	MeshCopyMaterial::MeshCopyMaterial() noexcept
	{
		this->setShader(std::make_shared<Shader>(depth_vert, depth_frag));
	}

	MeshCopyMaterial::~MeshCopyMaterial() noexcept
	{
	}

	void
	MeshCopyMaterial::setColorMap(const std::shared_ptr<Texture>& map) noexcept
	{
		this->colorMap_ = map;
		this->set("map", map);
		this->set("mapEnable", map ? true : false);
	}

	const std::shared_ptr<Texture>&
	MeshCopyMaterial::getColorMap() const noexcept
	{
		return this->colorMap_;
	}

	std::shared_ptr<Material>
	MeshCopyMaterial::clone() const noexcept
	{
		auto instance = std::make_shared<MeshCopyMaterial>();
		return instance;
	}
}