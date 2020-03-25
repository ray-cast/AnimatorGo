#include <octoon/video/render_pipeline.h>
#include <octoon/video/render_system.h>
#include <octoon/hal/graphics.h>

namespace octoon
{
	namespace video
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
				gl_Position = proj * model * (POSITION0 * vec4(1,1,1,1));
			})";

		const char* frag = R"(
			precision mediump float;

			uniform sampler2D decal;
			uniform vec4 color;
			uniform bool hasTexture;

			varying vec2 oTexcoord0;
			void main()
			{
				fragColor = color;
				if (hasTexture) fragColor *= pow(texture(decal, oTexcoord0), vec4(2.2));
				fragColor = pow(fragColor, vec4(1.0 / 2.2));
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
				gl_Position = proj * model * (POSITION0 * vec4(1,1,1,1));
			})";

		const char* frag = R"(#version 330
			layout(location  = 0) out vec4 fragColor;

			uniform sampler2D decal;
			uniform vec4 color;
			uniform bool hasTexture;

			in vec2 oTexcoord0;

			void main()
			{
				fragColor = color;
				if (hasTexture) fragColor *= pow(texture(decal, oTexcoord0), vec4(2.2));
				fragColor = pow(fragColor, vec4(1.0 / 2.2));
			})";
#endif

		RenderPipeline::RenderPipeline() noexcept
		{
		}

		RenderPipeline::RenderPipeline(const material::MaterialPtr& material) noexcept
		{
			this->setMaterial(material);
		}

		RenderPipeline::~RenderPipeline() noexcept
		{
		}

		void
		RenderPipeline::setMaterial(const material::MaterialPtr& material) noexcept
		{
			if (this->material_ != material)
			{
				this->material_ = material;
				this->onMaterialReplace(this->material_);
			}
		}
		
		const material::MaterialPtr&
		RenderPipeline::getMaterial() const noexcept
		{
			return this->material_;
		}

		void
		RenderPipeline::setTransform(const math::float4x4& m) noexcept
		{
			model_->uniform4fmat(m);
		}

		void
		RenderPipeline::setViewProjection(const math::float4x4& vp) noexcept
		{
			proj_->uniform4fmat(vp);
		}

		const hal::GraphicsPipelinePtr&
		RenderPipeline::getPipeline() const noexcept
		{
			return pipeline_;
		}

		const hal::GraphicsDescriptorSetPtr&
		RenderPipeline::getDescriptorSet() const noexcept
		{
			return descriptorSet_;
		}

		void
		RenderPipeline::onMaterialReplace(const material::MaterialPtr& material) noexcept(false)
		{
			if (material) {
				hal::GraphicsStateDesc stateDesc;
				stateDesc.setColorBlends(material->getColorBlends());
				stateDesc.setCullMode(material->getCullMode());
				stateDesc.setPolygonMode(material->getPolygonMode());
				stateDesc.setPrimitiveType(material->getPrimitiveType());
				stateDesc.setFrontFace(material->getFrontFace());
				stateDesc.setScissorTestEnable(material->getScissorTestEnable());
				stateDesc.setLinear2sRGBEnable(material->getLinear2sRGBEnable());
				stateDesc.setMultisampleEnable(material->getMultisampleEnable());
				stateDesc.setRasterizerDiscardEnable(material->getRasterizerDiscardEnable());
				stateDesc.setLineWidth(material->getLineWidth());
				stateDesc.setDepthEnable(material->getDepthEnable());
				stateDesc.setDepthWriteEnable(material->getDepthWriteEnable());
				stateDesc.setDepthBoundsEnable(material->getDepthBoundsEnable());
				stateDesc.setDepthMin(material->getDepthMin());
				stateDesc.setDepthMax(material->getDepthMax());
				stateDesc.setDepthFunc(material->getDepthFunc());
				stateDesc.setDepthBiasEnable(material->getDepthBiasEnable());
				stateDesc.setDepthBias(material->getDepthBias());
				stateDesc.setDepthSlopeScaleBias(material->getDepthSlopeScaleBias());
				stateDesc.setDepthBiasClamp(material->getDepthBiasClamp());
				stateDesc.setDepthClampEnable(material->getDepthClampEnable());
				stateDesc.setStencilEnable(material->getStencilEnable());
				stateDesc.setStencilFrontRef(material->getStencilFrontRef());
				stateDesc.setStencilFrontFunc(material->getStencilFrontFunc());
				stateDesc.setStencilFrontReadMask(material->getStencilFrontReadMask());
				stateDesc.setStencilFrontWriteMask(material->getStencilFrontWriteMask());
				stateDesc.setStencilFrontFail(material->getStencilFrontFail());
				stateDesc.setStencilFrontZFail(material->getStencilFrontZFail());
				stateDesc.setStencilFrontPass(material->getStencilFrontPass());
				stateDesc.setStencilBackRef(material->getStencilBackRef());
				stateDesc.setStencilBackFunc(material->getStencilBackFunc());
				stateDesc.setStencilBackReadMask(material->getStencilBackReadMask());
				stateDesc.setStencilBackWriteMask(material->getStencilBackWriteMask());
				stateDesc.setStencilBackFail(material->getStencilBackFail());
				stateDesc.setStencilBackZFail(material->getStencilBackZFail());
				stateDesc.setStencilBackPass(material->getStencilBackPass());
				stateDesc.setDepthEnable(material->getDepthEnable());
				stateDesc.setDepthWriteEnable(material->getDepthWriteEnable());
				stateDesc.setDepthBoundsEnable(material->getDepthBoundsEnable());
				stateDesc.setDepthMin(material->getDepthMin());
				stateDesc.setDepthMax(material->getDepthMax());
				stateDesc.setDepthFunc(material->getDepthFunc());
				stateDesc.setDepthBiasEnable(material->getDepthBiasEnable());
				stateDesc.setDepthBias(material->getDepthBias());
				stateDesc.setDepthSlopeScaleBias(material->getDepthSlopeScaleBias());
				stateDesc.setDepthBiasClamp(material->getDepthBiasClamp());
				stateDesc.setDepthClampEnable(material->getDepthClampEnable());
				stateDesc.setStencilEnable(material->getStencilEnable());
				stateDesc.setStencilFrontRef(material->getStencilFrontRef());
				stateDesc.setStencilFrontFunc(material->getStencilFrontFunc());
				stateDesc.setStencilFrontReadMask(material->getStencilFrontReadMask());
				stateDesc.setStencilFrontWriteMask(material->getStencilFrontWriteMask());
				stateDesc.setStencilFrontFail(material->getStencilFrontFail());
				stateDesc.setStencilFrontZFail(material->getStencilFrontZFail());
				stateDesc.setStencilFrontPass(material->getStencilFrontPass());
				stateDesc.setStencilBackRef(material->getStencilBackRef());
				stateDesc.setStencilBackFunc(material->getStencilBackFunc());
				stateDesc.setStencilBackReadMask(material->getStencilBackReadMask());
				stateDesc.setStencilBackWriteMask(material->getStencilBackWriteMask());
				stateDesc.setStencilBackFail(material->getStencilBackFail());
				stateDesc.setStencilBackZFail(material->getStencilBackZFail());
				stateDesc.setStencilBackPass(material->getStencilBackPass());
				auto renderState = RenderSystem::instance()->createRenderState(stateDesc);

				auto shader = material->getShader();

				hal::GraphicsProgramDesc programDesc;
				programDesc.addShader(RenderSystem::instance()->createShader(hal::GraphicsShaderDesc(hal::GraphicsShaderStageFlagBits::VertexBit, shader->vs, "main", hal::GraphicsShaderLang::GLSL)));
				programDesc.addShader(RenderSystem::instance()->createShader(hal::GraphicsShaderDesc(hal::GraphicsShaderStageFlagBits::FragmentBit, shader->fs, "main", hal::GraphicsShaderLang::GLSL)));
				auto program = RenderSystem::instance()->createProgram(programDesc);

				hal::GraphicsInputLayoutDesc layoutDesc;
				layoutDesc.addVertexLayout(hal::GraphicsVertexLayout(0, "POSITION", 0, hal::GraphicsFormat::R32G32B32SFloat));
				layoutDesc.addVertexLayout(hal::GraphicsVertexLayout(0, "TEXCOORD", 0, hal::GraphicsFormat::R32G32SFloat));
				layoutDesc.addVertexBinding(hal::GraphicsVertexBinding(0, layoutDesc.getVertexSize()));

				hal::GraphicsDescriptorSetLayoutDesc descriptor_set_layout;
				descriptor_set_layout.setUniformComponents(program->getActiveParams());

				hal::GraphicsPipelineDesc pipeline;
				pipeline.setGraphicsInputLayout(RenderSystem::instance()->createInputLayout(layoutDesc));
				pipeline.setGraphicsState(renderState);
				pipeline.setGraphicsProgram(std::move(program));
				pipeline.setGraphicsDescriptorSetLayout(RenderSystem::instance()->createDescriptorSetLayout(descriptor_set_layout));

				pipeline_ = RenderSystem::instance()->createRenderPipeline(pipeline);
				if (pipeline_)
				{
					hal::GraphicsDescriptorSetDesc descriptorSet;
					descriptorSet.setGraphicsDescriptorSetLayout(pipeline.getDescriptorSetLayout());
					descriptorSet_ = RenderSystem::instance()->createDescriptorSet(descriptorSet);
					if (!descriptorSet_)
						return;

					auto begin = descriptorSet_->getUniformSets().begin();
					auto end = descriptorSet_->getUniformSets().end();

					proj_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "proj"; });
					model_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "model"; });

					this->updateParameters();
				}
			}
			else
			{
				pipeline_ = nullptr;
			}
		}

		void
		RenderPipeline::updateParameters() noexcept
		{
			auto begin = descriptorSet_->getUniformSets().begin();
			auto end = descriptorSet_->getUniformSets().end();

			for (auto& prop : material_->getMaterialParams())
			{
				auto it = std::find_if(begin, end, [&](const hal::GraphicsUniformSetPtr& set) { return set->getName() == prop.key; });
				if (it != end)
				{
					auto uniform = *it;
					switch (prop.type)
					{
					case material::PropertyTypeInfoBool:
					{
						auto value = (bool*)prop.data;
						uniform->uniform1b(*value);
					}
					break;
					case material::PropertyTypeInfoInt | material::PropertyTypeInfoBuffer:
					{
						auto value = (int*)prop.data;
						if (prop.length == 4)
							uniform->uniform1i(value[0]);
						else if (prop.length == 8)
							uniform->uniform2i(value[0], value[1]);
						else if (prop.length == 12)
							uniform->uniform3i(value[0], value[1], value[2]);
						else if (prop.length == 16)
							uniform->uniform4i(value[0], value[1], value[2], value[3]);
					}
					break;
					case material::PropertyTypeInfoFloat | material::PropertyTypeInfoBuffer:
					{
						auto value = (float*)prop.data;
						if (prop.length == 4)
							uniform->uniform1f(value[0]);
						else if (prop.length == 8)
							uniform->uniform2f(value[0], value[1]);
						else if (prop.length == 12)
							uniform->uniform3f(value[0], value[1], value[2]);
						else if (prop.length == 16)
							uniform->uniform4f(value[0], value[1], value[2], value[3]);
					}
					break;
					case material::PropertyTypeInfoTexture:
					{
						uniform->uniformTexture(prop.texture);
					}
					break;
					default:
						break;
					}
				}
			}
		}

		std::shared_ptr<RenderPipeline>
		RenderPipeline::clone() const noexcept
		{
			auto instance = std::make_shared<RenderPipeline>();
			instance->setMaterial(this->getMaterial());

			return instance;
		}
	}
}