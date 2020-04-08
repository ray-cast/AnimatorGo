#include "gl20_pipeline.h"
#include "gl20_state.h"
#include "gl20_shader.h"
#include "gl20_input_layout.h"
#include "gl20_descriptor_set.h"
#include "gl20_graphics_data.h"

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL20Pipeline, GraphicsPipeline, "GL20Pipeline")

		GL20Pipeline::GL20Pipeline() noexcept
		{
		}

		GL20Pipeline::~GL20Pipeline() noexcept
		{
			this->close();
		}

		bool
		GL20Pipeline::setup(const GraphicsPipelineDesc& pipelineDesc) noexcept
		{
			assert(pipelineDesc.getGraphicsState());
			assert(pipelineDesc.getGraphicsProgram());
			assert(pipelineDesc.getInputLayout());
			assert(pipelineDesc.getDescriptorSetLayout());
			assert(pipelineDesc.getGraphicsState()->isInstanceOf<GL20GraphicsState>());
			assert(pipelineDesc.getGraphicsProgram()->isInstanceOf<GL20Program>());
			assert(pipelineDesc.getInputLayout()->isInstanceOf<GL20InputLayout>());
			assert(pipelineDesc.getDescriptorSetLayout()->isInstanceOf<GL20DescriptorSetLayout>());

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
						attribIndex = attrib->downcast<GL20GraphicsAttribute>()->getBindingPoint();
						break;
					}
				}

				if (attribIndex != GL_INVALID_INDEX)
				{
					GLenum type = GL20Types::asVertexFormat(it.getVertexFormat());
					if (type == GL_INVALID_ENUM)
					{
						GL_PLATFORM_LOG("Undefined vertex format.");
						return false;
					}

					VertexAttrib attrib;
					attrib.type = type;
					attrib.index = attribIndex;
					attrib.count = it.getVertexCount();
					attrib.stride = 0;
					attrib.offset = offset + it.getVertexOffset();
					attrib.normalize = GL20Types::isNormFormat(it.getVertexFormat());
					attrib.size = pipelineDesc.getInputLayout()->getInputLayoutDesc().getVertexSize((std::uint8_t)it.getVertexSlot());

					if (it.getVertexSlot() <= _attributes.size())
						_attributes.resize(it.getVertexSlot() + 1);

					_attributes[it.getVertexSlot()].push_back(attrib);
				}

				offset += it.getVertexOffset() + it.getVertexSize();
			}

			auto& bindings = pipelineDesc.getInputLayout()->getInputLayoutDesc().getVertexBindings();
			for (auto& it : bindings)
			{
				if (it.getVertexDivisor() != GraphicsVertexDivisor::Vertex)
				{
					GL_PLATFORM_LOG("Only support vertex divisor.");
					return false;
				}

				for (auto& attrib : _attributes[it.getVertexSlot()])
				{
					attrib.stride = it.getVertexSize();

					VertexBinding binding;
					binding.index = attrib.index;
					binding.slot = it.getVertexSlot();

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
			}

			_pipelineDesc = pipelineDesc;
			return true;
		}

		void
		GL20Pipeline::close() noexcept
		{
			_attributes.clear();
		}

		void
		GL20Pipeline::apply() noexcept
		{
		}

		void
		GL20Pipeline::bindVertexBuffers(GL20VertexBuffers& vbos, bool forceUpdate, std::uint32_t startVertices) noexcept
		{
			for (std::size_t slot = 0; slot < _attributes.size(); slot++)
			{
				if (!vbos[slot].vbo)
					continue;

				if (vbos[slot].setDirty || forceUpdate)
				{
					glBindBuffer(GL_ARRAY_BUFFER, vbos[slot].vbo->getInstanceID());

					for (auto& it : _attributes[slot])
					{
						auto offset = (GLbyte*)nullptr + vbos[slot].offset + it.offset + startVertices * it.size;

						glEnableVertexAttribArray(it.index);
						glVertexAttribPointer(it.index, it.count, it.type, it.normalize, it.stride, offset);
					}

					vbos[slot].setDirty = false;
				}
			}
		}

		const GraphicsPipelineDesc&
		GL20Pipeline::getPipelineDesc() const noexcept
		{
			return _pipelineDesc;
		}

		void
		GL20Pipeline::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL20Pipeline::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}