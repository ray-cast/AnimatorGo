#include <octoon/material/mesh_standard_material.h>
#include <octoon/hal/graphics_texture.h>

static const char* standard_vert = R"(
#include <common>
#include <uv_pars_vertex>
#include <uv2_pars_vertex>
#include <color_pars_vertex>
#include <normal_pars_vertex>
#include <shadowmap_pars_vertex>
out vec3 vViewPosition;
void main() {
#include <uv_vertex>
#include <uv2_vertex>
#include <color_vertex>
#include <beginnormal_vertex>
#include <defaultnormal_vertex>
#include <begin_vertex>
#include <worldpos_vertex>
#include <shadowmap_vertex>
#include <project_vertex>
vViewPosition = -mvPosition.xyz;
})";

static const char* standard_frag = R"(
#include <common>
#include <packing>
#include <encodings_pars_fragment>
#include <uv_pars_fragment>
#include <uv2_pars_fragment>
#include <normal_pars_fragment>
#include <map_pars_fragment>
#include <color_pars_fragment>
#include <normalmap_pars_fragment>
#include <opacitymap_pars_fragment>
#include <aomap_pars_fragment>
#include <lightmap_pars_fragment>
#include <emissivemap_pars_fragment>
#include <envmap_pars_fragment>
#include <shadowmap_pars_fragment>
#include <bumpmap_pars_fragment>
#include <roughnessmap_pars_fragment>
#include <specularmap_pars_fragment>
#include <metalnessmap_pars_fragment>
#include <anisotropymap_pars_fragment>
#include <sheenmap_pars_fragment>
#include <subsurfacemap_pars_fragment>
#include <clearcoatmap_pars_fragment>
#include <clearcoatRoughnessmap_pars_fragment>
#include <cube_uv_reflection_fragment>
#include <bsdfs>
#include <lights_pars>
#include <lights_physical_pars_fragment>
#include <tonemapping_pars_fragment>

uniform vec3 diffuse;
uniform vec3 emissive;
uniform vec3 subsurfaceColor;
uniform float opacity;
uniform float roughness;
uniform float metalness;
uniform float clearCoat;
uniform float clearCoatRoughness;
uniform float specular;
uniform float anisotropy;
uniform float sheen;
uniform float subsurface;
uniform float gamma;

in vec3 vViewPosition;

void main() {
	vec4 diffuseColor = vec4( diffuse, opacity );
	ReflectedLight reflectedLight = ReflectedLight( vec3( 0.0 ), vec3( 0.0 ), vec3( 0.0 ), vec3( 0.0 ) );
	vec3 totalEmissiveRadiance = emissive;
	#include <normal_flip>
	#include <normal_fragment>
	#include <map_fragment>
	#include <color_fragment>
	#include <opacitymap_fragment>
	#include <alphatest_fragment>
	#include <roughnessmap_fragment>
	#include <specularmap_fragment>
	#include <metalnessmap_fragment>
	#include <anisotropymap_fragment>
	#include <sheenmap_fragment>
	#include <clearcoatmap_fragment>
	#include <clearcoatRoughnessmap_fragment>
	#include <subsurfacemap_fragment>
	#include <emissivemap_fragment>
	#include <lights_physical_fragment>
	#include <lights_template>
	vec3 outgoingLight = vec3(0);
	if (dot(totalEmissiveRadiance, totalEmissiveRadiance) > 0.0)
		outgoingLight = totalEmissiveRadiance * (1 + pow(dot(normal, geometry.viewDir), 2));
	else
		outgoingLight = reflectedLight.directDiffuse + reflectedLight.indirectDiffuse + reflectedLight.directSpecular + reflectedLight.indirectSpecular;
	fragColor = vec4( outgoingLight, diffuseColor.a );
	#include <tonemapping_fragment>
	fragColor = LinearToGamma(fragColor, 2.2);
})";

namespace octoon
{
	OctoonImplementSubClass(MeshStandardMaterial, Material, "MeshStandardMaterial");

