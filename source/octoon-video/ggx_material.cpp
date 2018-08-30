#include <octoon/video/ggx_material.h>
#include <octoon/video/render_system.h>
#include <octoon/runtime/except.h>

namespace octoon
{
	namespace video
	{
		GGXMaterial::GGXMaterial() except
		{
			this->setup();
		}

		GGXMaterial::GGXMaterial(const graphics::GraphicsTexturePtr& texture) except
			:GGXMaterial()
		{
			this->setTexture(texture);
		}

		GGXMaterial::~GGXMaterial() noexcept
		{
		}

		void
		GGXMaterial::setup() except
		{
#if defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN) || defined(OCTOON_BUILD_PLATFORM_ANDROID)
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
				oTexcoord0 = normalize(mat3(model) * NORMAL0.xyz);
				oTexcoord1 = normalize(POSITION0.xyz);
				gl_Position = proj * model * POSITION0;
			})";

			const char* frag = R"(
			precision mediump float;

			uniform vec3 lightDir;
			uniform vec3 baseColor;
			uniform vec3 specularColor;
			uniform vec3 ambientColor;

			uniform float smoothness;
			uniform float metalness;

			varying vec3 oTexcoord0;
			varying vec3 oTexcoord1;

			void main()
			{
				vec3 ambient = pow(ambientColor, vec3(2.2));
				vec3 base = pow(baseColor, vec3(2.2));
				vec3 specular = pow(specularColor, vec3(2.2)) * 0.04;

				vec3 L = -lightDir;
				vec3 N = normalize(oTexcoord0);
				vec3 V = normalize(oTexcoord1);
				vec3 H = normalize(V + L);

				float nl = max(dot(N, L), 0.0);
				float nv = max(dot(N, V), 1e-5f);
				float nh = max(dot(N, H), 0.0);
				float vh = max(dot(V, H), 0.0);

				float roughness = max(1e-4, (1.0f - smoothness) * (1.0f - smoothness));
				float m2 = roughness * roughness;

				float spec = (nh * m2 - nh) * nh + 1.0;
				spec = m2 / (spec * spec);

				float Gv = nl * (nv * (1.0f - roughness) + roughness);
				float Gl = nv * (nl * (1.0f - roughness) + roughness);
				spec *= 0.5f / (Gv + Gl);

				vec3 f0 = mix(specular, base, vec3(metalness));
				vec3 f90 = vec3(clamp(dot(f0, vec3(0.33333)) * 50.0, 0.0, 1.0));
				vec3 fresnel = mix(f0, f90, vec3(pow(1.0 - vh, 5.0)));

				vec3 diffuse = mix(base, vec3(0.0), vec3(metalness));

				gl_FragColor = vec4(pow(ambient + (diffuse + spec * fresnel) * nl, vec3(1.0 / 2.2)), 1.0);
			})";
#else
			const char* vert = R"(#version 330
			uniform mat4 proj;
			uniform mat4 model;

			layout(location = 0) in vec4 POSITION0;
			layout(location = 1) in vec2 TEXCOORD0;
			layout(location = 2) in vec4 NORMAL0;

			out vec3 oTexcoord0;
			out vec2 oTexcoord1;
			out vec3 oTexcoord2;

			void main()
			{
				oTexcoord0 = normalize(inverse(mat3(model)) * NORMAL0.xyz);
				oTexcoord1 = TEXCOORD0;
				oTexcoord2 = normalize(POSITION0.xyz);
				gl_Position = proj * model * POSITION0;
			})";

			const char* frag = R"(#version 330

			uniform vec3 lightDir;
			uniform vec3 baseColor;
			uniform vec3 specularColor;
			uniform vec3 ambientColor;

			uniform float smoothness;
			uniform float metalness;
			uniform sampler2D decal;

			layout(location = 0) out vec4 fragColor;

			in vec3 oTexcoord0;
			in vec2 oTexcoord1;
			in vec3 oTexcoord2;

			void main()
			{
				vec3 ambient = pow(ambientColor, vec3(2.2f));
				vec3 base = pow(baseColor, vec3(2.2f));
				vec3 specular = pow(specularColor, vec3(2.2f)) * 0.04f;

				vec3 L = -lightDir;
				vec3 N = normalize(oTexcoord0);
				vec3 V = normalize(oTexcoord2);
				vec3 H = normalize(V + L);

				float nl = max(dot(N, L), 0.0f);
				float nv = max(dot(N, V), 1e-5f);
				float nh = max(dot(N, H), 0.0f);
				float vh = max(dot(V, H), 0.0f);

				float roughness = max(1e-4f, (1.0 - smoothness) * (1.0 - smoothness));
				float m2 = roughness * roughness;

				float spec = (nh * m2 - nh) * nh + 1;
				spec = m2 / (spec * spec);

				float Gv = nl * (nv * (1.0f - roughness) + roughness);
				float Gl = nv * (nl * (1.0f - roughness) + roughness);
				spec *= 0.5f / (Gv + Gl);

				vec3 f0 = mix(specular, base, vec3(metalness));
				vec3 f90 = vec3(clamp(dot(f0, vec3(0.33333f)) * 50.0f, 0.0f, 1.0f));
				vec3 fresnel = mix(f0, f90, vec3(pow(1.0 - vh, 5.0f)));

				vec3 diffuse = mix(base, vec3(0.0f), vec3(metalness));

				fragColor = vec4(pow(texture2D(decal, oTexcoord1).rgb * (ambient + (diffuse + spec * fresnel * 0.001f) * nl), vec3(1.0f / 2.2f)), 1.0f);
			})";
