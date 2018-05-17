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
			stateDesc.setPrimitiveType(graphics::GraphicsVertexType::LineList);
			stateDesc.setCullMode(graphics::GraphicsCullMode::None);
			stateDesc.setDepthEnable(true);
			stateDesc.setLineWidth(lineWidth_);

			graphics::GraphicsPipelineDesc pipeline;
			pipeline.setGraphicsInputLayout(RenderSystem::instance()->createInputLayout(layoutDesc));
			pipeline.setGraphicsState(RenderSystem::instance()->createRenderState(stateDesc));
			pipeline.setGraphicsProgram(std::move(program));
			pipeline.setGraphicsDescriptorSetLayout(RenderSystem::instance()->createDescriptorSetLayout(descriptor_set_layout));

			pipeline_ = RenderSystem::instance()->createRenderPipeline(pipeline);
			if (!pipeline_)
				return;

			graphics::GraphicsDescriptorSetDesc descriptorSet;
			descriptorSet.setGraphicsDescriptorSetLayout(pipeline.getDescriptorSetLayout());
			descriptorSet_ = RenderSystem::instance()->createDescriptorSet(descriptorSet);
			if (!descriptorSet_)
				return;

			auto begin = descriptorSet_->getGraphicsUniformSets().begin();
			auto end = descriptorSet_->getGraphicsUniformSets().end();

			proj_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "proj"; });
			model_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "model"; });
			color_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "color"; });

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

		const graphics::GraphicsPipelinePtr&
		LineMaterial::getPipeline() const noexcept
		{
			return pipeline_;
		}

		const graphics::GraphicsDescriptorSetPtr&
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

		MaterialPtr
		LineMaterial::clone() const noexcept
		{
			auto instance = std::make_shared<LineMaterial>();
			instance->setColor(this->getColor());

			return instance;
		}
	}
}