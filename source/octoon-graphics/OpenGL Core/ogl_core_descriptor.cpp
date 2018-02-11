#include "ogl_core_descriptor.h"
#include "ogl_core_texture.h"
#include "ogl_core_graphics_data.h"
#include "ogl_descriptor_set.h"
#include "ogl_shader.h"
#include "ogl_sampler.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(OGLCoreDescriptorSet, GraphicsDescriptorSet, "OGLCoreDescriptorSet")

		OGLCoreDescriptorSet::OGLCoreDescriptorSet() noexcept
		{
		}

		OGLCoreDescriptorSet::~OGLCoreDescriptorSet() noexcept
		{
			this->close();
		}

		bool
		OGLCoreDescriptorSet::setup(const GraphicsDescriptorSetDesc& descriptorSetDesc) noexcept
		{
			assert(descriptorSetDesc.getGraphicsDescriptorSetLayout());

			auto& descriptorSetLayoutDesc = descriptorSetDesc.getGraphicsDescriptorSetLayout()->getGraphicsDescriptorSetLayoutDesc();

			auto& params = descriptorSetLayoutDesc.getUniformComponents();
			for (auto& uniform : params)
			{
				auto uniformSet = std::make_shared<OGLGraphicsUniformSet>();
				uniformSet->setGraphicsParam(uniform);
				_activeUniformSets.push_back(uniformSet);
			}

			_descriptorSetDesc = descriptorSetDesc;
			return true;
		}

		void
		OGLCoreDescriptorSet::close() noexcept
		{
			_activeUniformSets.clear();
		}

		void
		OGLCoreDescriptorSet::apply(const OGLProgram& shaderObject) noexcept
		{
			auto program = shaderObject.getInstanceID();
			for (auto& it : _activeUniformSets)
			{
				auto type = it->getGraphicsParam()->getType();
				auto location = it->getGraphicsParam()->getBindingPoint();
				switch (type)
				{
				case GraphicsUniformType::GraphicsUniformTypeBool:
					glProgramUniform1i(program, location, it->getBool());
					break;
				case GraphicsUniformType::GraphicsUniformTypeInt:
					glProgramUniform1i(program, location, it->getInt());
					break;
				case GraphicsUniformType::GraphicsUniformTypeInt2:
					glProgramUniform2iv(program, location, 1, (GLint*)it->getInt2().ptr());
					break;
				case GraphicsUniformType::GraphicsUniformTypeInt3:
					glProgramUniform3iv(program, location, 1, (GLint*)it->getInt3().ptr());
					break;
				case GraphicsUniformType::GraphicsUniformTypeInt4:
					glProgramUniform4iv(program, location, 1, (GLint*)it->getInt4().ptr());
					break;
				case GraphicsUniformType::GraphicsUniformTypeUInt:
					glProgramUniform1ui(program, location, it->getUInt());
					break;
				case GraphicsUniformType::GraphicsUniformTypeUInt2:
					glProgramUniform2uiv(program, location, 1, (GLuint*)it->getUInt2().ptr());
					break;
				case GraphicsUniformType::GraphicsUniformTypeUInt3:
					glProgramUniform3uiv(program, location, 1, (GLuint*)it->getUInt3().ptr());
					break;
				case GraphicsUniformType::GraphicsUniformTypeUInt4:
					glProgramUniform4uiv(program, location, 1, (GLuint*)it->getUInt4().ptr());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat:
					glProgramUniform1f(program, location, it->getFloat());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat2:
					glProgramUniform2fv(program, location, 1, it->getFloat2().ptr());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat3:
					glProgramUniform3fv(program, location, 1, it->getFloat3().ptr());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat4:
					glProgramUniform4fv(program, location, 1, it->getFloat4().ptr());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat3x3:
					glProgramUniformMatrix3fv(program, location, 1, GL_FALSE, it->getFloat3x3().ptr());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat4x4:
					glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, it->getFloat4x4().ptr());
					break;
				case GraphicsUniformType::GraphicsUniformTypeIntArray:
					glProgramUniform1iv(program, location, static_cast<GLsizei>(it->getIntArray().size()), it->getIntArray().data());
					break;
				case GraphicsUniformType::GraphicsUniformTypeInt2Array:
					glProgramUniform2iv(program, location, static_cast<GLsizei>(it->getInt2Array().size()), (GLint*)it->getInt2Array().data());
					break;
				case GraphicsUniformType::GraphicsUniformTypeInt3Array:
					glProgramUniform3iv(program, location, static_cast<GLsizei>(it->getInt3Array().size()), (GLint*)it->getInt3Array().data());
					break;
				case GraphicsUniformType::GraphicsUniformTypeInt4Array:
					glProgramUniform4iv(program, location, static_cast<GLsizei>(it->getInt4Array().size()), (GLint*)it->getInt4Array().data());
					break;
				case GraphicsUniformType::GraphicsUniformTypeUIntArray:
					glProgramUniform1uiv(program, location, static_cast<GLsizei>(it->getUIntArray().size()), it->getUIntArray().data());
					break;
				case GraphicsUniformType::GraphicsUniformTypeUInt2Array:
					glProgramUniform2uiv(program, location, static_cast<GLsizei>(it->getUInt2Array().size()), (GLuint*)it->getUInt2Array().data());
					break;
				case GraphicsUniformType::GraphicsUniformTypeUInt3Array:
					glProgramUniform3uiv(program, location, static_cast<GLsizei>(it->getUInt3Array().size()), (GLuint*)it->getUInt3Array().data());
					break;
				case GraphicsUniformType::GraphicsUniformTypeUInt4Array:
					glProgramUniform4uiv(program, location, static_cast<GLsizei>(it->getUInt4Array().size()), (GLuint*)it->getUInt4Array().data());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloatArray:
					glProgramUniform1fv(program, location, static_cast<GLsizei>(it->getFloatArray().size()), (GLfloat*)it->getFloatArray().data());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat2Array:
					glProgramUniform2fv(program, location, static_cast<GLsizei>(it->getFloat2Array().size()), (GLfloat*)it->getFloat2Array().data());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat3Array:
					glProgramUniform3fv(program, location, static_cast<GLsizei>(it->getFloat3Array().size()), (GLfloat*)it->getFloat3Array().data());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat4Array:
					glProgramUniform4fv(program, location, static_cast<GLsizei>(it->getFloat4Array().size()), (GLfloat*)it->getFloat4Array().data());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat3x3Array:
					glProgramUniformMatrix3fv(program, location, static_cast<GLsizei>(it->getFloat3x3Array().size()), GL_FALSE, (GLfloat*)it->getFloat3x3Array().data());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat4x4Array:
					glProgramUniformMatrix4fv(program, location, static_cast<GLsizei>(it->getFloat4x4Array().size()), GL_FALSE, (GLfloat*)it->getFloat4x4Array().data());
					break;
				case GraphicsUniformType::GraphicsUniformTypeSampler:
					glBindSampler(location, it->getTextureSampler()->downcast<OGLSampler>()->getInstanceID());
					break;
				case GraphicsUniformType::GraphicsUniformTypeSamplerImage:
				case GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler:
				case GraphicsUniformType::GraphicsUniformTypeStorageImage:
				{
					auto texture = it->getTexture();
					if (texture)
						glBindTextureUnit(location, texture->downcast<OGLCoreTexture>()->getInstanceID());
					else
						glBindTextureUnit(location, GL_NONE);
				}
				break;
				case GraphicsUniformType::GraphicsUniformTypeStorageTexelBuffer:
					break;
				case GraphicsUniformType::GraphicsUniformTypeStorageBuffer:
					break;
				case GraphicsUniformType::GraphicsUniformTypeStorageBufferDynamic:
					break;
				case GraphicsUniformType::GraphicsUniformTypeUniformTexelBuffer:
					break;
				case GraphicsUniformType::GraphicsUniformTypeUniformBuffer:
				{
					auto& buffer = it->getBuffer();
					if (buffer)
					{
						auto ubo = buffer->downcast<OGLCoreGraphicsData>();
						glBindBufferBase(GL_UNIFORM_BUFFER, location, ubo->getInstanceID());
					}
				}
				break;
				case GraphicsUniformType::GraphicsUniformTypeUniformBufferDynamic:
					break;
				case GraphicsUniformType::GraphicsUniformTypeInputAttachment:
					break;
				default:
					break;
				}
			}
		}

		void
		OGLCoreDescriptorSet::copy(std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept
		{
			for (std::size_t i = 0; i < descriptorCopyCount; i++)
			{
				if (!descriptorCopies[i])
					continue;

				auto descriptorCope = descriptorCopies[i]->downcast<OGLCoreDescriptorSet>();
				for (auto& activeUniformSet : descriptorCope->_activeUniformSets)
				{
					auto it = std::find_if(_activeUniformSets.begin(), _activeUniformSets.end(), [&](GraphicsUniformSetPtr& it) { return it->getGraphicsParam() == activeUniformSet->getGraphicsParam(); });
					if (it == _activeUniformSets.end())
						continue;

					auto type = activeUniformSet->getGraphicsParam()->getType();
					switch (type)
					{
					case GraphicsUniformType::GraphicsUniformTypeBool:
						(*it)->uniform1b(activeUniformSet->getBool());
						break;
					case GraphicsUniformType::GraphicsUniformTypeInt:
						(*it)->uniform1i(activeUniformSet->getInt());
						break;
					case GraphicsUniformType::GraphicsUniformTypeInt2:
						(*it)->uniform2i(activeUniformSet->getInt2());
						break;
					case GraphicsUniformType::GraphicsUniformTypeInt3:
						(*it)->uniform3i(activeUniformSet->getInt3());
						break;
					case GraphicsUniformType::GraphicsUniformTypeInt4:
						(*it)->uniform4i(activeUniformSet->getInt4());
						break;
					case GraphicsUniformType::GraphicsUniformTypeUInt:
						(*it)->uniform1ui(activeUniformSet->getUInt());
						break;
					case GraphicsUniformType::GraphicsUniformTypeUInt2:
						(*it)->uniform2ui(activeUniformSet->getUInt2());
						break;
					case GraphicsUniformType::GraphicsUniformTypeUInt3:
						(*it)->uniform3ui(activeUniformSet->getUInt3());
						break;
					case GraphicsUniformType::GraphicsUniformTypeUInt4:
						(*it)->uniform4ui(activeUniformSet->getUInt4());
						break;
					case GraphicsUniformType::GraphicsUniformTypeFloat:
						(*it)->uniform1f(activeUniformSet->getFloat());
						break;
					case GraphicsUniformType::GraphicsUniformTypeFloat2:
						(*it)->uniform2f(activeUniformSet->getFloat2());
						break;
					case GraphicsUniformType::GraphicsUniformTypeFloat3:
						(*it)->uniform3f(activeUniformSet->getFloat3());
						break;
					case GraphicsUniformType::GraphicsUniformTypeFloat4:
						(*it)->uniform4f(activeUniformSet->getFloat4());
						break;
					case GraphicsUniformType::GraphicsUniformTypeFloat2x2:
						(*it)->uniform2fmat(activeUniformSet->getFloat2x2());
						break;
					case GraphicsUniformType::GraphicsUniformTypeFloat3x3:
						(*it)->uniform3fmat(activeUniformSet->getFloat3x3());
						break;
					case GraphicsUniformType::GraphicsUniformTypeFloat4x4:
						(*it)->uniform4fmat(activeUniformSet->getFloat4x4());
						break;
					case GraphicsUniformType::GraphicsUniformTypeIntArray:
						(*it)->uniform1iv(activeUniformSet->getIntArray());
						break;
					case GraphicsUniformType::GraphicsUniformTypeInt2Array:
						(*it)->uniform2iv(activeUniformSet->getInt2Array());
						break;
					case GraphicsUniformType::GraphicsUniformTypeInt3Array:
						(*it)->uniform3iv(activeUniformSet->getInt3Array());
						break;
					case GraphicsUniformType::GraphicsUniformTypeInt4Array:
						(*it)->uniform4iv(activeUniformSet->getInt4Array());
						break;
					case GraphicsUniformType::GraphicsUniformTypeUIntArray:
						(*it)->uniform1uiv(activeUniformSet->getUIntArray());
						break;
					case GraphicsUniformType::GraphicsUniformTypeUInt2Array:
						(*it)->uniform2uiv(activeUniformSet->getUInt2Array());
						break;
					case GraphicsUniformType::GraphicsUniformTypeUInt3Array:
						(*it)->uniform3uiv(activeUniformSet->getUInt3Array());
						break;
					case GraphicsUniformType::GraphicsUniformTypeUInt4Array:
						(*it)->uniform4uiv(activeUniformSet->getUInt4Array());
						break;
					case GraphicsUniformType::GraphicsUniformTypeFloatArray:
						(*it)->uniform1fv(activeUniformSet->getFloatArray());
						break;
					case GraphicsUniformType::GraphicsUniformTypeFloat2Array:
						(*it)->uniform2fv(activeUniformSet->getFloat2Array());
						break;
					case GraphicsUniformType::GraphicsUniformTypeFloat3Array:
						(*it)->uniform3fv(activeUniformSet->getFloat3Array());
						break;
					case GraphicsUniformType::GraphicsUniformTypeFloat4Array:
						(*it)->uniform4fv(activeUniformSet->getFloat4Array());
						break;
					case GraphicsUniformType::GraphicsUniformTypeFloat2x2Array:
						(*it)->uniform2fmatv(activeUniformSet->getFloat2x2Array());
						break;
					case GraphicsUniformType::GraphicsUniformTypeFloat3x3Array:
						(*it)->uniform3fmatv(activeUniformSet->getFloat3x3Array());
						break;
					case GraphicsUniformType::GraphicsUniformTypeFloat4x4Array:
						(*it)->uniform4fmatv(activeUniformSet->getFloat4x4Array());
						break;
					case GraphicsUniformType::GraphicsUniformTypeSampler:
						(*it)->uniformTexture(activeUniformSet->getTexture(), activeUniformSet->getTextureSampler());
						break;
					case GraphicsUniformType::GraphicsUniformTypeSamplerImage:
						(*it)->uniformTexture(activeUniformSet->getTexture(), activeUniformSet->getTextureSampler());
						break;
					case GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler:
						(*it)->uniformTexture(activeUniformSet->getTexture(), activeUniformSet->getTextureSampler());
						break;
					case GraphicsUniformType::GraphicsUniformTypeStorageImage:
						(*it)->uniformTexture(activeUniformSet->getTexture(), activeUniformSet->getTextureSampler());
						break;
					case GraphicsUniformType::GraphicsUniformTypeStorageTexelBuffer:
						break;
					case GraphicsUniformType::GraphicsUniformTypeStorageBuffer:
						break;
					case GraphicsUniformType::GraphicsUniformTypeStorageBufferDynamic:
						break;
					case GraphicsUniformType::GraphicsUniformTypeUniformTexelBuffer:
						break;
					case GraphicsUniformType::GraphicsUniformTypeUniformBuffer:
						(*it)->uniformBuffer(activeUniformSet->getBuffer());
						break;
					case GraphicsUniformType::GraphicsUniformTypeUniformBufferDynamic:
						break;
					case GraphicsUniformType::GraphicsUniformTypeInputAttachment:
						break;
					default:
						break;
					}
				}
			}
		}

		const GraphicsUniformSets&
		OGLCoreDescriptorSet::getGraphicsUniformSets() const noexcept
		{
			return _activeUniformSets;
		}

		const GraphicsDescriptorSetDesc&
		OGLCoreDescriptorSet::getGraphicsDescriptorSetDesc() const noexcept
		{
			return _descriptorSetDesc;
		}

		void
		OGLCoreDescriptorSet::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		OGLCoreDescriptorSet::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}