#endif

			graphics::GraphicsProgramDesc programDesc;
			programDesc.addShader(RenderSystem::instance()->createShader(graphics::GraphicsShaderDesc(graphics::GraphicsShaderStageFlagBits::VertexBit, vert, "main", graphics::GraphicsShaderLang::GLSL)));
			programDesc.addShader(RenderSystem::instance()->createShader(graphics::GraphicsShaderDesc(graphics::GraphicsShaderStageFlagBits::FragmentBit, frag, "main", graphics::GraphicsShaderLang::GLSL)));
			auto program = RenderSystem::instance()->createProgram(programDesc);

			graphics::GraphicsInputLayoutDesc layoutDesc;
			layoutDesc.addVertexLayout(graphics::GraphicsVertexLayout(0, "POSITION", 0, graphics::GraphicsFormat::R32G32B32SFloat));
			layoutDesc.addVertexLayout(graphics::GraphicsVertexLayout(0, "TEXCOORD", 0, graphics::GraphicsFormat::R32G32SFloat));
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

			auto begin = descriptorSet_->getUniformSets().begin();
			auto end = descriptorSet_->getUniformSets().end();

			proj_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "proj"; });
			model_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "model"; });
			lightDir_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "lightDir"; });
			baseColor_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "baseColor"; });
			ambientColor_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "ambientColor"; });
			specularColor_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "specularColor"; });
			smoothness_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "smoothness"; });
			metalness_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "metalness"; });
			decal_ = *std::find_if(begin, end, [](const graphics::GraphicsUniformSetPtr& set) { return set->getName() == "decal"; });

			lightDir_->uniform3f(-math::float3::UnitY);
			baseColor_->uniform3f(math::float3::One);
			ambientColor_->uniform3f(math::float3::One);
			specularColor_->uniform3f(math::float3::One);
			smoothness_->uniform1f(0.0f);
			metalness_->uniform1f(0.0f);
		}

		void
		GGXMaterial::setTransform(const math::float4x4& m) noexcept
		{
			model_->uniform4fmat(m);
		}

		void
		GGXMaterial::setViewProjection(const math::float4x4& vp) noexcept
		{
			proj_->uniform4fmat(vp);
		}

		const graphics::GraphicsPipelinePtr&
		GGXMaterial::getPipeline() const noexcept
		{
			return pipeline_;
		}

		const graphics::GraphicsDescriptorSetPtr&
		GGXMaterial::getDescriptorSet() const noexcept
		{
			return descriptorSet_;
		}

		void
		GGXMaterial::setLightDir(const math::float3& dir) noexcept
		{
			lightDir_->uniform3f(dir);
		}

		void
		GGXMaterial::setBaseColor(const math::float3& color) noexcept
		{
			baseColor_->uniform3f(color);
		}

		void
		GGXMaterial::setAmbientColor(const math::float3& color) noexcept
		{
			ambientColor_->uniform3f(color);
		}

		void
		GGXMaterial::setSpecularColor(const math::float3& color) noexcept
		{
			specularColor_->uniform3f(color);
		}

		void 
		GGXMaterial::setTexture(const graphics::GraphicsTexturePtr& texture) noexcept
		{
			decal_->uniformTexture(texture);
		}

		void
		GGXMaterial::setSmoothness(float smoothness) noexcept
		{
			smoothness_->uniform1f(smoothness);
		}

		void
		GGXMaterial::setMetalness(float metalness) noexcept
		{
			metalness_->uniform1f(metalness);
		}

		const math::float3&
		GGXMaterial::getLightDir() const noexcept
		{
			return lightDir_->getFloat3();
		}

		const math::float3&
		GGXMaterial::getBaseColor() const noexcept
		{
			return baseColor_->getFloat3();
		}

		const math::float3&
		GGXMaterial::getAmbientColor() const noexcept
		{
			return ambientColor_->getFloat3();
		}

		const math::float3&
		GGXMaterial::getSpecularColor() const noexcept
		{
			return specularColor_->getFloat3();
		}

		float
		GGXMaterial::getMetalness() const noexcept
		{
			return metalness_->getFloat();
		}

		float
		GGXMaterial::getSmoothness() const noexcept
		{
			return smoothness_->getFloat();
		}

		const graphics::GraphicsTexturePtr&
		GGXMaterial::getTexture() noexcept
		{
			return decal_->getTexture();
		}

		MaterialPtr
		GGXMaterial::clone() const noexcept
		{
			auto instance = std::make_shared<GGXMaterial>();
			instance->setLightDir(this->getLightDir());

			return instance;
		}
	}
}