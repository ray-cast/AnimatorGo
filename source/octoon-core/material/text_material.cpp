#include <octoon/material/text_material.h>
#include <octoon/video/render_system.h>
#include <octoon/runtime/except.h>

namespace octoon::material
{
	TextMaterial::TextMaterial() except
	{
		this->setup();
	}

	void
	TextMaterial::setup() except
	{
#if defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN) || defined(OCTOON_BUILD_PLATFORM_ANDROID)
		const char* vert = R"(
		precision mediump float;
		uniform mat4 proj;
		uniform mat4 model;
		uniform float lean;
		uniform vec3 frontColor;
		uniform vec3 sideColor;
		uniform vec3 translate;

		attribute vec4 POSITION0;
		attribute vec4 NORMAL0;

		varying vec3 oTexcoord0;

		void main()
		{
			vec4 P = POSITION0;
			P.x -= P.y * lean;
			if (P.z == 0.0)
				P.xyz += translate;

			if (abs(NORMAL0.z) > 0.5)
				oTexcoord0 = frontColor;
			else
				oTexcoord0 = sideColor;

			gl_Position = proj * model * P;
		})";

		const char* frag = R"(
		precision mediump float;
		varying vec3 oTexcoord0;
		void main()
		{
			gl_FragColor = vec4(oTexcoord0, 1.0);
		})";
#else
		const char* vert = R"(#version 330
		uniform mat4 proj;
		uniform mat4 model;
		uniform float lean;
		uniform vec3 frontColor;
		uniform vec3 sideColor;
		uniform vec3 translate;

		layout(location  = 0) in vec4 POSITION0;
		layout(location  = 1) in vec4 NORMAL0;

		out vec3 oTexcoord0;

		void main()
		{
			vec4 P = POSITION0;
			P.x -= P.y * lean;
			if (P.z == 0)
				P.xyz += translate;

			if (abs(NORMAL0.z) > 0.5)
				oTexcoord0 = frontColor;
			else
				oTexcoord0 = sideColor;

			gl_Position = proj * model * P;
		})";

		const char* frag = R"(#version 330
		layout(location  = 0) out vec4 fragColor;
		in vec3 oTexcoord0;
		void main()
		{
			fragColor = vec4(oTexcoord0, 1.0f);
		})";
#endif
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

		translate_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "translate"; });
		proj_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "proj"; });
		model_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "model"; });
		lean_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "lean"; });
		frontColor_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "frontColor"; });
		sideColor_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "sideColor"; });
	}

	TextMaterial::~TextMaterial() noexcept
	{
	}

	void
	TextMaterial::setTransform(const math::float4x4& m) noexcept
	{
		model_->uniform4fmat(m);
	}

	void
	TextMaterial::setViewProjection(const math::float4x4& vp) noexcept
	{
		proj_->uniform4fmat(vp);
	}

	const hal::GraphicsPipelinePtr&
	TextMaterial::getPipeline() const noexcept
	{
		return pipeline_;
	}

	const hal::GraphicsDescriptorSetPtr&
	TextMaterial::getDescriptorSet() const noexcept
	{
		return descriptorSet_;
	}

	void
	TextMaterial::setLean(float lean) noexcept
	{
		lean_->uniform1f(lean);
	}

	void
	TextMaterial::setTextColor(TextColor::Type which, const math::float3& colors) except
	{
		switch (which)
		{
		case TextColor::FrontColor:
			frontColor_->uniform3f(colors);
			break;
		case TextColor::SideColor:
			sideColor_->uniform3f(colors);
			break;
		default:
			throw runtime::out_of_range::create("Unknown enum type of text color");
		}
	}

	void
	TextMaterial::setTranslate(const math::float3& translate) noexcept
	{
		translate_->uniform3f(translate);
	}

	float
	TextMaterial::getLean() const noexcept
	{
		return lean_->getFloat();
	}

	const math::float3&
	TextMaterial::getTranslate() const noexcept
	{
		return translate_->getFloat3();
	}

	const math::float3&
	TextMaterial::getTextColor(TextColor::Type which) const except
	{
		switch (which)
		{
		case TextColor::FrontColor:
			return frontColor_->getFloat3();
		case TextColor::SideColor:
			return sideColor_->getFloat3();
		default:
			throw runtime::out_of_range::create("Unknown enum type of text color");
		}
	}

	video::RenderPipelinePtr
	TextMaterial::clone() const noexcept
	{
		auto instance = std::make_shared<TextMaterial>();
		instance->setTranslate(this->getTranslate());
		instance->setLean(this->getLean());
		instance->setTextColor(TextColor::FrontColor, this->getTextColor(TextColor::FrontColor));
		instance->setTextColor(TextColor::SideColor, this->getTextColor(TextColor::SideColor));

		return instance;
	}
}