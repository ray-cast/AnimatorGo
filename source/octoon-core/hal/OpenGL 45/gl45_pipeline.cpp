#include "gl45_pipeline.h"
#include "gl45_descriptor.h"
#include "gl45_graphics_data.h"
#include "gl33_input_layout.h"
#include "gl33_state.h"
#include "gl33_shader.h"
#include "gl33_descriptor_set.h"
#include "gl33_device.h"

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL45Pipeline, GraphicsPipeline, "GL45Pipeline")

		GL45Pipeline::GL45Pipeline() noexcept
		{
		}

		GL45Pipeline::~GL45Pipeline() noexcept
		{
			this->close();
		}

		bool
		GL45Pipeline::setup(const GraphicsPipelineDesc& pipelineDesc) noexcept
		{
			assert(pipelineDesc.getGraphicsState());
			assert(pipelineDesc.getGraphicsProgram());
			assert(pipelineDesc.getInputLayout());
			assert(pipelineDesc.getDescriptorSetLayout());
			assert(pipelineDesc.getGraphicsState()->isInstanceOf<GL33GraphicsState>());
			assert(pipelineDesc.getGraphicsProgram()->isInstanceOf<GL33Program>());
			assert(pipelineDesc.getInputLayout()->isInstanceOf<GL33InputLayout>());
			assert(pipelineDesc.getDescriptorSetLayout()->isInstanceOf<GL33DescriptorSetLayout>());

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
					attrib.slot = it.getVertexSlot();
					attrib.count = it.getVertexCount();
					attrib.offset = offset + it.getVertexOffset();
					attrib.normalize = GL33Types::isNormFormat(it.getVertexFormat());

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
		GL45Pipeline::close() noexcept
		{
		}

		const GraphicsPipelineDesc&
		GL45Pipeline::getPipelineDesc() const noexcept
		{
			return _pipelineDesc;
		}

		void
		GL45Pipeline::apply() noexcept
		{
			for (auto& it : _attributes)
			{
				glEnableVertexAttribArray(it.index);
				glVertexAttribBinding(it.index, it.slot);
				glVertexAttribFormat(it.index, it.count, it.type, it.normalize, it.offset);
			}

			if (GLEW_NV_vertex_buffer_unified_memory)
			{
				for (auto& it : _bindings)
				{
					glVertexBindingDivisor(it.slot, it.divisor);
					glBindVertexBuffer(it.slot, 0, 0, _bindings[it.slot].stride);
				}
			}
			else
			{
				for (auto& it : _bindings)
				{
					glVertexBindingDivisor(it.slot, it.divisor);
				}
			}
		}

		void
		GL45Pipeline::bindVertexBuffers(GL45VertexBuffers& vbos, bool forceUpdate) noexcept
		{
			if (GLEW_NV_vertex_buffer_unified_memory)
			{
				for (auto& it : _bindings)
				{
					if (!vbos[it.slot].vbo)
						continue;

					if (vbos[it.slot].setDirty)
					{
						GLuint64 addr = vbos[it.slot].vbo->getInstanceAddr() + vbos[it.slot].offset;
						GLsizeiptr size = vbos[it.slot].vbo->getDataDesc().getStreamSize() - vbos[it.slot].offset;
						glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, it.slot, addr, size);
						vbos[it.slot].setDirty = false;
					}
				}
			}
			else
			{
				for (auto& it : _bindings)
				{
					if (!vbos[it.slot].vbo)
						continue;

					if (vbos[it.slot].setDirty || forceUpdate)
					{
						glBindVertexBuffer(it.slot, vbos[it.slot].vbo->getInstanceID(), vbos[it.slot].offset, _bindings[it.slot].stride);
						vbos[it.slot].setDirty = false;
					}
				}
			}
		}

		void
		GL45Pipeline::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL45Pipeline::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}