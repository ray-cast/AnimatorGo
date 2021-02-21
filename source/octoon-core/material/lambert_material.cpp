#include <octoon/material/lambert_material.h>

namespace octoon
{
	OctoonImplementSubClass(LambertMaterial, Material, "LambertMaterial");

	LambertMaterial::LambertMaterial() noexcept
		: LambertMaterial(math::float3::One)
	{
	}

	LambertMaterial::LambertMaterial(const math::float3& color) noexcept
	{
	const char* vert = R"(#version 330
		uniform mat4 proj;
		uniform mat4 model;

		layout(location  = 0) in vec4 POSITION0;
		layout(location  = 1) in vec2 TEXCOORD0;

		out vec2 oTexcoord0;

		void main()
		{
			oTexcoord0 = TEXCOORD0;
			gl_Position = proj * model * (POSITION0 * vec4(1,1,1,1));
		})";

	const char* frag = R"(#version 330
		layout(location  = 0) out vec4 fragColor;

		uniform sampler2D decal;
		uniform vec4 color;
		uniform bool hasTexture;

		in vec2 oTexcoord0;

		void main()
		{
			fragColor = color;
			if (hasTexture) fragColor *= pow(texture(decal, oTexcoord0), vec4(2.2));
			fragColor = pow(fragColor, vec4(1.0 / 2.2));
		})";

		this->setColor(color);
		this->setOpacity(1.0f);
		this->setShader(std::make_shared<Shader>(vert, frag));
	}

	LambertMaterial::~LambertMaterial() noexcept
	{
	}

	void
	LambertMaterial::setColor(const math::float3& color) noexcept
	{
		this->color_ = color;
		this->set("color", math::float4(this->color_, this->opacity_));
	}

	const math::float3&
	LambertMaterial::getColor() const noexcept
	{
		return this->color_;
	}

	void
	LambertMaterial::setColorMap(const hal::GraphicsTexturePtr& map) noexcept
	{
		this->colorTexture_ = map;
		this->set("decal", map);
		this->set("hasTexture", map ? true : false);
	}

	const hal::GraphicsTexturePtr&
	LambertMaterial::getColorMap() const noexcept
	{
		return this->colorTexture_;
	}

	void
	LambertMaterial::setOpacity(float opacity) noexcept
	{
		this->opacity_ = opacity;
		this->set("color", math::float4(this->color_, this->opacity_));
	}

	float
	LambertMaterial::getOpacity() const noexcept
	{
		return opacity_;
	}

	std::shared_ptr<Material>
	LambertMaterial::clone() const noexcept
	{
		auto instance = std::make_shared<LambertMaterial>();
		instance->setColor(this->getColor());
		instance->setColorMap(this->getColorMap());
		instance->setOpacity(this->getOpacity());
		return instance;
	}
}