	MeshStandardMaterial::MeshStandardMaterial() noexcept
		: lightMapIntensity_(1.0f)
		, emissiveIntensity_(1.0f)
	{
		this->setColor(math::float3::One);
		this->setOpacity(1.0f);
		this->setEmissive(math::float3::Zero);
		this->setRoughness(1.0f);
		this->setAnisotropy(0.0f);
		this->setSheen(0.0f);
		this->setMetalness(0.0f);
		this->setSpecular(0.5f);
		this->setReflectionRatio(1.0f);
		this->setRefractionRatio(1.5f);
		this->setClearCoat(0.0f);
		this->setClearCoatRoughness(0.0f);
		this->setSubsurface(0.0f);
		this->setSubsurfaceColor(math::float3(1.0f, 0.2f, 0.05f) * 0.5f);
		this->setTransmission(0.0f);
		this->setGamma(2.2f);
		this->setOffset(math::float2::Zero);
		this->setRepeat(math::float2::One);
		this->setNormalScale(math::float2::One);
		this->setReceiveShadow(true);
		this->setShader(std::make_shared<Shader>(standard_vert, standard_frag));
	}

	MeshStandardMaterial::MeshStandardMaterial(std::string_view name) noexcept
		: MeshStandardMaterial()
	{
		this->setName(name);
	}

	MeshStandardMaterial::MeshStandardMaterial(std::string_view name, const math::float3& color) noexcept
		: MeshStandardMaterial()
	{
		this->setName(name);
		this->setColor(color);
	}

	MeshStandardMaterial::MeshStandardMaterial(const math::float3& color) noexcept
		: MeshStandardMaterial()
	{
		this->setColor(color);
	}

	MeshStandardMaterial::~MeshStandardMaterial() noexcept
	{
	}

	void
	MeshStandardMaterial::setColor(const math::float3& color) noexcept
	{
		this->color_ = color;
		this->setDirty(false);
		this->set("diffuse", this->color_);
	}

	void
	MeshStandardMaterial::setOpacity(float opacity) noexcept
	{
		this->opacity_ = opacity;
		this->setDirty(false);
		this->set("opacity", this->opacity_);
	}

	void
	MeshStandardMaterial::setEmissive(const math::float3& color) noexcept
	{
		this->emissive_ = color;
		this->setDirty(false);
		this->set("emissive", this->emissive_* this->emissiveIntensity_);
	}

	void
	MeshStandardMaterial::setEmissiveIntensity(float intensity) noexcept
	{
		this->emissiveIntensity_ = intensity;
		this->setDirty(false);
		this->set("emissive", this->emissive_ * this->emissiveIntensity_);
	}

	void
	MeshStandardMaterial::setAnisotropy(float anisotropy) noexcept
	{
		this->anisotropy_ = anisotropy;
		this->setDirty(false);
		this->set("anisotropy", anisotropy);
	}

	void
	MeshStandardMaterial::setSpecular(float specular) noexcept
	{
		this->specular_ = specular;
		this->setDirty(false);
		this->set("specular", specular);
	}

	void
	MeshStandardMaterial::setSheen(float sheen) noexcept
	{
		this->sheen_ = sheen;
		this->setDirty(false);
		this->set("sheen", sheen);
	}

	void
	MeshStandardMaterial::setRefractionRatio(float refractionRatio) noexcept
	{
		this->refractionRatio_ = refractionRatio;
		this->setDirty(false);
		this->set("refractionRatio", refractionRatio);
	}

	void
	MeshStandardMaterial::setClearCoat(float clearCoat) noexcept
	{
		this->clearCoat_ = clearCoat;
		this->setDirty(false);
		this->set("clearCoat", clearCoat);
	}

	void
	MeshStandardMaterial::setClearCoatRoughness(float clearCoatRoughness) noexcept
	{
		this->clearCoatRoughness_ = clearCoatRoughness;
		this->setDirty(false);
		this->set("clearCoatRoughness", clearCoatRoughness);
	}

	void
	MeshStandardMaterial::setSubsurface(float subsurface) noexcept
	{
		this->subsurface_ = subsurface;
		this->setDirty(false);
		this->set("subsurface", subsurface);
	}

	void
	MeshStandardMaterial::setSubsurfaceColor(const math::float3& subsurfaceColor) noexcept
	{
		this->subsurfaceColor_ = subsurfaceColor;
		this->setDirty(false);
		this->set("subsurfaceColor", subsurfaceColor);
	}

	void
	MeshStandardMaterial::setReflectionRatio(float ior) noexcept
	{
		this->reflectionRatio_ = ior;
		this->setDirty(false);
		this->set("ior", ior);
	}

	void
	MeshStandardMaterial::setTransmission(float transmission) noexcept
	{
		this->transmission_ = transmission;
		this->setDirty(false);
		this->set("transmission", transmission);
	}

