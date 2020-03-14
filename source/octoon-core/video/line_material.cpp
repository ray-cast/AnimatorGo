#include <octoon/video/line_material.h>
#include <octoon/video/render_system.h>
#include <octoon/runtime/except.h>

namespace octoon
{
	namespace video
	{
		LineMaterial::LineMaterial() except
			: lineWidth_(1.0f)
		{
			this->setup();
		}

		LineMaterial::LineMaterial(float lineWidth) except
			: lineWidth_(lineWidth)
		{
			this->setup();
		}

		void
		LineMaterial::setup() except
		{
#if defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN) || defined(OCTOON_BUILD_PLATFORM_ANDROID)
			const char* vert = R"(
			precision mediump float;
			uniform mat4 proj;
			uniform mat4 model;

			attribute vec4 POSITION0;
			attribute vec4 NORMAL0;

			void main()
			{
				gl_Position = proj * model * POSITION0;
			})";

			const char* frag = R"(
			precision mediump float;
			varying vec3 oTexcoord0;
			uniform vec3 color;
			void main()
			{
				gl_FragColor = vec4(color, 1.0);
			})";
#else
			const char* vert = R"(#version 330
			uniform mat4 proj;
			uniform mat4 model;

			layout(location  = 0) in vec4 POSITION0;
			layout(location  = 1) in vec4 NORMAL0;

			void main()
			{
				gl_Position = proj * model * POSITION0;
			})";

			const char* frag = R"(#version 330
			layout(location  = 0) out vec4 fragColor;
			in vec3 oTexcoord0;
			uniform vec3 color;
			void main()
			{
				fragColor = vec4(color, 1.0f);
			})";
#endif
			hal::GraphicsProgramDesc programDesc;
			programDesc.addShader(RenderSystem::instance()->createShader(hal::GraphicsShaderDesc(hal::GraphicsShaderStageFlagBits::VertexBit, vert, "main", hal::GraphicsShaderLang::GLSL)));
			programDesc.addShader(RenderSystem::instance()->createShader(hal::GraphicsShaderDesc(hal::GraphicsShaderStageFlagBits::FragmentBit, frag, "main", hal::GraphicsShaderLang::GLSL)));
			auto program = RenderSystem::instance()->createProgram(programDesc);

			hal::GraphicsInputLayoutDesc layoutDesc;
			layoutDesc.addVertexLayout(hal::GraphicsVertexLayout(0, "POSITION", 0, hal::GraphicsFormat::R32G32B32SFloat));
			layoutDesc.addVertexLayout(hal::GraphicsVertexLayout(0, "NORMAL", 0, hal::GraphicsFormat::R32G32B32SFloat));
			layoutDesc.addVertexBinding(hal::GraphicsVertexBinding(0, layoutDesc.getVertexSize()));

			hal::GraphicsDescriptorSetLayoutDesc descriptor_set_layout;
			descriptor_set_layout.setUniformComponents(program->getActiveParams());

			hal::GraphicsStateDesc stateDesc;
			stateDesc.setPrimitiveType(hal::GraphicsVertexType::LineList);
			stateDesc.setCullMode(hal::GraphicsCullMode::None);
			stateDesc.setDepthEnable(false);
			stateDesc.setLineWidth(lineWidth_);

			hal::GraphicsPipelineDesc pipeline;
			pipeline.setGraphicsInputLayout(RenderSystem::instance()->createInputLayout(layoutDesc));
			pipeline.setGraphicsState(RenderSystem::instance()->createRenderState(stateDesc));
			pipeline.setGraphicsProgram(std::move(program));
			pipeline.setGraphicsDescriptorSetLayout(RenderSystem::instance()->createDescriptorSetLayout(descriptor_set_layout));

			pipeline_ = RenderSystem::instance()->createRenderPipeline(pipeline);
			if (!pipeline_)
				return;

			hal::GraphicsDescriptorSetDesc descriptorSet;
			descriptorSet.setGraphicsDescriptorSetLayout(pipeline.getDescriptorSetLayout());
			descriptorSet_ = RenderSystem::instance()->createDescriptorSet(descriptorSet);
			if (!descriptorSet_)
				return;

			auto begin = descriptorSet_->getUniformSets().begin();
			auto end = descriptorSet_->getUniformSets().end();

			proj_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "proj"; });
			model_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "model"; });
			color_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "color"; });

			color_->uniform3f(math::float3::Zero);
		}

		LineMaterial::~LineMaterial() noexcept
		{
		}

		void
		LineMaterial::setTransform(const math::float4x4& m) noexcept
		{
			model_->uniform4fmat(m);
		}

		void
		LineMaterial::setViewProjection(const math::float4x4& vp) noexcept
		{
			proj_->uniform4fmat(vp);
		}

		const hal::GraphicsPipelinePtr&
		LineMaterial::getPipeline() const noexcept
		{
			return pipeline_;
		}

		const hal::GraphicsDescriptorSetPtr&
		LineMaterial::getDescriptorSet() const noexcept
		{
			return descriptorSet_;
		}

		void
		LineMaterial::setColor(const math::float3& colors) noexcept
		{
			color_->uniform3f(colors);
		}

		const math::float3&
		LineMaterial::getColor() const noexcept
		{
			return color_->getFloat3();
		}

		HALMaterialPtr
		LineMaterial::clone() const noexcept
		{
			auto instance = std::make_shared<LineMaterial>();
			instance->setColor(this->getColor());

			return instance;
		}
	}
}