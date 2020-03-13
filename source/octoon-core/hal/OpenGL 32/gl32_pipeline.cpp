#include "gl32_pipeline.h"
#include "gl32_state.h"
#include "gl32_shader.h"
#include "gl32_input_layout.h"
#include "gl32_descriptor_set.h"
#include "gl32_graphics_data.h"

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL32Pipeline, GraphicsPipeline, "GL32Pipeline")

		GL32Pipeline::GL32Pipeline() noexcept
		{
		}

		GL32Pipeline::~GL32Pipeline() noexcept
		{
			this->close();
		}

		bool
		GL32Pipeline::setup(const GraphicsPipelineDesc& pipelineDesc) noexcept
		{
			assert(pipelineDesc.getGraphicsState());
			assert(pipelineDesc.getGraphicsProgram());
			assert(pipelineDesc.getInputLayout());
			assert(pipelineDesc.getDescriptorSetLayout());
			assert(pipelineDesc.getGraphicsState()->isInstanceOf<GL32GraphicsState>());
			assert(pipelineDesc.getGraphicsProgram()->isInstanceOf<GL32Program>());
			assert(pipelineDesc.getInputLayout()->isInstanceOf<GL32InputLayout>());
			assert(pipelineDesc.getDescriptorSetLayout()->isInstanceOf<GL32DescriptorSetLayout>());

			std::uint16_t offset = 0;

			auto& layouts = pipelineDesc.getInputLayout()->getInputLayoutDesc().getVertexLayouts();
			for (auto& it : layouts)
			{
				GLuint attribIndex = GL_INVALID_INDEX;

				auto& attributes = pipelineDesc.getGraphicsProgram()->getActiveAttributes();
				for (auto& attrib : attributes)
				{
					if (attrib->getSemantic() == it.getSemantic() &&
						attrib->getSemanticIndex() == it.getSemanticIndex())
					{
						attribIndex = attrib->downcast<GL32GraphicsAttribute>()->getBindingPoint();
						break;
					}
				}

				if (attribIndex != GL_INVALID_INDEX)
				{
					GLenum type = GL32Types::asVertexFormat(it.getVertexFormat());
					if (type == GL_INVALID_ENUM)
					{
						GL_PLATFORM_LOG("Undefined vertex format.");
						return false;
					}

					VertexAttrib attrib;
					attrib.type = type;
					attrib.index = attribIndex;
					attrib.slot = it.getVertexSlot();
					attrib.count = it.getVertexCount();
					attrib.offset = offset + it.getVertexOffset();
					attrib.normalize = GL32Types::isNormFormat(it.getVertexFormat());

					_attributes.push_back(attrib);
				}

				offset += it.getVertexOffset() + it.getVertexSize();
			}

			auto& bindings = pipelineDesc.getInputLayout()->getInputLayoutDesc().getVertexBindings();
			for (auto& it : bindings)
			{
				VertexBinding binding;
				binding.slot = it.getVertexSlot();
				binding.stride = it.getVertexSize();

				auto divisor = it.getVertexDivisor();
				if (divisor == GraphicsVertexDivisor::Vertex)
					binding.divisor = 0;
				else if (divisor == GraphicsVertexDivisor::Instance)
					binding.divisor = 1;
				else
				{
					assert(false);
					binding.divisor = 0;
				}

				_bindings.push_back(binding);
			}

			_pipelineDesc = pipelineDesc;
			return true;
		}

		void
		GL32Pipeline::close() noexcept
		{
		}

		void
		GL32Pipeline::apply() noexcept
		{
			for (auto& it : _attributes)
			{
				glEnableVertexAttribArray(it.index);
				glVertexAttribBinding(it.index, it.slot);
				glVertexAttribFormat(it.index, it.count, it.type, it.normalize, it.offset);
			}

			for (auto& it : _bindings)
			{
				glVertexBindingDivisor(it.slot, it.divisor);
			}
		}

		void
		GL32Pipeline::bindVertexBuffers(GL32VertexBuffers& vbos, bool forceUpdate) noexcept
		{
			for (auto& it : _bindings)
			{
				if (!vbos[it.slot].vbo)
					continue;

				if (vbos[it.slot].needUpdate || forceUpdate)
				{
					glBindVertexBuffer(it.slot, vbos[it.slot].vbo->getInstanceID(), vbos[it.slot].offset, _bindings[it.slot].stride);
					vbos[it.slot].needUpdate = false;
				}
			}
		}

		const GraphicsPipelineDesc&
		GL32Pipeline::getPipelineDesc() const noexcept
		{
			return _pipelineDesc;
		}

		void
		GL32Pipeline::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL32Pipeline::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}