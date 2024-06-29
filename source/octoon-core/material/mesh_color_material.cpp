#include <octoon/material/mesh_color_material.h>

static const char* lambert_vert = R"(
#include <common>
void main() {
#include <begin_vertex>
#include <project_vertex>
})";
static const char* lambert_frag = R"(
#include <common>
#include <packing>
uniform vec3 diffuse;
uniform float opacity;
void main() {
	fragColor = vec4(diffuse, opacity);
})";

namespace octoon
{
	OctoonImplementSubClass(MeshColorMaterial, Material, "MeshColorMaterial");

	MeshColorMaterial::MeshColorMaterial() noexcept
		: MeshColorMaterial(math::float3::One)
	{
	}

	MeshColorMaterial::MeshColorMaterial(const math::float3& color) noexcept
	{
		this->setColor(color);
		this->setOpacity(1.0f);
		this->setShader(std::make_shared<Shader>(lambert_vert, lambert_frag));
	}

	MeshColorMaterial::~MeshColorMaterial() noexcept
	{
	}

	void
	MeshColorMaterial::setColor(const math::float3& color) noexcept
	{
		this->color_ = color;
		this->set("diffuse", this->color_);
	}

	const math::float3&
	MeshColorMaterial::getColor() const noexcept
	{
		return this->color_;
	}

	void
	MeshColorMaterial::setColorMap(const std::shared_ptr<Texture>& map) noexcept
	{
		this->edgeTexture_ = map;
		this->set("decal", map);
		this->set("hasTexture", map ? true : false);
	}

	const std::shared_ptr<Texture>&
	MeshColorMaterial::getColorMap() const noexcept
	{
		return this->edgeTexture_;
	}

	void
	MeshColorMaterial::setOpacity(float opacity) noexcept
	{
		this->opacity_ = opacity;
		this->set("opacity", this->opacity_);
	}

	float
	MeshColorMaterial::getOpacity() const noexcept
	{
		return opacity_;
	}

	std::shared_ptr<Material>
	MeshColorMaterial::clone() const noexcept
	{
		auto instance = std::make_shared<MeshColorMaterial>();
		instance->setColor(this->getColor());
		instance->setColorMap(this->getColorMap());
		instance->setOpacity(this->getOpacity());
		return instance;
	}
}