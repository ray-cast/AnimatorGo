#include <octoon/material/edge_material.h>
#include <octoon/hal/graphics_texture.h>

static const char* edge_vert = R"(
#include <common>
out vec2 vUv;
void main() {
#include <begin_vertex>
vUv = TEXCOORD0;
gl_Position = vec4( transformed, 1.0 );
})";
static const char* edge_frag = R"(
#include <common>
#include <packing>
in vec2 vUv;
uniform vec3 diffuse;
uniform float opacity;
uniform sampler2D map;
uniform vec4 map_TexelSize;
void main() {
	vec3 weights = vec3(0.25f, 0.5f, 0.25f);

	float L0 = dot(texture2D(map, vUv).rgb, weights);

	float delta = 0.0f;
	delta += L0 - dot(texture2D(map, vUv + vec2(-map_TexelSize.x, 0.0f)).rgb, weights);
	delta += L0 - dot(texture2D(map, vUv + vec2(0.0f, map_TexelSize.y)).rgb, weights);
	delta += L0 - dot(texture2D(map, vUv + vec2(map_TexelSize.x, 0.0f)).rgb, weights);
	delta += L0 - dot(texture2D(map, vUv + vec2(0.0f, -map_TexelSize.y)).rgb, weights);
	delta += L0 - dot(texture2D(map, vUv + vec2(-map_TexelSize.x, 0.0f) * 2.0f).rgb, weights);
	delta += L0 - dot(texture2D(map, vUv + vec2(0.0f, map_TexelSize.y) * 2.0f).rgb, weights);
	delta += L0 - dot(texture2D(map, vUv + vec2(map_TexelSize.x, 0.0f) * 2.0f).rgb, weights);
	delta += L0 - dot(texture2D(map, vUv + vec2(0.0f, -map_TexelSize.y) * 2.0f).rgb, weights);
	delta += L0 - dot(texture2D(map, vUv + vec2(0.0f, -map_TexelSize.y) * 3.0f).rgb, weights);
	delta += L0 - dot(texture2D(map, vUv + vec2(-map_TexelSize.x, 0.0f) * 3.0f).rgb, weights);
	delta += L0 - dot(texture2D(map, vUv + vec2(0.0f, map_TexelSize.y) * 3.0f).rgb, weights);
	delta += L0 - dot(texture2D(map, vUv + vec2(map_TexelSize.x, 0.0f) * 3.0f).rgb, weights);
	delta /= 12;

	float edge = step(1e-5, delta);

	fragColor = vec4(texture2D(map, vUv).rgb * edge, edge);
})";

namespace octoon
{
	OctoonImplementSubClass(EdgeMaterial, Material, "EdgeMaterial");

	EdgeMaterial::EdgeMaterial() noexcept
		: EdgeMaterial(math::float3::One)
	{
	}

	EdgeMaterial::EdgeMaterial(const math::float3& color) noexcept
	{
		this->setColor(color);
		this->setOpacity(1.0f);
		this->setShader(std::make_shared<Shader>(edge_vert, edge_frag));
	}

	EdgeMaterial::~EdgeMaterial() noexcept
	{
	}

	void
	EdgeMaterial::setColor(const math::float3& color) noexcept
	{
		this->color_ = color;
		this->set("diffuse", this->color_);
	}

	const math::float3&
	EdgeMaterial::getColor() const noexcept
	{
		return this->color_;
	}

	void
	EdgeMaterial::setColorMap(const std::shared_ptr<GraphicsTexture>& map) noexcept
	{
		this->edgeTexture_ = map;
		this->set("map", map);
		this->set("hasTexture", map ? true : false);
		if (map)
		{
			auto width = map->getTextureDesc().getWidth();
			auto height = map->getTextureDesc().getHeight();
			this->set("map_TexelSize", math::float4(1.0f / width, 1.0f / height, 0.0f, 0.0f));
		}
	}

	const std::shared_ptr<GraphicsTexture>&
	EdgeMaterial::getColorMap() const noexcept
	{
		return this->edgeTexture_;
	}

	void
	EdgeMaterial::setOpacity(float opacity) noexcept
	{
		this->opacity_ = opacity;
		this->set("opacity", this->opacity_);
	}

	float
	EdgeMaterial::getOpacity() const noexcept
	{
		return opacity_;
	}

	std::shared_ptr<Material>
	EdgeMaterial::clone() const noexcept
	{
		auto instance = std::make_shared<EdgeMaterial>();
		instance->setColor(this->getColor());
		instance->setColorMap(this->getColorMap());
		instance->setOpacity(this->getOpacity());
		return instance;
	}
}