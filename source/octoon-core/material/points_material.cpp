#include <octoon/material/points_material.h>

namespace octoon
{
static const char* point_vert = R"(
#include <common>
#include <color_pars_vertex>
void main() {
#include <color_vertex>
#include <begin_vertex>
#include <project_vertex>
})";
static const char* point_frag = R"(
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

	OctoonImplementSubClass(PointsMaterial, Material, "PointsMaterial");

	PointsMaterial::PointsMaterial() noexcept
		: PointsMaterial(math::float3::One)
	{
	}

	PointsMaterial::PointsMaterial(const math::float3& color) noexcept
		: opacity_(1.0f)
	{
		this->setColor(color);
		this->setOpacity(1.0f);
		this->setDepthEnable(false);
		this->setPrimitiveType(octoon::VertexType::PointList);
		this->setShader(std::make_shared<Shader>(point_vert, point_frag));
	}

	PointsMaterial::~PointsMaterial() noexcept
	{
	}

	void
	PointsMaterial::setColor(const math::float3& color) noexcept
	{
		this->set("color", math::float4(color, opacity_));
		this->color_ = color;
	}

	const math::float3&
	PointsMaterial::getColor() const noexcept
	{
		return this->color_;
	}

	void
	PointsMaterial::setOpacity(float opacity) noexcept
	{
		this->set("color", math::float4(color_, opacity));
		this->opacity_ = opacity;
	}

	float
	PointsMaterial::getOpacity() const noexcept
	{
		return opacity_;
	}

	std::shared_ptr<Material>
	PointsMaterial::clone() const noexcept
	{
		auto instance = std::make_shared<PointsMaterial>();
		instance->setColor(this->getColor());
		instance->setOpacity(this->getOpacity());

		return instance;
	}
}