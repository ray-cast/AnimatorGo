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
			uniform vec3 specularColor;
			uniform vec3 darkColor;
			uniform float shininess;
			uniform float lightIntensity;
			uniform float ambient;

			layout(location = 0) out vec4 fragColor;

			in vec3 oTexcoord0;
			in vec3 oTexcoord1;

			float lum(vec3 rgb)
			{
				return dot(rgb, vec3(0.30, 0.59, 0.11));
			}

			void main()
			{
				vec3 base = baseColor;
				vec3 specular = specularColor;

				vec3 L = lightDir;
				vec3 N = normalize(oTexcoord0);
				vec3 V = normalize(oTexcoord1);
				vec3 R = reflect(N, L);

				float nl = max(0.0f, dot(N, L));
				float spec = pow(max(0, dot(R, V)), pow(4096, shininess));

				vec3 lighting = mix(darkColor, ambientColor, ambient);
				lighting = mix(lighting, base + specular * spec, nl * lightIntensity);

				fragColor = vec4(lighting, 1.0f);
			})";

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
			stateDesc.setPrimitiveType(hal::GraphicsVertexType::TriangleList);
			stateDesc.setCullMode(hal::GraphicsCullMode::None);
			stateDesc.setDepthEnable(true);

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
			lightDir_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "lightDir"; });
			lightIntensity_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "lightIntensity"; });
			baseColor_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "baseColor"; });
			ambient_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "ambient"; });
			ambientColor_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "ambientColor"; });
			specularColor_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "specularColor"; });
			darkColor_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "darkColor"; });
			shininess_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "shininess"; });

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

		const hal::GraphicsPipelinePtr&
		PhongMaterial::getPipeline() const noexcept
		{
			return pipeline_;
		}

		const hal::GraphicsDescriptorSetPtr&
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
		PhongMaterial::setLightIntensity(const math::float1& intensity) noexcept
		{
			lightIntensity_->uniform1f(intensity);
		}

		void
		PhongMaterial::setBaseColor(const math::float3& color) noexcept
		{
			baseColor_->uniform3f(color);
		}

		void
		PhongMaterial::setAmbient(const math::float1& weight) noexcept
		{
			ambient_->uniform1f(weight);
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

		math::float1
		PhongMaterial::getLightIntensity() const noexcept
		{
			return lightIntensity_->getFloat();
		}

		const math::float3&
		PhongMaterial::getBaseColor() const noexcept
		{
			return baseColor_->getFloat3();
		}

		math::float1
		PhongMaterial::getAmbient() const noexcept
		{
			return ambient_->getFloat();
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

		RenderPipelinePtr
		PhongMaterial::clone() const noexcept
		{
			auto instance = std::make_shared<PhongMaterial>();
			instance->setLightDir(this->getLightDir());

			return instance;
		}
	}
}