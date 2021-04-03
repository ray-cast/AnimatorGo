#include <octoon/material/line_basic_material.h>

static const char* line_vert = R"(
#include <common>
#include <color_pars_vertex>
void main() {
#include <color_vertex>
#include <begin_vertex>
#include <project_vertex>
})";
static const char* line_frag = R"(
uniform vec3 diffuse;
uniform float opacity;
#include <common>
#include <packing>
#include <color_pars_fragment>
#include <encodings_pars_fragment>
#include <tonemapping_pars_fragment>
void main() {
	vec3 outgoingLight = vec3( 0.0 );
	vec4 diffuseColor = vec4( diffuse, opacity );
	#include <color_fragment>
	#include <alphatest_fragment>
	outgoingLight = diffuseColor.rgb;
	fragColor = vec4( outgoingLight, diffuseColor.a );
	#include <premultiplied_alpha_fragment>
	#include <tonemapping_fragment>
	fragColor = LinearToGamma(fragColor, 2.2);
})";

namespace octoon
{
	OctoonImplementSubClass(LineBasicMaterial, Material, "LineBasicMaterial");

	LineBasicMaterial::LineBasicMaterial() noexcept
		: LineBasicMaterial(math::float3::One)
	{
		this->setColor(math::float3::One);
	}

	LineBasicMaterial::LineBasicMaterial(const math::float3& color) noexcept
		: opacity_(1.0f)
	{

		this->setColor(color);
		this->setOpacity(1.0f);
		this->setDepthEnable(false);
		this->setPrimitiveType(octoon::hal::GraphicsVertexType::LineList);
		this->setShader(std::make_shared<Shader>(line_vert, line_frag));
	}

	LineBasicMaterial::~LineBasicMaterial() noexcept
	{
	}

	void
	LineBasicMaterial::setColor(const math::float3& color) noexcept
	{
		this->color_ = color;
		this->set("diffuse", color_);
	}

	const math::float3&
	LineBasicMaterial::getColor() const noexcept
	{
		return this->color_;
	}

	void
	LineBasicMaterial::setOpacity(float opacity) noexcept
	{
		this->opacity_ = opacity;
		this->set("opacity", opacity_);
	}

	float
	LineBasicMaterial::getOpacity() const noexcept
	{
		return opacity_;
	}

	std::shared_ptr<Material>
	LineBasicMaterial::clone() const noexcept
	{
		auto instance = std::make_shared<LineBasicMaterial>();
		instance->setColor(this->getColor());
		instance->setOpacity(this->getOpacity());

		return instance;
	}
}