#include "gl20_pipeline.h"
#include "gl20_state.h"
#include "gl20_shader.h"
#include "gl20_input_layout.h"
#include "gl20_descriptor_set.h"
#include "gl20_graphics_data.h"

namespace octoon
{
	namespace graphics
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
			assert(pipelineDesc.getGraphicsInputLayout());
			assert(pipelineDesc.getGraphicsDescriptorSetLayout());
			assert(pipelineDesc.getGraphicsState()->isInstanceOf<GL20GraphicsState>());
			assert(pipelineDesc.getGraphicsProgram()->isInstanceOf<GL20Program>());
			assert(pipelineDesc.getGraphicsInputLayout()->isInstanceOf<GL20InputLayout>());
			assert(pipelineDesc.getGraphicsDescriptorSetLayout()->isInstanceOf<GL20DescriptorSetLayout>());

			std::uint16_t offset = 0;

			auto& layouts = pipelineDesc.getGraphicsInputLayout()->getGraphicsInputLayoutDesc().getVertexLayouts();
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

					if (it.getVertexSlot() <= _attributes.size())
						_attributes.resize(it.getVertexSlot() + 1);

					_attributes[it.getVertexSlot()].push_back(attrib);
				}

				offset += it.getVertexOffset() + it.getVertexSize();
			}

			auto& bindings = pipelineDesc.getGraphicsInputLayout()->getGraphicsInputLayoutDesc().getVertexBindings();
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
		GL20Pipeline::bindVertexBuffers(GL20VertexBuffers& vbos, bool forceUpdate) noexcept
		{
			for (std::size_t slot = 0; slot < _attributes.size(); slot++)
			{
				if (!vbos[slot].vbo)
					continue;

				if (vbos[slot].needUpdate || forceUpdate)
				{
					glBindBuffer(GL_ARRAY_BUFFER, vbos[slot].vbo->getInstanceID());

					for (auto& it : _attributes[slot])
					{
						glEnableVertexAttribArray(it.index);
						glVertexAttribPointer(it.index, it.count, it.type, it.normalize, it.stride, (GLbyte*)nullptr + vbos[slot].offset + it.offset);
					}

					vbos[slot].needUpdate = false;
				}
			}
		}

		const GraphicsPipelineDesc&
		GL20Pipeline::getGraphicsPipelineDesc() const noexcept
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