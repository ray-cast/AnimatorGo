#include <octoon/video/blinn_material.h>
#include <octoon/video/render_system.h>
#include <octoon/runtime/except.h>

namespace octoon
{
	namespace video
	{
		BlinnMaterial::BlinnMaterial() except
		{
			this->setup();
		}

		void
		BlinnMaterial::setup() except
		{
			const char* vert = R"(#version 330
			uniform mat4 proj;
			uniform mat4 model;

			layout(location  = 0) in vec4 POSITION0;
			layout(location  = 1) in vec4 NORMAL0;

			out vec3 oTexcoord0;
			out vec3 oTexcoord1;

			void main()
			{
				oTexcoord0 = normalize(NORMAL0.xyz);
				oTexcoord1 = normalize(POSITION0.xyz);
				gl_Position = proj * model * POSITION0;
			})";

			const char* frag = R"(#version 330

			uniform vec3 lightDir;
			uniform vec3 baseColor;
			uniform vec3 ambientColor;
			uniform float shininess;

			layout(location  = 0) out vec4 fragColor;

			in vec3 oTexcoord0;
			in vec3 oTexcoord1;

			void main()
			{
				vec3 ambient = pow(ambientColor, vec3(2.2f));
				vec3 base = pow(baseColor, vec3(2.2f));

				vec3 N = normalize(oTexcoord0);
				vec3 V = normalize(oTexcoord1);
				vec3 H = normalize(V + lightDir);

				float nl = max(0.0f, dot(N, lightDir));
				float spec = pow(max(0, dot(N, H)), pow(4096, shininess));

				fragColor = vec4(pow(ambient + (base + spec) * nl, vec3(1.0f / 2.2f)), 1.0f);
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

			proj_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "proj"; });
			model_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "model"; });
			lightDir_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "lightDir"; });
			baseColor_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "baseColor"; });
			ambientColor_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "ambientColor"; });
			shininess_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "shininess"; });

			lightDir_->uniform3f(math::float3::UnitY);
			baseColor_->uniform3f(math::float3::One);
			ambientColor_->uniform3f(math::float3::Zero);
		}

		BlinnMaterial::~BlinnMaterial() noexcept
		{
		}

		void
		BlinnMaterial::setTransform(const math::float4x4& m) noexcept
		{
			model_->uniform4fmat(m);
		}

		void
		BlinnMaterial::setViewProjection(const math::float4x4& vp) noexcept
		{
			proj_->uniform4fmat(vp);
		}

		const graphics::GraphicsPipelinePtr&
		BlinnMaterial::getPipeline() const noexcept
		{
			return pipeline_;
		}

		const graphics::GraphicsDescriptorSetPtr&
		BlinnMaterial::getDescriptorSet() const noexcept
		{
			return descriptorSet_;
		}

		void
		BlinnMaterial::setLightDir(const math::float3& dir) noexcept
		{
			lightDir_->uniform3f(dir);
		}

		void
		BlinnMaterial::setBaseColor(const math::float3& color) noexcept
		{
			baseColor_->uniform3f(color);
		}

		void
		BlinnMaterial::setAmbientColor(const math::float3& color) noexcept
		{
			ambientColor_->uniform3f(color);
		}

		void
		BlinnMaterial::setShininess(float shininess) noexcept
		{
			shininess_->uniform1f(shininess);
		}

		const math::float3&
		BlinnMaterial::getLightDir() const noexcept
		{
			return lightDir_->getFloat3();
		}

		const math::float3&
		BlinnMaterial::getBaseColor() const noexcept
		{
			return baseColor_->getFloat3();
		}

		const math::float3&
		BlinnMaterial::getAmbientColor() const noexcept
		{
			return ambientColor_->getFloat3();
		}

		float
		BlinnMaterial::getShininess() const noexcept
		{
			return shininess_->getFloat();
		}

		MaterialPtr
		BlinnMaterial::clone() const noexcept
		{
			auto instance = std::make_shared<BlinnMaterial>();
			instance->setLightDir(this->getLightDir());

			return instance;
		}
	}
}