#include "ogl_pipeline.h"
#include "ogl_descriptor_set.h"
#include "ogl_graphics_data.h"
#include "ogl_input_layout.h"
#include "ogl_state.h"
#include "ogl_shader.h"
#include "ogl_descriptor_set.h"
#include "ogl_device.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(OGLPipeline, GraphicsPipeline, "OGLPipeline")

		OGLPipeline::OGLPipeline() noexcept
		{
		}

		OGLPipeline::~OGLPipeline() noexcept
		{
			this->close();
		}

		bool
		OGLPipeline::setup(const GraphicsPipelineDesc& pipelineDesc) noexcept
		{
			assert(pipelineDesc.getGraphicsState());
			assert(pipelineDesc.getGraphicsProgram());
			assert(pipelineDesc.getGraphicsInputLayout());
			assert(pipelineDesc.getGraphicsDescriptorSetLayout());
			assert(pipelineDesc.getGraphicsState()->isInstanceOf<OGLGraphicsState>());
			assert(pipelineDesc.getGraphicsProgram()->isInstanceOf<OGLProgram>());
			assert(pipelineDesc.getGraphicsInputLayout()->isInstanceOf<OGLInputLayout>());
			assert(pipelineDesc.getGraphicsDescriptorSetLayout()->isInstanceOf<OGLDescriptorSetLayout>());

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
					attrib.count = it.getVertexCount();
					attrib.stride = 0;
					attrib.offset = offset + it.getVertexOffset();
					attrib.normalize = OGLTypes::isNormFormat(it.getVertexFormat());

					if (it.getVertexSlot() <= _attributes.size())
						_attributes.resize(it.getVertexSlot() + 1);

					_attributes[it.getVertexSlot()].push_back(attrib);
				}

				offset += it.getVertexOffset() + it.getVertexSize();
			}

			auto& bindings = pipelineDesc.getGraphicsInputLayout()->getGraphicsInputLayoutDesc().getVertexBindings();
			for (auto& it : bindings)
			{
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
		OGLPipeline::close() noexcept
		{
			_attributes.clear();
		}

		const GraphicsPipelineDesc&
		OGLPipeline::getGraphicsPipelineDesc() const noexcept
		{
			return _pipelineDesc;
		}

		void
		OGLPipeline::apply() noexcept
		{
			for (auto& it : _bindings)
			{
				glVertexAttribDivisor(it.index, it.divisor);
			}
		}

		void
		OGLPipeline::bindVertexBuffers(OGLVertexBuffers& vbos, bool forceUpdate) noexcept
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

		void
		OGLPipeline::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		OGLPipeline::getDevice() noexcept
		{
			return _device.lock();
		}
}
}