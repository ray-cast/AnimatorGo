#include <octoon/video/phong_material.h>
#include <octoon/video/render_system.h>
#include <octoon/runtime/except.h>

namespace octoon
{
	namespace video
	{
		PhongMaterial::PhongMaterial() except
		{
			this->setup();
		}

		void
		PhongMaterial::setup() except
		{
			const char* vert = R"(
			precision mediump float;
			uniform mat4 proj;
			uniform mat4 model;

			attribute vec4 POSITION0;
			attribute vec4 NORMAL0;

			varying vec3 oTexcoord0;
			varying vec3 oTexcoord1;

			void main()
			{
				oTexcoord0 = normalize(NORMAL0.xyz);
				oTexcoord1 = normalize(POSITION0.xyz);
				gl_Position = proj * model * POSITION0;
			})";

			const char* frag = R"(
			precision mediump float;

			uniform vec3 lightDir;
			uniform vec3 baseColor;
			uniform vec3 ambientColor;
			uniform vec3 specularColor;
			uniform vec3 darkColor;
			uniform float shininess;

			varying vec3 oTexcoord0;
			varying vec3 oTexcoord1;

			void main()
			{
				vec3 ambient = pow(ambientColor, vec3(2.2));
				vec3 base = pow(baseColor, vec3(2.2));
				vec3 specular = pow(specularColor, vec3(2.2));
				vec3 dark = pow(darkColor, vec3(2.2));

				vec3 N = normalize(oTexcoord0);
				vec3 V = normalize(oTexcoord1);
				vec3 H = normalize(V + lightDir);
				vec3 R = -reflect(N, lightDir);

				float nl = max(0.0, dot(N, lightDir));
				float spec = pow(max(0, dot(R, V)), pow(4096, shininess));

				gl_FragColor = vec4(pow(ambient + mix(dark, (base + specular * spec), nl), vec3(1.0 / 2.2)), 1.0);
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
			descriptorSet.setGraphicsDescriptorSetLayout(pipeline.getDescriptorSetLayout());
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
			specularColor_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "specularColor"; });
			darkColor_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "darkColor"; });
			shininess_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "shininess"; });

			lightDir_->uniform3f(math::float3::UnitY);
			baseColor_->uniform3f(math::float3::One);
			ambientColor_->uniform3f(math::float3::Zero);
			specularColor_->uniform3f(math::float3::One);
			darkColor_->uniform3f(math::float3::Zero);
		}

		PhongMaterial::~PhongMaterial() noexcept
		{
		}

		void
		PhongMaterial::setTransform(const math::float4x4& m) noexcept
		{
			model_->uniform4fmat(m);
		}

		void
		PhongMaterial::setViewProjection(const math::float4x4& vp) noexcept
		{
			proj_->uniform4fmat(vp);
		}

		const graphics::GraphicsPipelinePtr&
		PhongMaterial::getPipeline() const noexcept
		{
			return pipeline_;
		}

		const graphics::GraphicsDescriptorSetPtr&
		PhongMaterial::getDescriptorSet() const noexcept
		{
			return descriptorSet_;
		}

		void
		PhongMaterial::setLightDir(const math::float3& dir) noexcept
		{
			lightDir_->uniform3f(dir);
		}

		void
		PhongMaterial::setBaseColor(const math::float3& color) noexcept
		{
			baseColor_->uniform3f(color);
		}

		void
		PhongMaterial::setAmbientColor(const math::float3& color) noexcept
		{
			ambientColor_->uniform3f(color);
		}

		void
		PhongMaterial::setSpecularColor(const math::float3& color) noexcept
		{
			specularColor_->uniform3f(color);
		}

		void
		PhongMaterial::setDarkColor(const math::float3& color) noexcept
		{
			darkColor_->uniform3f(color);
		}

		void
		PhongMaterial::setShininess(float shininess) noexcept
		{
			shininess_->uniform1f(shininess);
		}

		const math::float3&
		PhongMaterial::getLightDir() const noexcept
		{
			return lightDir_->getFloat3();
		}

		const math::float3&
		PhongMaterial::getBaseColor() const noexcept
		{
			return baseColor_->getFloat3();
		}

		const math::float3&
		PhongMaterial::getAmbientColor() const noexcept
		{
			return ambientColor_->getFloat3();
		}

		const math::float3&
		PhongMaterial::getSpecularColor() const noexcept
		{
			return specularColor_->getFloat3();
		}

		const math::float3&
		PhongMaterial::getDarkColor() const noexcept
		{
			return darkColor_->getFloat3();
		}

		float
		PhongMaterial::getShininess() const noexcept
		{
			return shininess_->getFloat();
		}

		MaterialPtr
		PhongMaterial::clone() const noexcept
		{
			auto instance = std::make_shared<PhongMaterial>();
			instance->setLightDir(this->getLightDir());

			return instance;
		}
	}
}