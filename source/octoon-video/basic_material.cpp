#include <octoon/video/basic_material.h>
#include <octoon/video/render_system.h>
#include <octoon/runtime/except.h>

namespace octoon
{
	namespace video
	{
		BasicMaterial::BasicMaterial() except
		{
			this->setup();
		}

		BasicMaterial::BasicMaterial(const graphics::GraphicsTexturePtr& texture) except
		{
			this->setup();
			this->setTexture(texture);
		}

		void
		BasicMaterial::setup() except
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
				gl_Position = proj * model * POSITION0;
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

			layout(location  = 0) in vec4 POSITION0;
			layout(location  = 1) in vec2 TEXCOORD0;

			out vec2 oTexcoord0;

			void main()
			{
				oTexcoord0 = TEXCOORD0;
				gl_Position = proj * model * POSITION0;
			})";

			const char* frag = R"(#version 330
			layout(location  = 0) out vec4 fragColor;

			uniform sampler2D decal;

			in vec2 oTexcoord0;

			void main()
			{
				fragColor = texture(decal, oTexcoord0);
			})";
#endif
			graphics::GraphicsProgramDesc programDesc;
			programDesc.addShader(RenderSystem::instance()->createShader(graphics::GraphicsShaderDesc(graphics::GraphicsShaderStageFlagBits::VertexBit, vert, "main", graphics::GraphicsShaderLang::GLSL)));
			programDesc.addShader(RenderSystem::instance()->createShader(graphics::GraphicsShaderDesc(graphics::GraphicsShaderStageFlagBits::FragmentBit, frag, "main", graphics::GraphicsShaderLang::GLSL)));
			auto program = RenderSystem::instance()->createProgram(programDesc);

			graphics::GraphicsInputLayoutDesc layoutDesc;
			layoutDesc.addVertexLayout(graphics::GraphicsVertexLayout(0, "POSITION", 0, graphics::GraphicsFormat::R32G32B32SFloat));
			layoutDesc.addVertexLayout(graphics::GraphicsVertexLayout(0, "TEXCOORD", 0, graphics::GraphicsFormat::R32G32SFloat));
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
			descriptorSet.setGraphicsDescriptorSetLayout(pipeline.getDescriptorSetLayout());
			descriptorSet_ = RenderSystem::instance()->createDescriptorSet(descriptorSet);
			if (!descriptorSet_)
				return;

			auto begin = descriptorSet_->getGraphicsUniformSets().begin();
			auto end = descriptorSet_->getGraphicsUniformSets().end();

			proj_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "proj"; });
			model_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "model"; });
			decal_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "decal"; });
		}

		BasicMaterial::~BasicMaterial() noexcept
		{
		}

		void
		BasicMaterial::setTransform(const math::float4x4& m) noexcept
		{
			model_->uniform4fmat(m);
		}

		void
		BasicMaterial::setViewProjection(const math::float4x4& vp) noexcept
		{
			proj_->uniform4fmat(vp);
		}

		void
		BasicMaterial::setTexture(const graphics::GraphicsTexturePtr& texture) noexcept
		{
			decal_->uniformTexture(texture);
		}

		const graphics::GraphicsPipelinePtr&
		BasicMaterial::getPipeline() const noexcept
		{
			return pipeline_;
		}

		const graphics::GraphicsDescriptorSetPtr&
		BasicMaterial::getDescriptorSet() const noexcept
		{
			return descriptorSet_;
		}

		MaterialPtr
		BasicMaterial::clone() const noexcept
		{
			auto instance = std::make_shared<BasicMaterial>();


			return instance;
		}
	}
}