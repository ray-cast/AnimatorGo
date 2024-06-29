#include "gl45_descriptor.h"
#include "gl45_texture.h"
#include "gl45_graphics_data.h"
#include "gl33_descriptor_set.h"
#include "gl33_shader.h"
#include "gl33_sampler.h"

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL45DescriptorSet, GraphicsDescriptorSet, "GL45DescriptorSet")

		GL45DescriptorSet::GL45DescriptorSet() noexcept
		{
		}

		GL45DescriptorSet::~GL45DescriptorSet() noexcept
		{
			this->close();
		}

		bool
		GL45DescriptorSet::setup(const GraphicsDescriptorSetDesc& descriptorSetDesc) noexcept
		{
			assert(descriptorSetDesc.getDescriptorSetLayout());

			auto& descriptorSetLayoutDesc = descriptorSetDesc.getDescriptorSetLayout()->getDescriptorSetLayoutDesc();

			auto& params = descriptorSetLayoutDesc.getUniformComponents();
			for (auto& uniform : params)
			{
				auto uniformSet = std::make_shared<GL33GraphicsUniformSet>();
				uniformSet->setGraphicsParam(uniform);
				_activeUniformSets.push_back(uniformSet);
			}

			_descriptorSetDesc = descriptorSetDesc;
			return true;
		}

		void
		GL45DescriptorSet::close() noexcept
		{
			_activeUniformSets.clear();
		}

		void
		GL45DescriptorSet::apply(const GL33Program& shaderObject) noexcept
		{
			auto program = shaderObject.getInstanceID();
			for (auto& it : _activeUniformSets)
			{
				auto type = it->getGraphicsParam()->getType();
				auto location = it->getGraphicsParam()->getBindingPoint();
				switch (type)
				{
				case UniformAttributeFormat::Boolean:
					glProgramUniform1i(program, location, it->getBool());
					break;
				case UniformAttributeFormat::Int:
					glProgramUniform1i(program, location, it->getInt());
					break;
				case UniformAttributeFormat::Int2:
					glProgramUniform2iv(program, location, 1, (GLint*)it->getInt2().ptr());
					break;
				case UniformAttributeFormat::Int3:
					glProgramUniform3iv(program, location, 1, (GLint*)it->getInt3().ptr());
					break;
				case UniformAttributeFormat::Int4:
					glProgramUniform4iv(program, location, 1, (GLint*)it->getInt4().ptr());
					break;
				case UniformAttributeFormat::UInt:
					glProgramUniform1ui(program, location, it->getUInt());
					break;
				case UniformAttributeFormat::UInt2:
					glProgramUniform2uiv(program, location, 1, (GLuint*)it->getUInt2().ptr());
					break;
				case UniformAttributeFormat::UInt3:
					glProgramUniform3uiv(program, location, 1, (GLuint*)it->getUInt3().ptr());
					break;
				case UniformAttributeFormat::UInt4:
					glProgramUniform4uiv(program, location, 1, (GLuint*)it->getUInt4().ptr());
					break;
				case UniformAttributeFormat::Float:
					glProgramUniform1f(program, location, it->getFloat());
					break;
				case UniformAttributeFormat::Float2:
					glProgramUniform2fv(program, location, 1, it->getFloat2().ptr());
					break;
				case UniformAttributeFormat::Float3:
					glProgramUniform3fv(program, location, 1, it->getFloat3().ptr());
					break;
				case UniformAttributeFormat::Float4:
					glProgramUniform4fv(program, location, 1, it->getFloat4().ptr());
					break;
				case UniformAttributeFormat::Float3x3:
					glProgramUniformMatrix3fv(program, location, 1, GL_FALSE, it->getFloat3x3().ptr());
					break;
				case UniformAttributeFormat::Float4x4:
					glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, it->getFloat4x4().ptr());
					break;
				case UniformAttributeFormat::IntArray:
					glProgramUniform1iv(program, location, static_cast<GLsizei>(it->getIntArray().size()), it->getIntArray().data());
					break;
				case UniformAttributeFormat::Int2Array:
					glProgramUniform2iv(program, location, static_cast<GLsizei>(it->getInt2Array().size()), (GLint*)it->getInt2Array().data());
					break;
				case UniformAttributeFormat::Int3Array:
					glProgramUniform3iv(program, location, static_cast<GLsizei>(it->getInt3Array().size()), (GLint*)it->getInt3Array().data());
					break;
				case UniformAttributeFormat::Int4Array:
					glProgramUniform4iv(program, location, static_cast<GLsizei>(it->getInt4Array().size()), (GLint*)it->getInt4Array().data());
					break;
				case UniformAttributeFormat::UIntArray:
					glProgramUniform1uiv(program, location, static_cast<GLsizei>(it->getUIntArray().size()), it->getUIntArray().data());
					break;
				case UniformAttributeFormat::UInt2Array:
					glProgramUniform2uiv(program, location, static_cast<GLsizei>(it->getUInt2Array().size()), (GLuint*)it->getUInt2Array().data());
					break;
				case UniformAttributeFormat::UInt3Array:
					glProgramUniform3uiv(program, location, static_cast<GLsizei>(it->getUInt3Array().size()), (GLuint*)it->getUInt3Array().data());
					break;
				case UniformAttributeFormat::UInt4Array:
					glProgramUniform4uiv(program, location, static_cast<GLsizei>(it->getUInt4Array().size()), (GLuint*)it->getUInt4Array().data());
					break;
				case UniformAttributeFormat::FloatArray:
					glProgramUniform1fv(program, location, static_cast<GLsizei>(it->getFloatArray().size()), (GLfloat*)it->getFloatArray().data());
					break;
				case UniformAttributeFormat::Float2Array:
					glProgramUniform2fv(program, location, static_cast<GLsizei>(it->getFloat2Array().size()), (GLfloat*)it->getFloat2Array().data());
					break;
				case UniformAttributeFormat::Float3Array:
					glProgramUniform3fv(program, location, static_cast<GLsizei>(it->getFloat3Array().size()), (GLfloat*)it->getFloat3Array().data());
					break;
				case UniformAttributeFormat::Float4Array:
					glProgramUniform4fv(program, location, static_cast<GLsizei>(it->getFloat4Array().size()), (GLfloat*)it->getFloat4Array().data());
					break;
				case UniformAttributeFormat::Float3x3Array:
					glProgramUniformMatrix3fv(program, location, static_cast<GLsizei>(it->getFloat3x3Array().size()), GL_FALSE, (GLfloat*)it->getFloat3x3Array().data());
					break;
				case UniformAttributeFormat::Float4x4Array:
					glProgramUniformMatrix4fv(program, location, static_cast<GLsizei>(it->getFloat4x4Array().size()), GL_FALSE, (GLfloat*)it->getFloat4x4Array().data());
					break;
				case UniformAttributeFormat::Sampler:
					glBindSampler(location, it->getTextureSampler()->downcast<GL33Sampler>()->getInstanceID());
					break;
				case UniformAttributeFormat::SamplerImage:
				case UniformAttributeFormat::CombinedImageSampler:
				case UniformAttributeFormat::StorageImage:
				{
					auto& texture = it->getTexture();
					if (texture)
					{
						auto gltexture = texture->downcast<GL45Texture>();
						glBindTextureUnit(location, gltexture->getInstanceID());

						auto sampler = it->getTextureSampler();
						if (sampler)
						{
							auto instance = sampler->downcast<GL33Sampler>()->getInstanceID();
							glBindSampler(location, instance);
						}
					}
					else
					{
						glBindTextureUnit(location, GL_NONE);
					}
				}
				break;
				case UniformAttributeFormat::StorageTexelBuffer:
					break;
				case UniformAttributeFormat::StorageBuffer:
					break;
				case UniformAttributeFormat::StorageBufferDynamic:
					break;
				case UniformAttributeFormat::UniformTexelBuffer:
					break;
				case UniformAttributeFormat::UniformBuffer:
				{
					auto& buffer = it->getBuffer();
					if (buffer)
					{
						auto ubo = buffer->downcast<GL45GraphicsData>();
						glBindBufferBase(GL_UNIFORM_BUFFER, location, ubo->getInstanceID());
					}
				}
				break;
				case UniformAttributeFormat::UniformBufferDynamic:
					break;
				case UniformAttributeFormat::InputAttachment:
					break;
				default:
					break;
				}
			}
		}

		void
		GL45DescriptorSet::copy(std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept
		{
			for (std::size_t i = 0; i < descriptorCopyCount; i++)
			{
				if (!descriptorCopies[i])
					continue;

				auto descriptorCope = descriptorCopies[i]->downcast<GL45DescriptorSet>();
				for (auto& activeUniformSet : descriptorCope->_activeUniformSets)
				{
					auto it = std::find_if(_activeUniformSets.begin(), _activeUniformSets.end(), [&](GraphicsUniformSetPtr& it) { return it->getGraphicsParam() == activeUniformSet->getGraphicsParam(); });
					if (it == _activeUniformSets.end())
						continue;

					auto type = activeUniformSet->getGraphicsParam()->getType();
					switch (type)
					{
					case UniformAttributeFormat::Boolean:
						(*it)->uniform1b(activeUniformSet->getBool());
						break;
					case UniformAttributeFormat::Int:
						(*it)->uniform1i(activeUniformSet->getInt());
						break;
					case UniformAttributeFormat::Int2:
						(*it)->uniform2i(activeUniformSet->getInt2());
						break;
					case UniformAttributeFormat::Int3:
						(*it)->uniform3i(activeUniformSet->getInt3());
						break;
					case UniformAttributeFormat::Int4:
						(*it)->uniform4i(activeUniformSet->getInt4());
						break;
					case UniformAttributeFormat::UInt:
						(*it)->uniform1ui(activeUniformSet->getUInt());
						break;
					case UniformAttributeFormat::UInt2:
						(*it)->uniform2ui(activeUniformSet->getUInt2());
						break;
					case UniformAttributeFormat::UInt3:
						(*it)->uniform3ui(activeUniformSet->getUInt3());
						break;
					case UniformAttributeFormat::UInt4:
						(*it)->uniform4ui(activeUniformSet->getUInt4());
						break;
					case UniformAttributeFormat::Float:
						(*it)->uniform1f(activeUniformSet->getFloat());
						break;
					case UniformAttributeFormat::Float2:
						(*it)->uniform2f(activeUniformSet->getFloat2());
						break;
					case UniformAttributeFormat::Float3:
						(*it)->uniform3f(activeUniformSet->getFloat3());
						break;
					case UniformAttributeFormat::Float4:
						(*it)->uniform4f(activeUniformSet->getFloat4());
						break;
					case UniformAttributeFormat::Float2x2:
						(*it)->uniform2fmat(activeUniformSet->getFloat2x2());
						break;
					case UniformAttributeFormat::Float3x3:
						(*it)->uniform3fmat(activeUniformSet->getFloat3x3());
						break;
					case UniformAttributeFormat::Float4x4:
						(*it)->uniform4fmat(activeUniformSet->getFloat4x4());
						break;
					case UniformAttributeFormat::IntArray:
						(*it)->uniform1iv(activeUniformSet->getIntArray());
						break;
					case UniformAttributeFormat::Int2Array:
						(*it)->uniform2iv(activeUniformSet->getInt2Array());
						break;
					case UniformAttributeFormat::Int3Array:
						(*it)->uniform3iv(activeUniformSet->getInt3Array());
						break;
					case UniformAttributeFormat::Int4Array:
						(*it)->uniform4iv(activeUniformSet->getInt4Array());
						break;
					case UniformAttributeFormat::UIntArray:
						(*it)->uniform1uiv(activeUniformSet->getUIntArray());
						break;
					case UniformAttributeFormat::UInt2Array:
						(*it)->uniform2uiv(activeUniformSet->getUInt2Array());
						break;
					case UniformAttributeFormat::UInt3Array:
						(*it)->uniform3uiv(activeUniformSet->getUInt3Array());
						break;
					case UniformAttributeFormat::UInt4Array:
						(*it)->uniform4uiv(activeUniformSet->getUInt4Array());
						break;
					case UniformAttributeFormat::FloatArray:
						(*it)->uniform1fv(activeUniformSet->getFloatArray());
						break;
					case UniformAttributeFormat::Float2Array:
						(*it)->uniform2fv(activeUniformSet->getFloat2Array());
						break;
					case UniformAttributeFormat::Float3Array:
						(*it)->uniform3fv(activeUniformSet->getFloat3Array());
						break;
					case UniformAttributeFormat::Float4Array:
						(*it)->uniform4fv(activeUniformSet->getFloat4Array());
						break;
					case UniformAttributeFormat::Float2x2Array:
						(*it)->uniform2fmatv(activeUniformSet->getFloat2x2Array());
						break;
					case UniformAttributeFormat::Float3x3Array:
						(*it)->uniform3fmatv(activeUniformSet->getFloat3x3Array());
						break;
					case UniformAttributeFormat::Float4x4Array:
						(*it)->uniform4fmatv(activeUniformSet->getFloat4x4Array());
						break;
					case UniformAttributeFormat::Sampler:
						(*it)->uniformTexture(activeUniformSet->getTexture(), activeUniformSet->getTextureSampler());
						break;
					case UniformAttributeFormat::SamplerImage:
						(*it)->uniformTexture(activeUniformSet->getTexture(), activeUniformSet->getTextureSampler());
						break;
					case UniformAttributeFormat::CombinedImageSampler:
						(*it)->uniformTexture(activeUniformSet->getTexture(), activeUniformSet->getTextureSampler());
						break;
					case UniformAttributeFormat::StorageImage:
						(*it)->uniformTexture(activeUniformSet->getTexture(), activeUniformSet->getTextureSampler());
						break;
					case UniformAttributeFormat::StorageTexelBuffer:
						break;
					case UniformAttributeFormat::StorageBuffer:
						break;
					case UniformAttributeFormat::StorageBufferDynamic:
						break;
					case UniformAttributeFormat::UniformTexelBuffer:
						break;
					case UniformAttributeFormat::UniformBuffer:
						(*it)->uniformBuffer(activeUniformSet->getBuffer());
						break;
					case UniformAttributeFormat::UniformBufferDynamic:
						break;
					case UniformAttributeFormat::InputAttachment:
						break;
					default:
						break;
					}
				}
			}
		}

		const GraphicsUniformSets&
		GL45DescriptorSet::getUniformSets() const noexcept
		{
			return _activeUniformSets;
		}

		const GraphicsDescriptorSetDesc&
		GL45DescriptorSet::getDescriptorSetDesc() const noexcept
		{
			return _descriptorSetDesc;
		}

		void
		GL45DescriptorSet::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL45DescriptorSet::getDevice() const noexcept
		{
			return _device.lock();
		}
	}
}