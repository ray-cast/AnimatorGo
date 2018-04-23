#include <octoon/video/text_material.h>
#include <octoon/video/render_system.h>
#include <octoon/runtime/except.h>

namespace octoon
{
	namespace video
	{
		const char* vert =
			"#version 330\n"
			"uniform mat4 proj;\n"
			"uniform mat4 model;\n"
			"uniform float extrude;\n"
			"uniform float lean;\n"
			"uniform vec3 frontColor;\n"
			"uniform vec3 sideColor;\n"
			"uniform vec3 translate;\n"
			"layout(location  = 0) in vec4 POSITION0;\n"
			"layout(location  = 1) in vec4 NORMAL0;\n"
			"out vec3 oTexcoord0;\n"
			"void main()\n"
			"{\n"
			"vec4 P = POSITION0;\n"
			"P.x -= P.y * lean;\n"
			"P.z *= extrude;\n"
			"P.xyz += translate;\n"
			"if (abs(NORMAL0.z) > 0.5)\n"
			"oTexcoord0 = frontColor;\n"
			"else\n"
			"oTexcoord0 = sideColor;\n"
			"gl_Position = proj * model * P;\n"
			"}\n";

		const char* frag =
			"#version 330\n"
			"uniform sampler2D decal;\n"
			"layout(location  = 0) out vec4 oColor;\n"
			"in vec3 oTexcoord0;\n"
			"void main()\n"
			"{\n"
			"	oColor = vec4(oTexcoord0, 1.0f);\n"
			"}";

		TextMaterial::TextMaterial() except
		{
			this->setup();
		}

		void
		TextMaterial::setup() except
		{
			graphics::GraphicsProgramDesc programDesc;
			programDesc.addShader(RenderSystem::instance()->createShader(graphics::GraphicsShaderDesc(graphics::GraphicsShaderStageFlagBits::VertexBit, vert, "main", graphics::GraphicsShaderLang::GLSL)));
			programDesc.addShader(RenderSystem::instance()->createShader(graphics::GraphicsShaderDesc(graphics::GraphicsShaderStageFlagBits::FragmentBit, frag, "main", graphics::GraphicsShaderLang::GLSL)));
			auto program = RenderSystem::instance()->createProgram(programDesc);

			graphics::GraphicsInputLayoutDesc layoutDesc;
			layoutDesc.addVertexLayout(graphics::GraphicsVertexLayout(0, "POSITION", 0, graphics::GraphicsFormat::R32G32B32SFloat));
			layoutDesc.addVertexLayout(graphics::GraphicsVertexLayout(0, "NORMAL", 0, graphics::GraphicsFormat::R32G32B32SFloat));
			layoutDesc.addVertexBinding(graphics::GraphicsVertexBinding(0, layoutDesc.getVertexSize()));

			graphics::GraphicsDescriptorSetLayoutDesc descriptor_set_layout;
			descriptor_set_layout.setUniformComponents(program->getActiveParams());

			graphics::GraphicsStateDesc stateDesc;
			stateDesc.setPrimitiveType(graphics::GraphicsVertexType::TriangleList);
			stateDesc.setCullMode(graphics::GraphicsCullMode::None);
			stateDesc.setDepthEnable(true);

			graphics::GraphicsPipelineDesc pipeline;
			pipeline.setGraphicsInputLayout(RenderSystem::instance()->createInputLayout(layoutDesc));
			pipeline.setGraphicsState(RenderSystem::instance()->createRenderState(stateDesc));
			pipeline.setGraphicsProgram(std::move(program));
			pipeline.setGraphicsDescriptorSetLayout(RenderSystem::instance()->createDescriptorSetLayout(descriptor_set_layout));

			pipeline_ = RenderSystem::instance()->createRenderPipeline(pipeline);
			if (!pipeline_)
				return;

			graphics::GraphicsDescriptorSetDesc descriptorSet;
			descriptorSet.setGraphicsDescriptorSetLayout(pipeline.getGraphicsDescriptorSetLayout());
			descriptorSet_ = RenderSystem::instance()->createDescriptorSet(descriptorSet);
			if (!descriptorSet_)
				return;

			auto begin = descriptorSet_->getGraphicsUniformSets().begin();
			auto end = descriptorSet_->getGraphicsUniformSets().end();

			translate_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->get_name() == "translate"; });
			proj_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->get_name() == "proj"; });
			model_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->get_name() == "model"; });
			lean_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->get_name() == "lean"; });
			extrude_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->get_name() == "extrude"; });
			frontColor_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->get_name() == "frontColor"; });
			sideColor_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->get_name() == "sideColor"; });

			extrude_->uniform1f(1.0f);
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

		graphics::GraphicsPipelinePtr
		TextMaterial::getPipeline() const noexcept
		{
			return pipeline_;
		}

		graphics::GraphicsDescriptorSetPtr
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
		TextMaterial::setExtrude(float extrude) noexcept
		{
			extrude_->uniform1f(extrude);
		}

		void
		TextMaterial::setTextColor(TextColor::Type which, const math::float3& colors) except
		{
			switch (which)
			{
			case octoon::video::TextColor::FrontColor:
				frontColor_->uniform3f(colors);
				break;
			case octoon::video::TextColor::SideColor:
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

		float
		TextMaterial::getExtrude() const noexcept
		{
			return extrude_->getFloat();
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
			case octoon::video::TextColor::FrontColor:
				return frontColor_->getFloat3();
			case octoon::video::TextColor::SideColor:
				return sideColor_->getFloat3();
			default:
				throw runtime::out_of_range::create("Unknown enum type of text color");
			}
		}

		MaterialPtr
		TextMaterial::clone() const noexcept
		{
			auto instance = std::make_shared<TextMaterial>();
			instance->setTranslate(this->getTranslate());
			instance->setExtrude(this->getExtrude());
			instance->setLean(this->getLean());
			instance->setTextColor(TextColor::FrontColor, this->getTextColor(TextColor::FrontColor));
			instance->setTextColor(TextColor::SideColor, this->getTextColor(TextColor::SideColor));

			return instance;
		}
	}
}