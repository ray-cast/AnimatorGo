#include "ogl_core_pipeline.h"
#include "ogl_core_descriptor.h"
#include "ogl_core_graphics_data.h"
#include "ogl_input_layout.h"
#include "ogl_state.h"
#include "ogl_shader.h"
#include "ogl_descriptor_set.h"
#include "ogl_device.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(OGLCorePipeline, GraphicsPipeline, "OGLCorePipeline")

		OGLCorePipeline::OGLCorePipeline() noexcept
		{
		}

		OGLCorePipeline::~OGLCorePipeline() noexcept
		{
			this->close();
		}

		bool
		OGLCorePipeline::setup(const GraphicsPipelineDesc& pipelineDesc) noexcept
		{
			assert(pipelineDesc.getGraphicsState());
			assert(pipelineDesc.getGraphicsProgram());
			assert(pipelineDesc.getGraphicsInputLayout());
			assert(pipelineDesc.getGraphicsDescriptorSetLayout());
			assert(pipelineDesc.getGraphicsState()->is_instance_of<OGLGraphicsState>());
			assert(pipelineDesc.getGraphicsProgram()->is_instance_of<OGLProgram>());
			assert(pipelineDesc.getGraphicsInputLayout()->is_instance_of<OGLInputLayout>());
			assert(pipelineDesc.getGraphicsDescriptorSetLayout()->is_instance_of<OGLDescriptorSetLayout>());

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
						attribIndex = attrib->downcast<OGLGraphicsAttribute>()->getBindingPoint();
						break;
					}
				}

				if (attribIndex != GL_INVALID_INDEX)
				{
					GLenum type = OGLTypes::asVertexFormat(it.getVertexFormat());
					if (type == GL_INVALID_ENUM)
					{
						this->getDevice()->downcast<OGLDevice>()->message("Undefined vertex format.");
						return false;
					}

					VertexAttrib attrib;
					attrib.type = type;
					attrib.index = attribIndex;
					attrib.slot = it.getVertexSlot();
					attrib.count = it.getVertexCount();
					attrib.offset = offset + it.getVertexOffset();
					attrib.normalize = OGLTypes::isNormFormat(it.getVertexFormat());

					_attributes.push_back(attrib);
				}

				offset += it.getVertexOffset() + it.getVertexSize();
			}

			auto& bindings = pipelineDesc.getGraphicsInputLayout()->getGraphicsInputLayoutDesc().getVertexBindings();
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
		OGLCorePipeline::close() noexcept
		{
		}

		const GraphicsPipelineDesc&
		OGLCorePipeline::getGraphicsPipelineDesc() const noexcept
		{
			return _pipelineDesc;
		}

		void
		OGLCorePipeline::apply() noexcept
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
		OGLCorePipeline::bindVertexBuffers(OGLCoreVertexBuffers& vbos, bool forceUpdate) noexcept
		{
			if (GLEW_NV_vertex_buffer_unified_memory)
			{
				for (auto& it : _bindings)
				{
					if (!vbos[it.slot].vbo)
						continue;

					if (vbos[it.slot].needUpdate)
					{
						GLuint64 addr = vbos[it.slot].vbo->getInstanceAddr() + vbos[it.slot].offset;
						GLsizeiptr size = vbos[it.slot].vbo->getGraphicsDataDesc().getStreamSize() - vbos[it.slot].offset;
						glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, it.slot, addr, size);
						vbos[it.slot].needUpdate = false;
					}
				}
			}
			else
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
		}

		void
		OGLCorePipeline::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		OGLCorePipeline::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}