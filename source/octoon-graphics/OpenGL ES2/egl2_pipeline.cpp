#include "egl2_pipeline.h"
#include "egl2_state.h"
#include "egl2_shader.h"
#include "egl2_input_layout.h"
#include "egl2_descriptor.h"
#include "egl2_graphics_data.h"

namespace octoon 
{
	namespace graphics
	{
		OctoonImplementSubClass(EGL2Pipeline, GraphicsPipeline, "EGL2Pipeline")

		EGL2Pipeline::EGL2Pipeline() noexcept
		{
		}

		EGL2Pipeline::~EGL2Pipeline() noexcept
		{
			this->close();
		}

		bool
		EGL2Pipeline::setup(const GraphicsPipelineDesc& pipelineDesc) noexcept
		{
			assert(pipelineDesc.getGraphicsState());
			assert(pipelineDesc.getGraphicsProgram());
			assert(pipelineDesc.getGraphicsInputLayout());
			assert(pipelineDesc.getGraphicsDescriptorSetLayout());
			assert(pipelineDesc.getGraphicsState()->isInstanceOf<EGL2GraphicsState>());
			assert(pipelineDesc.getGraphicsProgram()->isInstanceOf<EGL2Program>());
			assert(pipelineDesc.getGraphicsInputLayout()->isInstanceOf<EGL2InputLayout>());
			assert(pipelineDesc.getGraphicsDescriptorSetLayout()->isInstanceOf<EGL2DescriptorSetLayout>());

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
						attribIndex = attrib->downcast<EGL2GraphicsAttribute>()->getBindingPoint();
						break;
					}
				}

				if (attribIndex != GL_INVALID_INDEX)
				{
					GLenum type = EGL2Types::asVertexFormat(it.getVertexFormat());
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
					attrib.normalize = EGL2Types::isNormFormat(it.getVertexFormat());

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
		EGL2Pipeline::close() noexcept
		{
			_attributes.clear();
		}

		void
		EGL2Pipeline::apply() noexcept
		{
		}

		void
		EGL2Pipeline::bindVertexBuffers(EGL2VertexBuffers& vbos, bool forceUpdate) noexcept
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
		EGL2Pipeline::getGraphicsPipelineDesc() const noexcept
		{
			return _pipelineDesc;
		}

		void
		EGL2Pipeline::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		EGL2Pipeline::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}
