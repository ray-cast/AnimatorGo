#include <octoon/material/mesh_standard_material.h>

static const char* standard_vert = R"(
#include <common>
#include <uv_pars_vertex>
#include <color_pars_vertex>
#include <normal_pars_vertex>
out vec3 vViewPosition;
void main() {
#include <uv_vertex>
#include <color_vertex>
#include <beginnormal_vertex>
#include <defaultnormal_vertex>
#include <begin_vertex>
#include <project_vertex>
vViewPosition = -mvPosition.xyz;
})";

static const char* standard_frag = R"(
uniform vec3 diffuse;
uniform vec3 emissive;
uniform float opacity;
uniform float roughness;
uniform float metalness;
uniform float clearCoat;
uniform float clearCoatRoughness;
uniform float reflectivity;
in vec3 vViewPosition;
#include <common>
#include <packing>
#include <encodings_pars_fragment>
#include <uv_pars_fragment>
#include <normal_pars_fragment>
#include <map_pars_fragment>
#include <color_pars_fragment>
#include <alphamap_pars_fragment>
#include <aomap_pars_fragment>
#include <lightmap_pars_fragment>
#include <emissivemap_pars_fragment>
#include <envmap_pars_fragment>
#include <shadowmap_pars_fragment>
#include <bumpmap_pars_fragment>
#include <roughnessmap_pars_fragment>
#include <metalnessmap_pars_fragment>
#include <bsdfs>
#include <lights_pars>
#include <lights_physical_pars_fragment>
void main() {
	vec4 diffuseColor = vec4( diffuse, opacity );
	ReflectedLight reflectedLight = ReflectedLight( vec3( 0.0 ), vec3( 0.0 ), vec3( 0.0 ), vec3( 0.0 ) );
	vec3 totalEmissiveRadiance = emissive;
	#include <normal_flip>
	#include <normal_fragment>
	#include <map_fragment>
	#include <color_fragment>
	#include <alphamap_fragment>
	#include <alphatest_fragment>
	#include <roughnessmap_fragment>
	#include <metalnessmap_fragment>
	#include <emissivemap_fragment>
	#include <lights_physical_fragment>
	#include <lights_template>
	vec3 outgoingLight = reflectedLight.directDiffuse + reflectedLight.indirectDiffuse + reflectedLight.directSpecular + reflectedLight.indirectSpecular + totalEmissiveRadiance;
	fragColor = vec4( outgoingLight, diffuseColor.a );
	#include <tonemapping_fragment>
	fragColor = LinearToGamma(fragColor, 2.2);
})";

namespace octoon::material
{
	OctoonImplementSubClass(MeshStandardMaterial, Material, "MeshStandardMaterial");

	MeshStandardMaterial::MeshStandardMaterial() noexcept
		: MeshStandardMaterial(math::float3::One)
	{
	}

	MeshStandardMaterial::MeshStandardMaterial(const math::float3& color) noexcept
	{
		this->setColor(color);
		this->setOpacity(1.0f);
		this->setOffset(math::float2::Zero);
		this->setRepeat(math::float2::One);
		this->setEmissive(math::float3::Zero);
		this->setSmoothness(0.0f);
		this->setMetalness(0.0f);
		this->setReflectivity(0.5f);
		this->setClearCoat(0.0f);
		this->setClearCoatRoughness(0.0f);
		this->setShader(std::make_shared<Shader>(standard_vert, standard_frag));
	}

	MeshStandardMaterial::~MeshStandardMaterial() noexcept
	{
	}

	void
	MeshStandardMaterial::setColor(const math::float3& color) noexcept
	{
		this->color_ = color;
		this->set("diffuse", this->color_);
	}

	const math::float3&
	MeshStandardMaterial::getColor() const noexcept
	{
		return this->color_;
	}

	void
	MeshStandardMaterial::setOpacity(float opacity) noexcept
	{
		this->opacity_ = opacity;
		this->set("opacity", this->opacity_);
	}

	float
	MeshStandardMaterial::getOpacity() const noexcept
	{
		return opacity_;
	}

	void
	MeshStandardMaterial::setEmissive(const math::float3& color) noexcept
	{
		this->emissive_ = color;
		this->set("emissive", this->emissive_);
	}

	const math::float3&
	MeshStandardMaterial::getEmissive()const noexcept
	{
		return this->emissive_;
	}

	void
	MeshStandardMaterial::setOffset(const math::float2& offset) noexcept
	{
		this->offset_ = offset;
		this->set("offsetRepeat", math::float4(this->offset_, this->repeat_));
	}

	const math::float2&
	MeshStandardMaterial::getOffset() const noexcept
	{
		return this->offset_;
	}

	void
	MeshStandardMaterial::setRepeat(const math::float2& repeat) noexcept
	{
		this->repeat_ = repeat;
		this->set("offsetRepeat", math::float4(this->offset_, this->repeat_));
	}

	const math::float2&
	MeshStandardMaterial::getRepeat() const noexcept
	{
		return this->repeat_;
	}

	void
	MeshStandardMaterial::setColorTexture(const hal::GraphicsTexturePtr& map) noexcept
	{
		this->colorTexture_ = map;
		this->set("map", map);
		this->set("mapEnable", map ? true : false);
	}

	const hal::GraphicsTexturePtr&
	MeshStandardMaterial::getColorTexture() const noexcept
	{
		return this->colorTexture_;
	}

	void
	MeshStandardMaterial::setSmoothness(float smoothness) noexcept
	{
		this->smoothness_ = smoothness;
		this->set("roughness", (1- smoothness) * (1 - smoothness));
	}

	float
	MeshStandardMaterial::getSmoothness() const noexcept
	{
		return this->smoothness_;
	}

	void
	MeshStandardMaterial::setMetalness(float metalness) noexcept
	{
		this->metalness_ = metalness;
		this->set("metalness", metalness);
	}

	float
	MeshStandardMaterial::getMetalness() const noexcept
	{
		return this->metalness_;
	}

	void
	MeshStandardMaterial::setReflectivity(float reflectivity) noexcept
	{
		this->reflectivity_ = reflectivity;
		this->set("reflectivity", reflectivity);
	}

	float
	MeshStandardMaterial::getReflectivity() const noexcept
	{
		return this->reflectivity_;
	}

	void
	MeshStandardMaterial::setClearCoat(float clearCoat) noexcept
	{
		this->clearCoat_ = clearCoat;
		this->set("clearCoat", clearCoat);
	}

	float
	MeshStandardMaterial::getClearCoat() const noexcept
	{
		return this->clearCoat_;
	}

	void
	MeshStandardMaterial::setClearCoatRoughness(float clearCoatRoughness) noexcept
	{
		this->clearCoatRoughness_ = clearCoatRoughness;
		this->set("clearCoatRoughness", clearCoatRoughness);
	}

	float
	MeshStandardMaterial::getClearCoatRoughness() const noexcept
	{
		return this->clearCoatRoughness_;
	}

	std::shared_ptr<Material>
	MeshStandardMaterial::clone() const noexcept
	{
		auto instance = std::make_shared<MeshStandardMaterial>();
		instance->setColor(this->getColor());

		return instance;
	}
}