#include "gl33_pipeline.h"
#include "gl33_descriptor_set.h"
#include "gl33_graphics_data.h"
#include "gl33_input_layout.h"
#include "gl33_state.h"
#include "gl33_shader.h"
#include "gl33_descriptor_set.h"
#include "gl33_device.h"

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL33Pipeline, GraphicsPipeline, "GL33Pipeline")

		GL33Pipeline::GL33Pipeline() noexcept
		{
		}

		GL33Pipeline::~GL33Pipeline() noexcept
		{
			this->close();
		}

		bool
		GL33Pipeline::setup(const GraphicsPipelineDesc& pipelineDesc) noexcept
		{
			assert(pipelineDesc.getGraphicsState());
			assert(pipelineDesc.getProgram());
			assert(pipelineDesc.getInputLayout());
			assert(pipelineDesc.getDescriptorSetLayout());
			assert(pipelineDesc.getGraphicsState()->isInstanceOf<GL33GraphicsState>());
			assert(pipelineDesc.getProgram()->isInstanceOf<GL33Program>());
			assert(pipelineDesc.getInputLayout()->isInstanceOf<GL33InputLayout>());
			assert(pipelineDesc.getDescriptorSetLayout()->isInstanceOf<GL33DescriptorSetLayout>());

			std::uint16_t offset = 0;

			auto& layouts = pipelineDesc.getInputLayout()->getInputLayoutDesc().getVertexLayouts();
			for (auto& it : layouts)
			{
				GLuint attribIndex = GL_INVALID_INDEX;

				auto& attributes = pipelineDesc.getProgram()->getActiveAttributes();
				for (auto& attrib : attributes)
				{
					if (attrib->getSemantic() == it.getSemantic() &&
						attrib->getSemanticIndex() == it.getSemanticIndex())
					{
						attribIndex = attrib->downcast<GL33GraphicsAttribute>()->getBindingPoint();
						break;
					}
				}

				if (attribIndex != GL_INVALID_INDEX)
				{
					GLenum type = GL33Types::asVertexFormat(it.getVertexFormat());
					if (type == GL_INVALID_ENUM)
					{
						this->getDevice()->downcast<GL33Device>()->message("Undefined vertex format.");
						return false;
					}

					VertexAttrib attrib;
					attrib.type = type;
					attrib.index = attribIndex;
					attrib.count = it.getVertexCount();
					attrib.stride = 0;
					attrib.offset = offset + it.getVertexOffset();
					attrib.normalize = GL33Types::isNormFormat(it.getVertexFormat());

					if (it.getVertexSlot() <= _attributes.size())
						_attributes.resize(it.getVertexSlot() + 1);

					_attributes[it.getVertexSlot()].push_back(attrib);
				}

				offset += it.getVertexOffset() + it.getVertexSize();
			}

			auto& bindings = pipelineDesc.getInputLayout()->getInputLayoutDesc().getVertexBindings();
			for (auto& it : bindings)
			{
				for (auto& attrib : _attributes[it.getVertexSlot()])
				{
					attrib.stride = it.getVertexSize();

					VertexBinding binding;
					binding.index = attrib.index;
					binding.slot = it.getVertexSlot();

					auto divisor = it.getVertexDivisor();
					if (divisor == VertexAttribDivisor::Vertex)
						binding.divisor = 0;
					else if (divisor == VertexAttribDivisor::Instance)
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
		GL33Pipeline::close() noexcept
		{
			_attributes.clear();
		}

		const GraphicsPipelineDesc&
		GL33Pipeline::getPipelineDesc() const noexcept
		{
			return _pipelineDesc;
		}

		void
		GL33Pipeline::apply() noexcept
		{
			if (glVertexAttribDivisor)
			{
				for (auto& it : _bindings)
					glVertexAttribDivisor(it.index, it.divisor);
			}
			else
			{
				for (auto& it : _bindings)
				{
					assert(it.divisor == 0);
				}
			}
		}

		void
		GL33Pipeline::bindVertexBuffers(GL33VertexBuffers& vbos, bool forceUpdate) noexcept
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
						glEnableVertexAttribArray(it.index);
						glVertexAttribPointer(it.index, it.count, it.type, it.normalize, it.stride, (GLbyte*)nullptr + vbos[slot].offset + it.offset);
					}

					vbos[slot].setDirty = false;
				}
			}
		}

		void
		GL33Pipeline::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL33Pipeline::getDevice() const noexcept
		{
			return _device.lock();
		}
}
}