	void
	MeshStandardMaterial::setGamma(float gamma) noexcept
	{
		this->gamma_ = gamma;
		this->setDirty(false);
		this->set("gamma", gamma);
	}

	void
	MeshStandardMaterial::setOffset(const math::float2& offset) noexcept
	{
		this->offset_ = offset;
		this->setDirty(false);
		this->set("offsetRepeat", math::float4(this->offset_, this->repeat_));
	}

	void
	MeshStandardMaterial::setRepeat(const math::float2& repeat) noexcept
	{
		this->repeat_ = repeat;
		this->setDirty(false);
		this->set("offsetRepeat", math::float4(this->offset_, this->repeat_));
	}

	void
	MeshStandardMaterial::setNormalScale(const math::float2& normalScale) noexcept
	{
		this->normalScale_ = normalScale;
		this->setDirty(false);
		this->set("normalScale", normalScale);
	}

	void
	MeshStandardMaterial::setLightMapIntensity(float intensity) noexcept
	{
		this->lightMapIntensity_ = intensity;
		this->setDirty(false);
		this->set("lightMapIntensity", intensity);
	}

	void
	MeshStandardMaterial::setSmoothness(float smoothness) noexcept
	{
		this->roughness_ = 1 - smoothness;
		this->setDirty(false);
		this->set("roughness", this->roughness_);
	}

	void
	MeshStandardMaterial::setRoughness(float roughness) noexcept
	{
		this->roughness_ = roughness;
		this->setDirty(false);
		this->set("roughness", roughness);
	}

	void
	MeshStandardMaterial::setColorMap(const std::shared_ptr<Texture>& map) noexcept
	{
		this->colorMap_ = map;
		this->set("colorMap", map);
		this->set("colorMapEnable", map ? true : false);
	}

	void
	MeshStandardMaterial::setOpacityMap(const std::shared_ptr<Texture>& map) noexcept
	{
		this->alphaMap_ = map;
		this->set("alphaMap", map);
		this->set("alphaMapEnable", map ? true : false);
	}

	void
	MeshStandardMaterial::setSubsurfaceMap(const std::shared_ptr<Texture>& map) noexcept
	{
		this->subsurfaceMap_ = map;
		this->set("subsurfaceMap", map);
		this->set("subsurfaceMapEnable", map ? true : false);
	}

	void
	MeshStandardMaterial::setSubsurfaceColorMap(const std::shared_ptr<Texture>& map) noexcept
	{
		this->subsurfaceMap_ = map;
		this->set("subsurfaceColorMap", map);
		this->set("subsurfaceColorMapEnable", map ? true : false);
	}

	void
	MeshStandardMaterial::setEmissiveMap(const std::shared_ptr<Texture>& map) noexcept
	{
		this->emissiveMap_ = map;
		this->set("emissiveMap", map);
		this->set("emissiveMapEnable", map ? true : false);
	}

	void
	MeshStandardMaterial::setNormalMap(const std::shared_ptr<Texture>& map) noexcept
	{
		this->normalMap_ = map;
		this->set("normalMap", map);
		this->set("normalMapEnable", map ? true : false);
	}

	void
	MeshStandardMaterial::setLightMap(const std::shared_ptr<Texture>& map) noexcept
	{
		this->lightMap_ = map;
		this->set("lightMap", map);
		this->set("lightMapIntensity", this->lightMapIntensity_);
		this->set("lightMapEnable", map ? true : false);
	}

	void
	MeshStandardMaterial::setSpecularMap(const std::shared_ptr<Texture>& map) noexcept
	{
		this->specularMap_ = map;
		this->set("specularMap", map);
		this->set("specularMapEnable", map ? true : false);
	}

	void
	MeshStandardMaterial::setRoughnessMap(const std::shared_ptr<Texture>& map) noexcept
	{
		this->roughnessMap_ = map;
		this->set("roughnessMap", map);
		this->set("roughnessMapEnable", map ? true : false);
	}

	void
	MeshStandardMaterial::setMetalness(float metalness) noexcept
	{
		this->metalness_ = metalness;
		this->setDirty(false);
		this->set("metalness", metalness);
	}

	void
	MeshStandardMaterial::setMetalnessMap(const std::shared_ptr<Texture>& map) noexcept
	{
		this->metalnessMap_ = map;
		this->set("metalnessMap", map);
		this->set("metalnessMapEnable", map ? true : false);
	}

