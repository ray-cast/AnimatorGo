#include <octoon/video/render_pipeline.h>
#include <octoon/video/renderer.h>
#include <octoon/hal/graphics.h>

namespace octoon::video
{
	RenderPipeline::RenderPipeline() noexcept
	{
	}

	RenderPipeline::RenderPipeline(const material::MaterialPtr& material, const RenderContext& context) noexcept
	{
		this->setMaterial(material, context);
	}

	RenderPipeline::~RenderPipeline() noexcept
	{
	}

	void
	RenderPipeline::setMaterial(const material::MaterialPtr& material, const RenderContext& context) noexcept
	{
		if (this->material_ != material)
		{
			this->material_ = material;
			this->updateMaterial(this->material_, context);
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
	RenderPipeline::update(const camera::Camera& camera, const geometry::Geometry& geometry) noexcept
	{
		if (this->material_)
		{
			this->setTransform(geometry.getTransform());
			this->setViewProjection(camera.getViewProjection());
			this->updateParameters();
		}
	}

	void
	RenderPipeline::updateMaterial(const material::MaterialPtr& material, const RenderContext& context) noexcept(false)
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
			auto renderState = Renderer::instance()->createRenderState(stateDesc);

			auto shader = material->getShader();

			hal::GraphicsProgramDesc programDesc;
			programDesc.addShader(Renderer::instance()->createShader(hal::GraphicsShaderDesc(hal::GraphicsShaderStageFlagBits::VertexBit, shader->vs, "main", hal::GraphicsShaderLang::GLSL)));
			programDesc.addShader(Renderer::instance()->createShader(hal::GraphicsShaderDesc(hal::GraphicsShaderStageFlagBits::FragmentBit, shader->fs, "main", hal::GraphicsShaderLang::GLSL)));
			auto program = Renderer::instance()->createProgram(programDesc);

			hal::GraphicsInputLayoutDesc layoutDesc;
			layoutDesc.addVertexLayout(hal::GraphicsVertexLayout(0, "POSITION", 0, hal::GraphicsFormat::R32G32B32SFloat));
			layoutDesc.addVertexLayout(hal::GraphicsVertexLayout(0, "TEXCOORD", 0, hal::GraphicsFormat::R32G32SFloat));
			layoutDesc.addVertexBinding(hal::GraphicsVertexBinding(0, layoutDesc.getVertexSize()));

			hal::GraphicsDescriptorSetLayoutDesc descriptor_set_layout;
			descriptor_set_layout.setUniformComponents(program->getActiveParams());

			hal::GraphicsPipelineDesc pipeline;
			pipeline.setGraphicsInputLayout(Renderer::instance()->createInputLayout(layoutDesc));
			pipeline.setGraphicsState(renderState);
			pipeline.setGraphicsProgram(std::move(program));
			pipeline.setGraphicsDescriptorSetLayout(Renderer::instance()->createDescriptorSetLayout(descriptor_set_layout));

			pipeline_ = Renderer::instance()->createRenderPipeline(pipeline);
			if (pipeline_)
			{
				hal::GraphicsDescriptorSetDesc descriptorSet;
				descriptorSet.setGraphicsDescriptorSetLayout(pipeline.getDescriptorSetLayout());
				descriptorSet_ = Renderer::instance()->createDescriptorSet(descriptorSet);
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
		if (this->material_->needUpdate())
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

			this->material_->needUpdate(false);
		}
	}
}