#include <octoon/material/mesh_basic_material.h>

static const char* basic_vert = R"(
#include <common>
#include <uv_pars_vertex>
#include <color_pars_vertex>
void main() {
#include <uv_vertex>
#include <color_vertex>
#include <begin_vertex>
#include <project_vertex>
})";

static const char* basic_frag = R"(
#include <common>
#include <packing>
#include <encodings_pars_fragment>
#include <uv_pars_fragment>
#include <normal_pars_vertex>
#include <map_pars_fragment>
#include <color_pars_fragment>
#include <alphamap_pars_fragment>
#include <aomap_pars_fragment>
#include <lightmap_pars_fragment>
#include <envmap_pars_fragment>
#include <tonemapping_pars_fragment>
uniform vec3 diffuse;
uniform float opacity;
uniform float gamma;
void main() {
	vec3 outgoingLight = vec3( 0.0 );
	vec4 diffuseColor = vec4( diffuse, opacity );
	#include <map_fragment>
	#include <color_fragment>
	#include <alphamap_fragment>
	#include <alphatest_fragment>
	outgoingLight = diffuseColor.rgb;
	fragColor = vec4( outgoingLight, diffuseColor.a );
	#include <premultiplied_alpha_fragment>
	#include <tonemapping_fragment>
	fragColor = LinearToGamma(fragColor, 2.2);
})";

namespace octoon::material
{
	OctoonImplementSubClass(MeshBasicMaterial, Material, "MeshBasicMaterial");

	MeshBasicMaterial::MeshBasicMaterial() noexcept
		: MeshBasicMaterial(math::float3::One)
	{
	}

	MeshBasicMaterial::MeshBasicMaterial(const math::float3& color) noexcept
		: opacity_(1.0f)
	{
		this->setColor(color);
		this->setOpacity(1.0f);
		this->setOffset(math::float2::Zero);
		this->setRepeat(math::float2::One);
		this->setGamma(2.2f);
		this->setShader(std::make_shared<Shader>(basic_vert, basic_frag));
	}

	MeshBasicMaterial::~MeshBasicMaterial() noexcept
	{
	}

	void
	MeshBasicMaterial::setColor(const math::float3& color) noexcept
	{
		this->color_ = color;
		this->set("diffuse", this->color_);
	}

	const math::float3&
	MeshBasicMaterial::getColor() const noexcept
	{
		return this->color_;
	}

	void
	MeshBasicMaterial::setOffset(const math::float2& offset) noexcept
	{
		this->offset_ = offset;
		this->set("offsetRepeat", math::float4(this->offset_, this->repeat_));
	}

	const math::float2&
	MeshBasicMaterial::getOffset() const noexcept
	{
		return this->offset_;
	}

	void
	MeshBasicMaterial::setRepeat(const math::float2& repeat) noexcept
	{
		this->repeat_ = repeat;
		this->set("offsetRepeat", math::float4(this->offset_, this->repeat_));
	}

	const math::float2&
	MeshBasicMaterial::getRepeat() const noexcept
	{
		return this->repeat_;
	}

	void
	MeshBasicMaterial::setColorTexture(const hal::GraphicsTexturePtr& map) noexcept
	{
		this->colorTexture_ = map;
		this->set("map", map);
		this->set("mapEnable", map ? true : false);
	}

	const hal::GraphicsTexturePtr&
	MeshBasicMaterial::getColorTexture() const noexcept
	{
		return this->colorTexture_;
	}

	void
	MeshBasicMaterial::setGamma(float gamma) noexcept
	{
		this->gamma_ = gamma;
		this->set("gamma", gamma);
	}

	float
	MeshBasicMaterial::getGamma() const noexcept
	{
		return this->gamma_;
	}

	void
	MeshBasicMaterial::setOpacity(float opacity) noexcept
	{
		this->opacity_ = opacity;
		this->set("opacity", this->opacity_);
	}

	float
	MeshBasicMaterial::getOpacity() const noexcept
	{
		return opacity_;
	}

	std::shared_ptr<Material>
	MeshBasicMaterial::clone() const noexcept
	{
		auto instance = std::make_shared<MeshBasicMaterial>();
		instance->setColor(this->getColor());
		instance->setColorTexture(this->getColorTexture());
		instance->setOpacity(this->getOpacity());
		instance->setGamma(this->getGamma());
		instance->setRepeat(this->getRepeat());
		instance->setOffset(this->getOffset());

		return instance;
	}
}