	void
	MeshStandardMaterial::setAnisotropyMap(const std::shared_ptr<Texture>& map) noexcept
	{
		this->anisotropyMap_ = map;
		this->set("anisotropyMap", map);
		this->set("anisotropyMapEnable", map ? true : false);
	}

	void
	MeshStandardMaterial::setSheenMap(const std::shared_ptr<Texture>& map) noexcept
	{
		this->sheenMap_ = map;
		this->set("sheenMap", map);
		this->set("sheenMapEnable", map ? true : false);
	}

	void
	MeshStandardMaterial::setClearCoatMap(const std::shared_ptr<Texture>& map) noexcept
	{
		this->clearcoatMap_ = map;
		this->set("clearCoatMap", map);
		this->set("clearCoatMapEnable", map ? true : false);
	}

	void
	MeshStandardMaterial::setClearCoatRoughnessMap(const std::shared_ptr<Texture>& map) noexcept
	{
		this->clearcoatRoughnessMap_ = map;
		this->set("clearCoatRoughnessMap", map);
		this->set("clearCoatRoughnessMapEnable", map ? true : false);
	}

	const math::float3&
	MeshStandardMaterial::getColor() const noexcept
	{
		return this->color_;
	}

	const math::float3&
	MeshStandardMaterial::getSubsurfaceColor()const noexcept
	{
		return this->subsurfaceColor_;
	}

	const math::float3&
	MeshStandardMaterial::getEmissive()const noexcept
	{
		return this->emissive_;
	}

	float
	MeshStandardMaterial::getOpacity() const noexcept
	{
		return opacity_;
	}

	float
	MeshStandardMaterial::getEmissiveIntensity() const noexcept
	{
		return this->emissiveIntensity_;
	}

	float
	MeshStandardMaterial::getSmoothness() const noexcept
	{
		return 1 - this->roughness_;
	}

	float
	MeshStandardMaterial::getRoughness() const noexcept
	{
		return this->roughness_;
	}

	float
	MeshStandardMaterial::getMetalness() const noexcept
	{
		return this->metalness_;
	}

	float
	MeshStandardMaterial::getReflectionRatio() const noexcept
	{
		return this->reflectionRatio_;
	}

	float
	MeshStandardMaterial::getTransmission() const noexcept
	{
		return this->transmission_;
	}	

	float
	MeshStandardMaterial::getLightMapIntensity() const noexcept
	{
		return this->lightMapIntensity_;
	}


	const math::float2&
	MeshStandardMaterial::getOffset() const noexcept
	{
		return this->offset_;
	}

	const math::float2&
	MeshStandardMaterial::getRepeat() const noexcept
	{
		return this->repeat_;
	}

	const math::float2&
	MeshStandardMaterial::getNormalScale() const noexcept
	{
		return this->normalScale_;
	}

	const std::shared_ptr<Texture>&
	MeshStandardMaterial::getColorMap() const noexcept
	{
		return this->colorMap_;
	}

	const std::shared_ptr<Texture>&
	MeshStandardMaterial::getOpacityMap() const noexcept
	{
		return this->alphaMap_;
	}

	const std::shared_ptr<Texture>&
	MeshStandardMaterial::getSpecularMap() const noexcept
	{
		return this->specularMap_;
	}

	const std::shared_ptr<Texture>&
	MeshStandardMaterial::getRoughnessMap() const noexcept
	{
		return this->roughnessMap_;
	}

	const std::shared_ptr<Texture>&
	MeshStandardMaterial::getNormalMap() const noexcept
	{
		return this->normalMap_;
	}

	const std::shared_ptr<Texture>&
	MeshStandardMaterial::getMetalnessMap() const noexcept
	{
		return this->metalnessMap_;
	}

	const std::shared_ptr<Texture>&
	MeshStandardMaterial::getAnisotropyMap() const noexcept
	{
		return this->anisotropyMap_;
	}

	const std::shared_ptr<Texture>&
	MeshStandardMaterial::getSheenMap() const noexcept
	{
		return this->sheenMap_;
	}

	const std::shared_ptr<Texture>&
	MeshStandardMaterial::getClearCoatMap() const noexcept
	{
		return this->clearcoatMap_;
	}

	const std::shared_ptr<Texture>&
	MeshStandardMaterial::getClearCoatRoughnessMap() const noexcept
	{
		return this->clearcoatRoughnessMap_;
	}

