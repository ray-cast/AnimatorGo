#include <octoon/material/lambert_material.h>
#include <octoon/video/render_system.h>
#include <octoon/runtime/except.h>

namespace octoon::material
{
	LambertMaterial::LambertMaterial() except
	{
		this->setup();
	}

	void
	LambertMaterial::setup() except
	{
		const char* vert = R"(#version 330
		uniform mat4 proj;
		uniform mat4 model;

		layout(location = 0) in vec4 POSITION0;
		layout(location = 1) in vec4 NORMAL0;

		out vec3 oTexcoord0;
		out vec3 oTexcoord1;

		void main()
		{
			oTexcoord0 = inverse(mat3(model)) * normalize(NORMAL0.xyz);
			oTexcoord1 = normalize(POSITION0.xyz);
			gl_Position = proj * model * POSITION0;
		})";

		const char* frag = R"(#version 330

		uniform vec3 lightDir;
		uniform vec3 baseColor;
		uniform vec3 ambientColor;
		uniform vec3 darkColor;
		uniform float ambient;
		uniform float lightIntensity;

		layout(location  = 0) out vec4 fragColor;

		in vec3 oTexcoord0;
		in vec3 oTexcoord1;

		void main()
		{
			vec3 base = baseColor;

			vec3 L = lightDir;
			vec3 N = normalize(oTexcoord0);
			vec3 V = normalize(oTexcoord1);

			float nl = max(0.0f, dot(N, L));

			vec3 lighting = mix(darkColor, ambientColor, ambient);
			lighting = mix(lighting, base, nl * lightIntensity);

			fragColor = vec4(lighting, 1.0f);
		})";

		hal::GraphicsProgramDesc programDesc;
		programDesc.addShader(video::RenderSystem::instance()->createShader(hal::GraphicsShaderDesc(hal::GraphicsShaderStageFlagBits::VertexBit, vert, "main", hal::GraphicsShaderLang::GLSL)));
		programDesc.addShader(video::RenderSystem::instance()->createShader(hal::GraphicsShaderDesc(hal::GraphicsShaderStageFlagBits::FragmentBit, frag, "main", hal::GraphicsShaderLang::GLSL)));
		auto program = video::RenderSystem::instance()->createProgram(programDesc);

		hal::GraphicsInputLayoutDesc layoutDesc;
		layoutDesc.addVertexLayout(hal::GraphicsVertexLayout(0, "POSITION", 0, hal::GraphicsFormat::R32G32B32SFloat));
		layoutDesc.addVertexLayout(hal::GraphicsVertexLayout(0, "NORMAL", 0, hal::GraphicsFormat::R32G32B32SFloat));
		layoutDesc.addVertexBinding(hal::GraphicsVertexBinding(0, layoutDesc.getVertexSize()));

		hal::GraphicsDescriptorSetLayoutDesc descriptor_set_layout;
		descriptor_set_layout.setUniformComponents(program->getActiveParams());

		hal::GraphicsStateDesc stateDesc;
		stateDesc.setPrimitiveType(hal::GraphicsVertexType::TriangleList);
		stateDesc.setCullMode(hal::GraphicsCullMode::None);
		stateDesc.setDepthEnable(true);

		hal::GraphicsPipelineDesc pipeline;
		pipeline.setGraphicsInputLayout(video::RenderSystem::instance()->createInputLayout(layoutDesc));
		pipeline.setGraphicsState(video::RenderSystem::instance()->createRenderState(stateDesc));
		pipeline.setGraphicsProgram(std::move(program));
		pipeline.setGraphicsDescriptorSetLayout(video::RenderSystem::instance()->createDescriptorSetLayout(descriptor_set_layout));

		pipeline_ = video::RenderSystem::instance()->createRenderPipeline(pipeline);
		if (!pipeline_)
			return;

		hal::GraphicsDescriptorSetDesc descriptorSet;
		descriptorSet.setGraphicsDescriptorSetLayout(pipeline.getDescriptorSetLayout());
		descriptorSet_ = video::RenderSystem::instance()->createDescriptorSet(descriptorSet);
		if (!descriptorSet_)
			return;

		auto begin = descriptorSet_->getUniformSets().begin();
		auto end = descriptorSet_->getUniformSets().end();

		proj_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "proj"; });
		model_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "model"; });
		lightDir_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "lightDir"; });
		lightIntensity_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "lightIntensity"; });
		baseColor_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "baseColor"; });
		ambient_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "ambient"; });
		ambientColor_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "ambientColor"; });
		darkColor_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "darkColor"; });

		lightDir_->uniform3f(math::float3::UnitY);
		baseColor_->uniform3f(math::float3::One);
		ambientColor_->uniform3f(math::float3::Zero);
		darkColor_->uniform3f(math::float3::Zero);
	}

	LambertMaterial::~LambertMaterial() noexcept
	{
	}

	void
	LambertMaterial::setTransform(const math::float4x4& m) noexcept
	{
		model_->uniform4fmat(m);
	}

	void
	LambertMaterial::setViewProjection(const math::float4x4& vp) noexcept
	{
		proj_->uniform4fmat(vp);
	}

	const hal::GraphicsPipelinePtr&
	LambertMaterial::getPipeline() const noexcept
	{
		return pipeline_;
	}

	const hal::GraphicsDescriptorSetPtr&
	LambertMaterial::getDescriptorSet() const noexcept
	{
		return descriptorSet_;
	}

	void
	LambertMaterial::setLightDir(const math::float3& dir) noexcept
	{
		lightDir_->uniform3f(dir);
	}

	void
	LambertMaterial::setLightIntensity(const math::float1& intensity) noexcept
	{
		lightIntensity_->uniform1f(intensity);
	}

	void
	LambertMaterial::setAmbient(math::float1 weight) noexcept
	{
		ambient_->uniform1f(weight);
	}

	void
	LambertMaterial::setBaseColor(const math::float3& color) noexcept
	{
		baseColor_->uniform3f(color);
	}

	void
	LambertMaterial::setAmbientColor(const math::float3& color) noexcept
	{
		ambientColor_->uniform3f(color);
	}

	void
	LambertMaterial::setDarkColor(const math::float3& color) noexcept
	{
		darkColor_->uniform3f(color);
	}

	const math::float3&
	LambertMaterial::getLightDir() const noexcept
	{
		return lightDir_->getFloat3();
	}

	math::float1
	LambertMaterial::getLightIntensity() const noexcept
	{
		return lightIntensity_->getFloat();
	}

	math::float1
	LambertMaterial::getAmbient() const noexcept
	{
		return ambient_->getFloat();
	}

	const math::float3&
	LambertMaterial::getBaseColor() const noexcept
	{
		return baseColor_->getFloat3();
	}

	const math::float3&
	LambertMaterial::getAmbientColor() const noexcept
	{
		return ambientColor_->getFloat3();
	}

	const math::float3&
	LambertMaterial::getDarkColor() const noexcept
	{
		return darkColor_->getFloat3();
	}

	video::RenderPipelinePtr
	LambertMaterial::clone() const noexcept
	{
		auto instance = std::make_shared<LambertMaterial>();
		instance->setLightDir(this->getLightDir());

		return instance;
	}
}