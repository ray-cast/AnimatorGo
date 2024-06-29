#include "gl45_device_context.h"
#include "gl45_texture.h"
#include "gl45_framebuffer.h"
#include "gl45_graphics_data.h"
#include "gl45_descriptor.h"
#include "gl45_pipeline.h"
#include "gl33_input_layout.h"
#include "gl33_state.h"
#include "gl33_shader.h"
#include "gl33_sampler.h"
#include "gl33_device.h"

#include <ogl_swapchain.h>

#include <iostream>

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL45DeviceContext, GraphicsContext, "GL45DeviceContext")

		GL45DeviceContext::GL45DeviceContext() noexcept
			: _clearDepth(1.0f)
			, _clearStencil(0)
			, _inputLayout(GL_NONE)
			, _indexType(GL_UNSIGNED_INT)
			, _indexOffset(0)
			, _needUpdatePipeline(false)
			, _needUpdateDescriptor(false)
			, _needUpdateVertexBuffers(false)
		{
			_stateDefault = std::make_shared<GL33GraphicsState>();
			_stateDefault->setup(RenderStateDesc());
		}

		GL45DeviceContext::~GL45DeviceContext() noexcept
		{
			this->close();
		}

		bool
		GL45DeviceContext::setup(const GraphicsContextDesc& desc) noexcept
		{
			assert(desc.getSwapchain());
			assert(desc.getSwapchain()->isInstanceOf<OGLSwapchain>());

			_glcontext = desc.getSwapchain()->downcast_pointer<OGLSwapchain>();
			_glcontext->setActive(true);

			if (!_glcontext->getActive())
				return false;

			if (!this->checkSupport())
				return false;

			if (this->getDevice()->getDeviceDesc().isEnabledDebugControl())
				this->startDebugControl();

			if (!this->initStateSystem())
				return false;

			return true;
		}

		void
		GL45DeviceContext::close() noexcept
		{
			_framebuffer.reset();
			_program.reset();
			_pipeline.reset();
			_descriptorSet.reset();
			_state.reset();
			_indexBuffer.reset();
			_glcontext.reset();
			_vertexBuffers.clear();

			if (_inputLayout != GL_NONE)
			{
				glDeleteVertexArrays(1, &_inputLayout);
				_inputLayout = GL_NONE;
			}
		}

		void
		GL45DeviceContext::renderBegin() noexcept
		{
			assert(_glcontext);
			_glcontext->setActive(true);

			this->setRenderPipeline(nullptr);
			this->setIndexBufferData(nullptr);
		}

		void
		GL45DeviceContext::renderEnd() noexcept
		{
		}

		void
		GL45DeviceContext::setViewport(std::uint32_t i, const math::float4& view) noexcept
		{
			assert(_glcontext->getActive());

			if (_viewports[i] != view)
			{
				glViewportIndexedf(i, view.left, view.top, view.width, view.height);
				_viewports[i] = view;
			}
		}

		const math::float4&
		GL45DeviceContext::getViewport(std::uint32_t i) const noexcept
		{
			return _viewports[i];
		}

		void
		GL45DeviceContext::setScissor(std::uint32_t i, const math::uint4& scissor) noexcept
		{
			assert(_glcontext->getActive());

			if (_scissors[i] != scissor)
			{
				std::uint32_t height;
				if (_framebuffer)
					height = _framebuffer->getFramebufferDesc().getHeight();
				else
					height = _glcontext->getGraphicsSwapchainDesc().getHeight();

				glScissorIndexed(i, scissor.left, height - scissor.height - scissor.top, scissor.width, scissor.height);
				_scissors[i] = scissor;
			}
		}

		const math::uint4&
		GL45DeviceContext::getScissor(std::uint32_t i) const noexcept
		{
			return _scissors[i];
		}

		void
		GL45DeviceContext::setStencilCompareMask(StencilFaceFlags face, std::uint32_t mask) noexcept
		{
			if (face & StencilFaceFlagBits::FrontBit)
			{
				if (_stateCaptured.getStencilFrontReadMask() != mask)
				{
					GLenum frontfunc = GL33Types::asCompareFunction(_stateCaptured.getStencilFrontFunc());
					if (frontfunc == GL_INVALID_ENUM)
					{
						this->getDevice()->downcast<GL33Device>()->message("Invalid compare function");
						return;
					}

					glStencilFuncSeparate(GL_FRONT, frontfunc, _stateCaptured.getStencilFrontRef(), mask);
					_stateCaptured.setStencilFrontReadMask(mask);
				}
			}
			if (face & StencilFaceFlagBits::BackBit)
			{
				if (_stateCaptured.getStencilBackReadMask() != mask)
				{
					GLenum backfunc = GL33Types::asCompareFunction(_stateCaptured.getStencilBackFunc());
					if (backfunc == GL_INVALID_ENUM)
					{
						this->getDevice()->downcast<GL33Device>()->message("Invalid compare function");
						return;
					}

					glStencilFuncSeparate(GL_BACK, backfunc, _stateCaptured.getStencilBackRef(), mask);
					_stateCaptured.setStencilBackReadMask(mask);
				}
			}
		}

		std::uint32_t
		GL45DeviceContext::getStencilCompareMask(StencilFaceFlags face) noexcept
		{
			assert(face == StencilFaceFlagBits::FrontBit || face == StencilFaceFlagBits::BackBit);

			if (face == StencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontReadMask();
			else
				return _stateCaptured.getStencilBackReadMask();
		}

		void
		GL45DeviceContext::setStencilReference(StencilFaceFlags face, std::uint32_t reference) noexcept
		{
			if (face & StencilFaceFlagBits::FrontBit)
			{
				if (_stateCaptured.getStencilFrontRef() != reference)
				{
					GLenum frontfunc = GL33Types::asCompareFunction(_stateCaptured.getStencilFrontFunc());
					if (frontfunc == GL_INVALID_ENUM)
					{
						this->getDevice()->downcast<GL33Device>()->message("Invalid compare function");
						return;
					}

					glStencilFuncSeparate(GL_FRONT, frontfunc, reference, _stateCaptured.getStencilFrontReadMask());
					_stateCaptured.setStencilFrontRef(reference);
				}
			}
			if (face & StencilFaceFlagBits::BackBit)
			{
				if (_stateCaptured.getStencilBackRef() != reference)
				{
					GLenum backfunc = GL33Types::asCompareFunction(_stateCaptured.getStencilBackFunc());
					if (backfunc == GL_INVALID_ENUM)
					{
						this->getDevice()->downcast<GL33Device>()->message("Invalid compare function");
						return;
					}

					glStencilFuncSeparate(GL_BACK, backfunc, reference, _stateCaptured.getStencilBackReadMask());
					_stateCaptured.setStencilBackRef(reference);
				}
			}
		}

		std::uint32_t
		GL45DeviceContext::getStencilReference(StencilFaceFlags face) noexcept
		{
			assert(face == StencilFaceFlagBits::FrontBit || face == StencilFaceFlagBits::BackBit);

			if (face == StencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontRef();
			else
				return _stateCaptured.getStencilBackRef();
		}

		void
		GL45DeviceContext::setStencilWriteMask(StencilFaceFlags face, std::uint32_t mask) noexcept
		{
			if (face & StencilFaceFlagBits::FrontBit)
			{
				if (_stateCaptured.getStencilFrontWriteMask() != mask)
				{
					glStencilMaskSeparate(GL_FRONT, mask);
					_stateCaptured.setStencilFrontWriteMask(mask);
				}
			}
			if (face & StencilFaceFlagBits::BackBit)
			{
				if (_stateCaptured.getStencilBackWriteMask() != mask)
				{
					glStencilMaskSeparate(GL_BACK, mask);
					_stateCaptured.setStencilBackWriteMask(mask);
				}
			}
		}

		std::uint32_t
		GL45DeviceContext::getStencilWriteMask(StencilFaceFlags face) noexcept
		{
			assert(face == StencilFaceFlagBits::FrontBit || face == StencilFaceFlagBits::BackBit);

			if (face == StencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontWriteMask();
			else
				return _stateCaptured.getStencilBackWriteMask();
		}

		void
		GL45DeviceContext::setRenderPipeline(const GraphicsPipelinePtr& pipeline) noexcept
		{
			assert(!pipeline || pipeline && pipeline->isInstanceOf<GL45Pipeline>());
			assert(_glcontext->getActive());

			if (pipeline)
			{
				auto glpipeline = pipeline->downcast_pointer<GL45Pipeline>();

				if (_pipeline != glpipeline)
				{
					auto& pipelineDesc = pipeline->getPipelineDesc();

					auto glstate = pipelineDesc.getGraphicsState()->downcast_pointer<GL33GraphicsState>();
					if (_state != glstate)
					{
						glstate->apply(_stateCaptured);
						_state = glstate;
					}

					auto glprogram = pipelineDesc.getProgram()->downcast_pointer<GL33Program>();
					if (_program != glprogram)
					{
						_program = glprogram;
						_program->apply();
					}

					if (_pipeline != glpipeline)
					{
						_pipeline = glpipeline;
						_pipeline->apply();
						_needUpdatePipeline = true;
					}
				}
			}
			else
			{
				if (_pipeline != pipeline)
				{
					if (_state != _stateDefault)
					{
						_stateDefault->apply(_stateCaptured);
						_state = _stateDefault;
					}

					if (_program)
					{
						glUseProgram(GL_NONE);
						_program = nullptr;
					}

					_pipeline = nullptr;
				}
			}
		}

		GraphicsPipelinePtr
		GL45DeviceContext::getRenderPipeline() const noexcept
		{
			return _pipeline;
		}

		void
		GL45DeviceContext::setDescriptorSet(const GraphicsDescriptorSetPtr& descriptorSet) noexcept
		{
			assert(descriptorSet);
			assert(descriptorSet->isInstanceOf<GL45DescriptorSet>());
			assert(_glcontext->getActive());

			_descriptorSet = descriptorSet->downcast_pointer<GL45DescriptorSet>();
			_needUpdateDescriptor = true;
		}

		GraphicsDescriptorSetPtr
		GL45DeviceContext::getDescriptorSet() const noexcept
		{
			return _descriptorSet;
		}

		void
		GL45DeviceContext::setVertexBufferData(std::uint32_t i, const GraphicsDataPtr& data, std::intptr_t offset) noexcept
		{
			assert(data);
			assert(data->isInstanceOf<GL45GraphicsData>());
			assert(data->getDataDesc().getType() == GraphicsDataType::StorageVertexBuffer);
			assert(_vertexBuffers.size() > i);
			assert(_glcontext->getActive());

			auto vbo = data->downcast_pointer<GL45GraphicsData>();
			if (_vertexBuffers[i].vbo != vbo || _vertexBuffers[i].offset != offset)
			{
				_vertexBuffers[i].vbo = vbo;
				_vertexBuffers[i].offset = offset;
				_vertexBuffers[i].setDirty = true;
				_needUpdateVertexBuffers = true;
			}
		}

		GraphicsDataPtr
		GL45DeviceContext::getVertexBufferData(std::uint32_t i) const noexcept
		{
			assert(_vertexBuffers.size() > i);
			return _vertexBuffers[i].vbo;
		}

		void
		GL45DeviceContext::setIndexBufferData(const GraphicsDataPtr& data, std::intptr_t offset, IndexFormat indexType) noexcept
		{
			assert(data);
			assert(data->isInstanceOf<GL45GraphicsData>());
			assert(data->getDataDesc().getType() == GraphicsDataType::StorageIndexBuffer);
			assert(indexType == IndexFormat::UInt16 || indexType == IndexFormat::UInt32);
			assert(_glcontext->getActive());

			auto ibo = data->downcast_pointer<GL45GraphicsData>();
			if (_indexBuffer != ibo)
			{
				::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->getInstanceID());
				_indexBuffer = ibo;
			}

			_indexType = GL33Types::asIndexType(indexType);
			_indexOffset = offset;

			if (_indexType == GL_INVALID_ENUM) this->getDevice()->downcast<GL33Device>()->message("Invalid index type");
		}

		GraphicsDataPtr
		GL45DeviceContext::getIndexBufferData() const noexcept
		{
			return _indexBuffer;
		}

		void
		GL45DeviceContext::generateMipmap(const std::shared_ptr<GraphicsTexture>& texture) noexcept
		{
			assert(texture);
			assert(texture->isInstanceOf<GL45Texture>());

			auto gltexture = texture->downcast<GL45Texture>();
			auto textureID = gltexture->getInstanceID();
			glGenerateTextureMipmap(textureID);
		}

		void
		GL45DeviceContext::setFramebuffer(const GraphicsFramebufferPtr& target) noexcept
		{
			assert(_glcontext->getActive());

			if (target)
			{
				auto framebuffer = target->downcast_pointer<GL45Framebuffer>();
				if (_framebuffer != framebuffer)
				{
					_framebuffer = framebuffer;
					glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer->getInstanceID());

					auto& framebufferDesc = _framebuffer->getFramebufferDesc();
					auto& colorAttachment = framebufferDesc.getColorAttachments();

					std::uint32_t viewportCount = std::max<std::uint32_t>(1, static_cast<std::uint32_t>(colorAttachment.size()));
					for (std::uint32_t i = 0; i < viewportCount; i++)
					{
						this->setViewport(i, math::float4(0.0f, 0.0f, (float)framebufferDesc.getWidth(), (float)framebufferDesc.getHeight()));

						glScissorIndexed(i, _scissors[i].left, framebufferDesc.getHeight() - _scissors[i].height - _scissors[i].top, _scissors[i].width, _scissors[i].height);
					}
				}
			}
			else
			{
				glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
				this->setRenderPipeline(nullptr);

				_framebuffer = nullptr;
			}
		}

		void
		GL45DeviceContext::clearFramebuffer(std::uint32_t i, ClearFlags flags, const math::float4& color, float depth, std::int32_t stencil) noexcept
		{
			assert(_glcontext->getActive());

			GLint buffer = i;

			if (_stateCaptured.getScissorTestEnable())
			{
				if (_framebuffer)
					glScissor(0, 0, _framebuffer->getFramebufferDesc().getWidth(), _framebuffer->getFramebufferDesc().getHeight());
				else
					glScissor(0, 0, _glcontext->getGraphicsSwapchainDesc().getWidth(), _glcontext->getGraphicsSwapchainDesc().getHeight());
			}

			if (flags & ClearFlagBits::DepthBit)
			{
				auto depthWriteEnable = _stateCaptured.getDepthWriteEnable();
				if (!depthWriteEnable)
				{
					glDepthMask(GL_TRUE);
				}

				if (flags & ClearFlagBits::StencilBit)
				{
					GLint s = stencil;
					GLfloat f = depth;
					glClearBufferfi(GL_DEPTH_STENCIL, buffer, f, s);
				}
				else
				{
					GLfloat f = depth;
					glClearBufferfv(GL_DEPTH, buffer, &f);
				}

				if (!depthWriteEnable)
				{
					glDepthMask(GL_FALSE);
				}
			}
			else if (flags & ClearFlagBits::StencilBit)
			{
				GLint s = stencil;
				glClearBufferiv(GL_STENCIL, buffer, &s);
			}

			if (flags & ClearFlagBits::ColorBit)
			{
				auto colorWriteFlags = _stateCaptured.getColorWriteMask();
				if (colorWriteFlags != ColorWriteMask::RGBABit)
				{
					glColorMaski(buffer, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				}

				glClearBufferfv(GL_COLOR, buffer, color.ptr());

				if (colorWriteFlags != ColorWriteMask::RGBABit)
				{
					GLboolean r = colorWriteFlags & ColorWriteMask::RedBit ? GL_TRUE : GL_FALSE;
					GLboolean g = colorWriteFlags & ColorWriteMask::GreendBit ? GL_TRUE : GL_FALSE;
					GLboolean b = colorWriteFlags & ColorWriteMask::BlurBit ? GL_TRUE : GL_FALSE;
					GLboolean a = colorWriteFlags & ColorWriteMask::AlphaBit ? GL_TRUE : GL_FALSE;
					glColorMaski(buffer, r, g, b, a);
				}
			}

			if (_stateCaptured.getScissorTestEnable())
			{
				if (_framebuffer)
				{
					GLuint viewportCount = std::max<GLuint>(1, (GLuint)_framebuffer->getFramebufferDesc().getColorAttachments().size());
					for (GLuint j = 0; j < viewportCount; j++)
					{
						glScissorIndexed(j, _scissors[j].left, _scissors[j].top, _scissors[j].width, _scissors[j].height);
					}
				}
				else
				{
					glScissorIndexed(0, _scissors[0].left, _scissors[0].top, _scissors[0].width, _scissors[0].height);
				}
			}
		}

		void
		GL45DeviceContext::blitFramebuffer(const GraphicsFramebufferPtr& src, const math::float4& v1, const GraphicsFramebufferPtr& dest, const math::float4& v2, SamplerFilter filter) noexcept
		{
			assert(src);
			assert(src->isInstanceOf<GL45Framebuffer>());
			assert(!dest || (dest && dest->isInstanceOf<GL45Framebuffer>()));
			assert(_glcontext->getActive());

			this->_readFramebuffer = src ? src->downcast_pointer<GL45Framebuffer>() : nullptr;
			this->_drawFramebuffer = dest ? dest->downcast_pointer<GL45Framebuffer>() : nullptr;

			auto readFramebuffer = this->_readFramebuffer->getInstanceID();
			auto drawFramebuffer = this->_drawFramebuffer ? this->_drawFramebuffer->getInstanceID() : GL_NONE;

			GLenum mode = (filter == SamplerFilter::Linear ? GL_LINEAR : GL_NEAREST);
			glBlitNamedFramebuffer(readFramebuffer, drawFramebuffer, (GLint)v1.left, (GLint)v1.top, (GLint)v1.width, (GLint)v1.height, (GLint)v2.left, (GLint)v2.top, (GLint)v2.width, (GLint)v2.height, GL_COLOR_BUFFER_BIT, mode);
		}

		void
		GL45DeviceContext::discardFramebuffer(const GraphicsFramebufferPtr& framebuffer, ClearFlags flags) noexcept
		{
			assert(framebuffer);
			assert(_glcontext->getActive());

			if (framebuffer)
			{
				auto fbo = framebuffer->downcast_pointer<GL45Framebuffer>();

				_attachments.clear();

				const auto& layoutDesc = fbo->getFramebufferDesc().getFramebufferLayout()->getFramebufferLayoutDesc();
				auto count = layoutDesc.getComponents().size();
				for (GLenum i = 0; i < count; i++)
				{
					auto& attachment = layoutDesc.getComponents().at(i);
					if (attachment.getAttachType() == GraphicsImageLayout::ColorAttachmentOptimal)
					{
						if (flags & ClearFlagBits::ColorBit)
							_attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
					}
					else if (attachment.getAttachType() == GraphicsImageLayout::DepthStencilReadOnlyOptimal ||
						attachment.getAttachType() == GraphicsImageLayout::DepthStencilAttachmentOptimal)
					{
						auto format = attachment.getAttachFormat();
						if (format == GraphicsFormat::S8UInt)
						{
							if (flags & ClearFlagBits::StencilBit)
								_attachments.push_back(GL_STENCIL_ATTACHMENT);
						}
						else if (format == GraphicsFormat::D16UNorm || format == GraphicsFormat::X8_D24UNormPack32 || format == GraphicsFormat::D32_SFLOAT)
						{
							if (flags & ClearFlagBits::DepthBit)
								_attachments.push_back(GL_DEPTH_ATTACHMENT);
						}
						else
						{
							if (flags & ClearFlagBits::DepthStencilBit)
								_attachments.push_back(GL_DEPTH_STENCIL_ATTACHMENT);
						}
					}
				}

				if (!_attachments.empty())
					glInvalidateNamedFramebufferData(fbo->getInstanceID(), (GLsizei)_attachments.size(), _attachments.data());
			}
		}

		void
		GL45DeviceContext::readFramebuffer(std::uint32_t i, const std::shared_ptr<GraphicsTexture>& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
		{
			GLenum internalFormat = GL33Types::asTextureInternalFormat(texture->getTextureDesc().getTexFormat());
			if (internalFormat == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<GL33Device>()->message("Invalid texture internal format.");
				return;
			}

			glNamedFramebufferReadBuffer(_framebuffer->getInstanceID(), GL_COLOR_ATTACHMENT0 + i);
			glCopyTextureSubImage2D(texture->downcast<GL45Texture>()->getInstanceID(), miplevel, 0, 0, x, y, width, height);
		}

		void
		GL45DeviceContext::readFramebufferToCube(std::uint32_t i, std::uint32_t face, const std::shared_ptr<GraphicsTexture>& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
		{
			GLenum internalFormat = GL33Types::asTextureFormat(texture->getTextureDesc().getTexFormat());
			if (internalFormat == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<GL33Device>()->message("Invalid texture format");
				return;
			}

			glNamedFramebufferReadBuffer(_framebuffer->getInstanceID(), GL_COLOR_ATTACHMENT0 + i);
			glCopyTextureSubImage2DEXT(texture->downcast<GL45Texture>()->getInstanceID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, miplevel, 0, 0, x, y, width, height);
		}

		GraphicsFramebufferPtr
		GL45DeviceContext::getFramebuffer() const noexcept
		{
			return _framebuffer;
		}

		void
		GL45DeviceContext::draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
		{
			assert(_pipeline);
			assert(_glcontext->getActive());
			assert(startInstances == 0);

			if (_needUpdatePipeline || _needUpdateVertexBuffers)
			{
				_pipeline->bindVertexBuffers(_vertexBuffers, _needUpdatePipeline);
				_needUpdatePipeline = false;
				_needUpdateVertexBuffers = false;
			}

			if (_needUpdateDescriptor)
			{
				_descriptorSet->apply(*_program);
				_needUpdateDescriptor = false;
			}

			if (numVertices > 0)
			{
				GLenum drawType = GL33Types::asVertexType(_stateCaptured.getPrimitiveType());
				if (drawType != GL_INVALID_ENUM)
					glDrawArraysInstancedBaseInstance(drawType, startVertice, numVertices, numInstances, startInstances);
				else
					this->getDevice()->downcast<GL33Device>()->message("Invalid vertex type");
			}
		}

		void
		GL45DeviceContext::drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
		{
			assert(_pipeline);
			assert(_glcontext->getActive());
			assert(_indexBuffer);
			assert(_indexType == GL_UNSIGNED_INT || _indexType == GL_UNSIGNED_SHORT);
			assert(startInstances == 0);

			if (_needUpdatePipeline || _needUpdateVertexBuffers)
			{
				_pipeline->bindVertexBuffers(_vertexBuffers, _needUpdatePipeline);
				_needUpdatePipeline = false;
				_needUpdateVertexBuffers = false;
			}

			if (_needUpdateDescriptor)
			{
				_descriptorSet->apply(*_program);
				_needUpdateDescriptor = false;
			}

			if (numIndices > 0)
			{
				GLbyte* offsetIndices = nullptr;
				if (_indexType == GL_UNSIGNED_INT)
					offsetIndices += _indexOffset + sizeof(std::uint32_t) * startIndice;
				else
					offsetIndices += _indexOffset + sizeof(std::uint16_t) * startIndice;

				GLenum drawType = GL33Types::asVertexType(_stateCaptured.getPrimitiveType());
				if (drawType != GL_INVALID_ENUM)
					glDrawElementsInstancedBaseVertexBaseInstance(drawType, numIndices, _indexType, offsetIndices, numInstances, startVertice, startInstances);
				else
					this->getDevice()->downcast<GL33Device>()->message("Invalid vertex type");
			}
		}

		void
		GL45DeviceContext::drawIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept
		{
			assert(_pipeline);
			assert(_glcontext->getActive());
			assert(data && data->getDataDesc().getType() == GraphicsDataType::IndirectBiffer);

			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, data->downcast<GL45GraphicsData>()->getInstanceID());

			if (drawCount > 0)
			{
				GLenum drawType = GL33Types::asVertexType(_stateCaptured.getPrimitiveType());
				if (drawType != GL_INVALID_ENUM)
					glMultiDrawArraysIndirect(drawType, (char*)nullptr + offset, drawCount, stride);
				else
					this->getDevice()->downcast<GL33Device>()->message("Invalid vertex type");
			}
		}

		void
		GL45DeviceContext::drawIndexedIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept
		{
			assert(_pipeline);
			assert(_glcontext->getActive());
			assert(data && data->getDataDesc().getType() == GraphicsDataType::IndirectBiffer);

			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, data->downcast<GL45GraphicsData>()->getInstanceID());

			if (drawCount > 0)
			{
				GLenum drawType = GL33Types::asVertexType(_stateCaptured.getPrimitiveType());
				if (drawType != GL_INVALID_ENUM)
					glMultiDrawElementsIndirect(drawType, GL_UNSIGNED_INT, (char*)nullptr + offset, drawCount, stride);
				else
					this->getDevice()->downcast<GL33Device>()->message("Invalid vertex type");
			}
		}

		void
		GL45DeviceContext::present() noexcept
		{
			assert(_glcontext->getActive());
			_glcontext->present();
		}

		bool
		GL45DeviceContext::checkSupport() noexcept
		{
			if (!GLEW_ARB_vertex_array_object)
			{
				this->getDevice()->downcast<GL33Device>()->message("Can't support vertex array object");
				return false;
			}

			if (!GL_ARB_vertex_attrib_binding)
			{
				this->getDevice()->downcast<GL33Device>()->message("Can't support vertex attrib binding");
				return false;
			}

			if (!GLEW_ARB_sampler_objects)
			{
				this->getDevice()->downcast<GL33Device>()->message("Can't support sampler object");
				return false;
			}

			if (!GLEW_ARB_framebuffer_object)
			{
				this->getDevice()->downcast<GL33Device>()->message("Can't support framebuffer object");
				return false;
			}

			if (!GLEW_ARB_uniform_buffer_object)
			{
				this->getDevice()->downcast<GL33Device>()->message("Can't support uniform buffer object");
				return false;
			}

			if (!GLEW_ARB_texture_buffer_object)
			{
				this->getDevice()->downcast<GL33Device>()->message("Can't support texture buffer object");
				return false;
			}

			if (!GL_ARB_buffer_storage)
			{
				this->getDevice()->downcast<GL33Device>()->message("Can't support buffer storage");
				return false;
			}

			if (!GLEW_ARB_texture_storage)
			{
				this->getDevice()->downcast<GL33Device>()->message("Can't support texture storage");
				return false;
			}

			if (!GLEW_EXT_texture_filter_anisotropic)
			{
				this->getDevice()->downcast<GL33Device>()->message("Can't support anisotropic sampler");
				return false;
			}

			if (!GLEW_ARB_separate_shader_objects)
			{
				this->getDevice()->downcast<GL33Device>()->message("Can't support separate shader objects");
				return false;
			}

			if (!GLEW_ARB_direct_state_access)
			{
				this->getDevice()->downcast<GL33Device>()->message("Can't support direct state access");
				return false;
			}

			return true;
		}

		void
		GL45DeviceContext::startDebugControl() noexcept
		{
			if (!GLEW_KHR_debug)
			{
				this->getDevice()->downcast<GL33Device>()->message("Can't support GL_KHR_debug.");
				return;
			}

			// 131184 memory info
			// 131185 memory allocation info
			GLuint ids[] =
			{
				131076,
				131169,
				131184,
				131185,
				131218,
				131204
			};

			glEnable(GL_DEBUG_OUTPUT);

			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

			glDebugMessageCallback(debugCallBack, this);

			// enable all
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);

			// disable ids
			glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 6, ids, GL_FALSE);
		}

		void
		GL45DeviceContext::stopDebugControl() noexcept
		{
			if (GLEW_KHR_debug)
			{
				glDisable(GL_DEBUG_OUTPUT);

				glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			}
		}

		bool
		GL45DeviceContext::initStateSystem() noexcept
		{
			glClearDepth(1.0f);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClearStencil(0);

			glViewport(0, 0, 0, 0);

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LEQUAL);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CW);

			glDisable(GL_STENCIL_TEST);
			glStencilMask(0xFFFFFFFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);

			glDisable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glGenVertexArrays(1, &_inputLayout);
			glBindVertexArray(_inputLayout);

			if (GLEW_NV_vertex_buffer_unified_memory)
			{
				glEnableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
			}

			auto& deviceProperties = this->getDevice()->getSystemInfo();
			_vertexBuffers.resize(deviceProperties.maxVertexInputBindings);
			_viewports.resize(deviceProperties.maxViewports, math::float4::Zero);
			_scissors.resize(deviceProperties.maxViewports, math::uint4::Zero);
			_clearColor.resize(deviceProperties.maxFramebufferColorAttachments, math::float4(0.0f, 0.0f, 0.0f, 0.0f));

			return true;
		}

		void
		GL45DeviceContext::debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept
		{
			std::cerr << "source : ";
			switch (source)
			{
			case GL_DEBUG_SOURCE_API:
				std::cerr << "GL_DEBUG_SOURCE_API";
				break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
				std::cerr << "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
				break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER:
				std::cerr << "GL_DEBUG_SOURCE_SHADER_COMPILER";
				break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:
				std::cerr << "GL_DEBUG_SOURCE_THIRD_PARTY";
				break;
			case GL_DEBUG_SOURCE_APPLICATION:
				std::cerr << "GL_DEBUG_SOURCE_APPLICATION";
				break;
			case GL_DEBUG_SOURCE_OTHER:
				std::cerr << "GL_DEBUG_SOURCE_OTHER";
				break;
			}

			std::cerr << std::endl;

			std::cerr << "type : ";
			switch (type)
			{
			case GL_DEBUG_TYPE_ERROR:
				std::cerr << "GL_DEBUG_TYPE_ERROR";
				break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
				std::cerr << "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
				break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
				std::cerr << "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
				break;
			case GL_DEBUG_TYPE_PORTABILITY:
				std::cerr << "GL_DEBUG_TYPE_PORTABILITY";
				break;
			case GL_DEBUG_TYPE_PERFORMANCE:
				std::cerr << "GL_DEBUG_TYPE_PERFORMANCE";
				break;
			case GL_DEBUG_TYPE_OTHER:
				std::cerr << "GL_DEBUG_TYPE_OTHER";
				break;
			case GL_DEBUG_TYPE_MARKER:
				std::cerr << "GL_DEBUG_TYPE_MARKER";
				break;
			case GL_DEBUG_TYPE_PUSH_GROUP:
				std::cerr << "GL_DEBUG_TYPE_PUSH_GROUP";
				break;
			case GL_DEBUG_TYPE_POP_GROUP:
				std::cerr << "GL_DEBUG_TYPE_POP_GROUP";
				break;
			}

			std::cerr << std::endl;

			std::cerr << "id : " << id << std::endl;

			switch (severity)
			{
			case GL_DEBUG_SEVERITY_NOTIFICATION:
				std::cerr << "notice";
				break;
			case GL_DEBUG_SEVERITY_LOW:
				std::cerr << "low";
				break;
			case GL_DEBUG_SEVERITY_MEDIUM:
				std::cerr << "medium";
				break;
			case GL_DEBUG_SEVERITY_HIGH:
				std::cerr << "high";
				break;
			}

			std::cerr << std::endl;

			std::cerr << "message : " << message << std::endl;
		}

		void
		GL45DeviceContext::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL45DeviceContext::getDevice() const noexcept
		{
			return _device.lock();
		}
	}
}