	const std::shared_ptr<Texture>&
	MeshStandardMaterial::getLightMap() const noexcept
	{
		return this->lightMap_;
	}

	const std::shared_ptr<Texture>&
	MeshStandardMaterial::getSubsurfaceMap() const noexcept
	{
		return this->subsurfaceMap_;
	}

	const std::shared_ptr<Texture>&
	MeshStandardMaterial::getSubsurfaceColorMap() const noexcept
	{
		return this->subsurfaceColorMap_;
	}

	const std::shared_ptr<Texture>&
	MeshStandardMaterial::getEmissiveMap() const noexcept
	{
		return this->emissiveMap_;
	}

	float
	MeshStandardMaterial::getAnisotropy() const noexcept
	{
		return this->anisotropy_;
	}

	float
	MeshStandardMaterial::getSheen() const noexcept
	{
		return this->sheen_;
	}

	float
	MeshStandardMaterial::getSpecular() const noexcept
	{
		return this->specular_;
	}

	float
	MeshStandardMaterial::getRefractionRatio() const noexcept
	{
		return this->refractionRatio_;
	}

	float
	MeshStandardMaterial::getClearCoat() const noexcept
	{
		return this->clearCoat_;
	}

	float
	MeshStandardMaterial::getClearCoatRoughness() const noexcept
	{
		return this->clearCoatRoughness_;
	}

	float
	MeshStandardMaterial::getSubsurface() const noexcept
	{
		return this->subsurface_;
	}

	float
	MeshStandardMaterial::getGamma() const noexcept
	{
		return this->gamma_;
	}

	void
	MeshStandardMaterial::setReceiveShadow(bool enable) noexcept
	{
		this->setDirty(true);
		this->receiveShadow_ = enable;
	}

	bool
	MeshStandardMaterial::getReceiveShadow() const noexcept
	{
		return this->receiveShadow_;
	}

	void
	MeshStandardMaterial::copy(const MeshStandardMaterial& material)
	{
		Material::copy(material);

		this->setColorMap(material.getColorMap());
		this->setOpacityMap(material.getOpacityMap());
		this->setNormalMap(material.getNormalMap());
		this->setSpecularMap(material.getSpecularMap());
		this->setRoughnessMap(material.getRoughnessMap());
		this->setMetalnessMap(material.getMetalnessMap());
		this->setAnisotropyMap(material.getAnisotropyMap());
		this->setSheenMap(material.getSheenMap());
		this->setClearCoatMap(material.getClearCoatMap());
		this->setClearCoatRoughnessMap(material.getClearCoatRoughnessMap());
		this->setEmissiveMap(material.getEmissiveMap());
		this->setSubsurfaceMap(material.getSubsurfaceMap());
		this->setSubsurfaceColorMap(material.getSubsurfaceColorMap());
		this->setLightMap(material.getLightMap());

		this->setReceiveShadow(material.getReceiveShadow());

		this->setEmissiveIntensity(material.getEmissiveIntensity());
		this->setOpacity(material.getOpacity());
		this->setSmoothness(material.getSmoothness());
		this->setRoughness(material.getRoughness());
		this->setMetalness(material.getMetalness());
		this->setAnisotropy(material.getAnisotropy());
		this->setSheen(material.getSheen());
		this->setSpecular(material.getSpecular());
		this->setRefractionRatio(material.getRefractionRatio());
		this->setClearCoat(material.getClearCoat());
		this->setClearCoatRoughness(material.getClearCoatRoughness());
		this->setSubsurface(material.getSubsurface());
		this->setReflectionRatio(material.getReflectionRatio());
		this->setTransmission(material.getTransmission());
		this->setLightMapIntensity(material.getLightMapIntensity());
		this->setGamma(material.getGamma());

		this->setOffset(material.getOffset());
		this->setRepeat(material.getRepeat());
		this->setNormalScale(material.getNormalScale());

		this->setColor(material.getColor());
		this->setEmissive(material.getEmissive());
		this->setSubsurfaceColor(material.getSubsurfaceColor());
	}

	std::shared_ptr<Material>
	MeshStandardMaterial::clone() const noexcept
	{
		auto instance = std::make_shared<MeshStandardMaterial>();
		instance->copy(*this);

		return instance;
	}

	void
	MeshStandardMaterial::save(const std::filesystem::path& path)
	{
	}
}