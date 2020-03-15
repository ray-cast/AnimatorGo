#include <octoon/material/line_basic_material.h>

namespace octoon::material
{
	OctoonImplementSubClass(LineBasicMaterial, Material, "LineBasicMaterial");

	LineBasicMaterial::LineBasicMaterial() noexcept
		: LineBasicMaterial(math::float3::One)
	{
		this->setColor(math::float3::One);
	}

	LineBasicMaterial::LineBasicMaterial(const math::float3& color) noexcept
	{
#if defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN) || defined(OCTOON_BUILD_PLATFORM_ANDROID)
		const char* vert = R"(
			precision mediump float;
			uniform mat4 proj;
			uniform mat4 model;

			attribute vec4 POSITION0;
			attribute vec4 NORMAL0;

			varying vec3 oTexcoord0;

			void main()
			{
				oTexcoord0 = NORMAL0;
				gl_Position = proj * model * (POSITION0 * vec4(1,1,1,1));
			})";

		const char* frag = R"(
			precision mediump float;

			uniform sampler2D decal;
			uniform vec4 color;
			uniform bool hasTexture;

			varying vec2 oTexcoord0;
			void main()
			{
				fragColor = color;
				if (hasTexture) fragColor *= pow(texture(decal, oTexcoord0), vec4(2.2));
				fragColor = pow(fragColor, vec4(1.0 / 2.2));
			})";
#else
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

			uniform vec4 color;

			in vec2 oTexcoord0;

			void main()
			{
				fragColor = pow(color, vec4(1.0 / 2.2));
			})";
#endif

		this->setColor(color);
		this->setOpacity(1.0f);
		this->setDepthEnable(false);
		this->setPrimitiveType(octoon::hal::GraphicsVertexType::LineList);
		this->set(MATKEY_SHADER_VERT, vert);
		this->set(MATKEY_SHADER_FRAG, frag);
	}

	LineBasicMaterial::~LineBasicMaterial() noexcept
	{
	}

	void
	LineBasicMaterial::setColor(const math::float3& color) noexcept
	{
		this->set(MATKEY_COLOR_DIFFUSE, color);
		this->color_ = color;
	}

	const math::float3&
	LineBasicMaterial::getColor() const noexcept
	{
		return this->color_;
	}

	void
	LineBasicMaterial::setOpacity(float opacity) noexcept
	{
		this->set(MATKEY_OPACITY, opacity);
		this->opacity_ = opacity;
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