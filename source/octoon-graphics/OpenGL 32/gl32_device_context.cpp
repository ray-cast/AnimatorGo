#include "gl32_device_context.h"
#include "gl32_state.h"
#include "gl32_shader.h"
#include "gl32_texture.h"
#include "gl32_framebuffer.h"
#include "gl32_input_layout.h"
#include "gl32_sampler.h"
#include "gl32_descriptor.h"
#include "gl32_pipeline.h"
#include "gl32_graphics_data.h"

#include "ogl_swapchain.h"

#include <iostream>

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(GL32DeviceContext, GraphicsContext, "GL32DeviceContext")

		GL32DeviceContext::GL32DeviceContext() noexcept
			: _clearDepth(1.0)
			, _clearStencil(0)
			, _viewport(0, 0, 0, 0)
			, _inputLayout(GL_NONE)
			, _indexType(GL_UNSIGNED_INT)
			, _needUpdatePipeline(false)
			, _needUpdateDescriptor(false)
			, _needUpdateVertexBuffers(false)
			, _needEnableDebugControl(false)
			, _needDisableDebugControl(false)
		{
		}

		GL32DeviceContext::~GL32DeviceContext() noexcept
		{
			this->close();
		}

		bool
		GL32DeviceContext::setup(const GraphicsContextDesc& desc) noexcept
		{
			assert(desc.getSwapchain());
			assert(desc.getSwapchain()->isInstanceOf<OGLSwapchain>());

			_glcontext = desc.getSwapchain()->downcast_pointer<OGLSwapchain>();
			_glcontext->setActive(true);

			if (!_glcontext->getActive())
				return false;

			if (GraphicsSystem::instance()->enableDebugControl())
				this->startDebugControl();

			if (!this->initStateSystem())
				return false;

			return true;
		}

		void
		GL32DeviceContext::close() noexcept
		{
			_framebuffer.reset();
			_program.reset();
			_pipeline.reset();
			_descriptorSet.reset();
			_state.reset();
			_indexBuffer.reset();
			_glcontext.reset();
			_vertexBuffers.clear();
		}

		void
		GL32DeviceContext::renderBegin() noexcept
		{
			assert(_glcontext);
			_glcontext->setActive(true);

			if (_needEnableDebugControl)
			{
				this->startDebugControl();
				_needEnableDebugControl = false;
			}

			if (_needDisableDebugControl)
			{
				this->stopDebugControl();
				_needDisableDebugControl = false;
			}
		}

		void
		GL32DeviceContext::renderEnd() noexcept
		{
		}

		void
		GL32DeviceContext::setViewport(std::uint32_t i, const float4& view) noexcept
		{
			assert(_glcontext->getActive());

			if (_viewport != view)
			{
				GL_CHECK(glViewport(view.left, view.top, view.width, view.height));
				_viewport = view;
			}
		}

		const float4&
		GL32DeviceContext::getViewport(std::uint32_t i) const noexcept
		{
			return _viewport;
		}

		void
		GL32DeviceContext::setScissor(std::uint32_t i, const uint4& scissor) noexcept
		{
			assert(_glcontext->getActive());

			if (_scissor != scissor)
			{
				std::uint32_t height;
				if (_framebuffer)
					height = _framebuffer->getGraphicsFramebufferDesc().getHeight();
				else
					height = _glcontext->getGraphicsSwapchainDesc().getHeight();

				GL_CHECK(glScissor(scissor.left, height - scissor.top, scissor.width, scissor.height));
				_scissor = scissor;
			}
		}

		const uint4&
		GL32DeviceContext::getScissor(std::uint32_t i) const noexcept
		{
			return _scissor;
		}

		void
		GL32DeviceContext::setStencilCompareMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
		{
			if (face & GraphicsStencilFaceFlagBits::FrontBit)
			{
				if (_stateCaptured.getStencilFrontReadMask() != mask)
				{
					GLenum frontfunc = GL32Types::asCompareFunction(_stateCaptured.getStencilFrontFunc());
					GL_CHECK(glStencilFuncSeparate(GL_FRONT, frontfunc, _stateCaptured.getStencilFrontRef(), mask));
					_stateCaptured.setStencilFrontReadMask(mask);
				}
			}
			if (face & GraphicsStencilFaceFlagBits::BackBit)
			{
				if (_stateCaptured.getStencilBackReadMask() != mask)
				{
					GLenum backfunc = GL32Types::asCompareFunction(_stateCaptured.getStencilBackFunc());
					GL_CHECK(glStencilFuncSeparate(GL_BACK, backfunc, _stateCaptured.getStencilBackRef(), mask));
					_stateCaptured.setStencilBackReadMask(mask);
				}
			}
		}

		std::uint32_t
		GL32DeviceContext::getStencilCompareMask(GraphicsStencilFaceFlags face) noexcept
		{
			assert(face == GraphicsStencilFaceFlagBits::FrontBit || face == GraphicsStencilFaceFlagBits::BackBit);

			if (face == GraphicsStencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontReadMask();
			else
				return _stateCaptured.getStencilBackReadMask();
		}

		void
		GL32DeviceContext::setStencilReference(GraphicsStencilFaceFlags face, std::uint32_t reference) noexcept
		{
			if (face & GraphicsStencilFaceFlagBits::FrontBit)
			{
				if (_stateCaptured.getStencilFrontRef() != reference)
				{
					GLenum frontfunc = GL32Types::asCompareFunction(_stateCaptured.getStencilFrontFunc());
					GL_CHECK(glStencilFuncSeparate(GL_FRONT, frontfunc, reference, _stateCaptured.getStencilFrontReadMask()));
					_stateCaptured.setStencilFrontRef(reference);
				}
			}
			if (face & GraphicsStencilFaceFlagBits::BackBit)
			{
				if (_stateCaptured.getStencilBackRef() != reference)
				{
					GLenum backfunc = GL32Types::asCompareFunction(_stateCaptured.getStencilBackFunc());
					GL_CHECK(glStencilFuncSeparate(GL_BACK, backfunc, reference, _stateCaptured.getStencilBackReadMask()));
					_stateCaptured.setStencilBackRef(reference);
				}
			}
		}

		std::uint32_t
		GL32DeviceContext::getStencilReference(GraphicsStencilFaceFlags face) noexcept
		{
			assert(face == GraphicsStencilFaceFlagBits::FrontBit || face == GraphicsStencilFaceFlagBits::BackBit);

			if (face == GraphicsStencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontRef();
			else
				return _stateCaptured.getStencilBackRef();
		}

		void
		GL32DeviceContext::setStencilWriteMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
		{
			if (face & GraphicsStencilFaceFlagBits::FrontBit)
			{
				if (_stateCaptured.getStencilFrontWriteMask() != mask)
				{
					GL_CHECK(glStencilMaskSeparate(GL_FRONT, mask));
					_stateCaptured.setStencilFrontWriteMask(mask);
				}
			}
			if (face & GraphicsStencilFaceFlagBits::BackBit)
			{
				if (_stateCaptured.getStencilBackWriteMask() != mask)
				{
					GL_CHECK(glStencilMaskSeparate(GL_BACK, mask));
					_stateCaptured.setStencilBackWriteMask(mask);
				}
			}
		}

		std::uint32_t
		GL32DeviceContext::getStencilWriteMask(GraphicsStencilFaceFlags face) noexcept
		{
			assert(face == GraphicsStencilFaceFlagBits::FrontBit || face == GraphicsStencilFaceFlagBits::BackBit);

			if (face == GraphicsStencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontWriteMask();
			else
				return _stateCaptured.getStencilBackWriteMask();
		}

		void
		GL32DeviceContext::setRenderPipeline(const GraphicsPipelinePtr& pipeline) noexcept
		{
			assert(pipeline);
			assert(pipeline->isInstanceOf<GL32Pipeline>());
			assert(_glcontext->getActive());

			auto glpipeline = pipeline->downcast_pointer<GL32Pipeline>();
			if (_pipeline != glpipeline)
			{
				auto& pipelineDesc = pipeline->getGraphicsPipelineDesc();

				auto glstate = pipelineDesc.getGraphicsState()->downcast_pointer<GL32GraphicsState>();
				if (_state != glstate)
				{
					glstate->apply(_stateCaptured);
					_state = glstate;
				}

				auto glshader = pipelineDesc.getGraphicsProgram()->downcast_pointer<GL32Program>();
				if (_program != glshader)
				{
					_program = glshader;
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

		GraphicsPipelinePtr
		GL32DeviceContext::getRenderPipeline() const noexcept
		{
			return _pipeline;
		}

		void
		GL32DeviceContext::setDescriptorSet(const GraphicsDescriptorSetPtr& descriptorSet) noexcept
		{
			assert(descriptorSet);
			assert(descriptorSet->isInstanceOf<GL32DescriptorSet>());
			assert(_glcontext->getActive());

			_descriptorSet = descriptorSet->downcast_pointer<GL32DescriptorSet>();
			_needUpdateDescriptor = true;
		}

		GraphicsDescriptorSetPtr
		GL32DeviceContext::getDescriptorSet() const noexcept
		{
			return _descriptorSet;
		}

		void
		GL32DeviceContext::setVertexBufferData(std::uint32_t i, const GraphicsDataPtr& data, std::intptr_t offset) noexcept
		{
			assert(data);
			assert(data->isInstanceOf<GL32GraphicsData>());
			assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::StorageVertexBuffer);
			assert(_vertexBuffers.size() > i);
			assert(_glcontext->getActive());

			auto vbo = data->downcast_pointer<GL32GraphicsData>();
			if (_vertexBuffers[i].vbo != vbo || _vertexBuffers[i].offset != offset)
			{
				_vertexBuffers[i].vbo = vbo;
				_vertexBuffers[i].offset = offset;
				_vertexBuffers[i].needUpdate = true;
				_needUpdateVertexBuffers = true;
			}
		}

		GraphicsDataPtr
		GL32DeviceContext::getVertexBufferData(std::uint32_t i) const noexcept
		{
			assert(i < _vertexBuffers.size());
			return _vertexBuffers[i].vbo;
		}

		void
		GL32DeviceContext::setIndexBufferData(const GraphicsDataPtr& data, std::intptr_t offset, GraphicsIndexType indexType) noexcept
		{
			assert(data);
			assert(data->isInstanceOf<GL32GraphicsData>());
			assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::StorageIndexBuffer);
			assert(indexType == GraphicsIndexType::UInt16 || indexType == GraphicsIndexType::UInt32);
			assert(_glcontext->getActive());

			auto ibo = data->downcast_pointer<GL32GraphicsData>();
			if (_indexBuffer != ibo)
			{
				GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->getInstanceID()));
				_indexBuffer = ibo;
			}

			_indexType = GL32Types::asIndexType(indexType);
			_indexOffset = offset;
		}

		GraphicsDataPtr
		GL32DeviceContext::getIndexBufferData() const noexcept
		{
			return _indexBuffer;
		}

		void
		GL32DeviceContext::generateMipmap(const GraphicsTexturePtr& texture) noexcept
		{
			assert(texture);
			assert(texture->isInstanceOf<GL32Texture>());

			auto gltexture = texture->downcast<GL32Texture>();
			auto textureID = gltexture->getInstanceID();
			auto textureTarget = gltexture->getTarget();

			GL_CHECK(glActiveTexture(GL_TEXTURE0));
			GL_CHECK(glBindTexture(textureTarget, textureID));
			GL_CHECK(glGenerateMipmap(textureTarget));
		}

		void
		GL32DeviceContext::setFramebuffer(const GraphicsFramebufferPtr& target) noexcept
		{
			assert(_glcontext->getActive());

			if (target)
			{
				auto framebuffer = target->downcast_pointer<GL32Framebuffer>();
				if (_framebuffer != framebuffer)
				{
					_framebuffer = framebuffer;
					GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer->getInstanceID()));

					auto& framebufferDesc = _framebuffer->getGraphicsFramebufferDesc();
					this->setViewport(0, float4(0, 0, framebufferDesc.getWidth(), framebufferDesc.getHeight()));

					glScissor(_scissor.left, framebufferDesc.getHeight() - _scissor.height - _scissor.top, _scissor.width, _scissor.height);
				}
			}
			else
			{
				GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE));
				_framebuffer = nullptr;
			}
		}

		void
		GL32DeviceContext::blitFramebuffer(const GraphicsFramebufferPtr& src, const float4& v1, const GraphicsFramebufferPtr& dest, const float4& v2) noexcept
		{
			assert(src);
			assert(src->isInstanceOf<GL32Framebuffer>());
			assert(_glcontext->getActive());

			auto srcTarget = src->downcast<GL32Framebuffer>()->getInstanceID();
			glBindFramebuffer(GL_READ_FRAMEBUFFER, srcTarget);

			if (dest)
			{
				auto destTarget = dest->downcast<GL32Framebuffer>()->getInstanceID();
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destTarget);
			}
			else
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

			glBlitFramebuffer(v1.left, v1.top, v1.width, v1.height, v2.left, v2.top, v2.width, v2.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		GraphicsFramebufferPtr
		GL32DeviceContext::getFramebuffer() const noexcept
		{
			return _framebuffer;
		}

		void
		GL32DeviceContext::clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
		{
			assert(_framebuffer || i == 0);
			assert(_glcontext->getActive());

			GLint buffer = 0;
			if (_framebuffer)
			{
				const auto& layoutDesc = _framebuffer->getGraphicsFramebufferDesc().getFramebufferLayout()->getFramebufferLayoutDesc();
				if (layoutDesc.getComponents().size() <= i)
					return;

				auto type = layoutDesc.getComponents().at(i).getAttachType();
				if (type == GraphicsImageLayout::ColorAttachmentOptimal)
				{
					if (!(flags & GraphicsClearFlagBits::ColorBit))
						return;

					flags = GraphicsClearFlagBits::ColorBit;
					buffer = i;
				}
				else if (type == GraphicsImageLayout::DepthStencilAttachmentOptimal ||
						 type == GraphicsImageLayout::DepthStencilReadOnlyOptimal)
				{
					if (!(flags & GraphicsClearFlagBits::DepthBit) &&
						!(flags & GraphicsClearFlagBits::StencilBit))
					{
						return;
					}
				}
			}

			if (_stateCaptured.getScissorTestEnable())
			{
				if (_framebuffer)
					glScissor(0, 0, _framebuffer->getGraphicsFramebufferDesc().getWidth(), _framebuffer->getGraphicsFramebufferDesc().getHeight());
				else
					glScissor(0, 0, _glcontext->getGraphicsSwapchainDesc().getWidth(), _glcontext->getGraphicsSwapchainDesc().getHeight());
			}

			if (flags & GraphicsClearFlagBits::DepthBit)
			{
				auto depthWriteEnable = _stateCaptured.getDepthWriteEnable();
				if (!depthWriteEnable)
				{
					glDepthMask(GL_TRUE);
				}

				if (flags & GraphicsClearFlagBits::StencilBit)
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
			else if (flags & GraphicsClearFlagBits::StencilBit)
			{
				GLint s = stencil;
				glClearBufferiv(GL_STENCIL, buffer, &s);
			}
			else
			{
				auto colorWriteFlags = _stateCaptured.getColorBlends()[buffer].getColorWriteMask();
				if (colorWriteFlags != GraphicsColorMaskFlagBits::RGBABit)
				{
					glColorMaski(buffer, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				}

				glClearBufferfv(GL_COLOR, buffer, color.ptr());

				if (colorWriteFlags != GraphicsColorMaskFlagBits::RGBABit)
				{
					GLboolean r = colorWriteFlags & GraphicsColorMaskFlagBits::RedBit ? GL_TRUE : GL_FALSE;
					GLboolean g = colorWriteFlags & GraphicsColorMaskFlagBits::GreendBit ? GL_TRUE : GL_FALSE;
					GLboolean b = colorWriteFlags & GraphicsColorMaskFlagBits::BlurBit ? GL_TRUE : GL_FALSE;
					GLboolean a = colorWriteFlags & GraphicsColorMaskFlagBits::AlphaBit ? GL_TRUE : GL_FALSE;

					glColorMaski(buffer, r, g, b, a);
				}
			}

			if (_stateCaptured.getScissorTestEnable())
			{
				glScissor(_scissor.left, _scissor.top, _scissor.width, _scissor.height);
			}
		}

		void
		GL32DeviceContext::discardFramebuffer(std::uint32_t i) noexcept
		{
			assert(_framebuffer);
			assert(_glcontext->getActive());

			const auto& layoutDesc = _framebuffer->getGraphicsFramebufferDesc().getFramebufferLayout()->getFramebufferLayoutDesc();
			if (layoutDesc.getComponents().size() > i)
			{
				auto& attachment = layoutDesc.getComponents().at(i);
				switch (attachment.getAttachType())
				{
				case GraphicsImageLayout::ColorAttachmentOptimal:
				{
					GLenum attachments = GL_COLOR_ATTACHMENT0 + i;
					glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, &attachments);
				}
				break;
				case GraphicsImageLayout::DepthStencilReadOnlyOptimal:
				case GraphicsImageLayout::DepthStencilAttachmentOptimal:
				{
					auto format = attachment.getAttachFormat();
					if (format == GraphicsFormat::S8UInt)
					{
						GLenum attachments = GL_STENCIL_ATTACHMENT;
						glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, &attachments);
					}
					else if (format == GraphicsFormat::D16UNorm || format == GraphicsFormat::X8_D24UNormPack32 || format == GraphicsFormat::D32_SFLOAT)
					{
						GLenum attachments = GL_DEPTH_ATTACHMENT;
						glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, &attachments);
					}
					else
					{
						GLenum attachments = GL_DEPTH_STENCIL_ATTACHMENT;
						glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, &attachments);
					}
				}
				break;
				default:
					break;
				}
			}
		}

		void
		GL32DeviceContext::readFramebuffer(std::uint32_t i, const GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
		{
		}

		void
		GL32DeviceContext::readFramebufferToCube(std::uint32_t i, std::uint32_t face, const GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
		{
		}

		void
		GL32DeviceContext::draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
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
				GLenum drawType = GL32Types::asVertexType(_stateCaptured.getPrimitiveType());
				GL_CHECK(glDrawArraysInstanced(drawType, startVertice, numVertices, numInstances));
			}
		}

		void
		GL32DeviceContext::drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
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

				GLenum drawType = GL32Types::asVertexType(_stateCaptured.getPrimitiveType());
				GL_CHECK(glDrawElementsInstanced(drawType, numIndices, _indexType, offsetIndices, numInstances));
			}
		}

		void
		GL32DeviceContext::drawIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept
		{
		}

		void
		GL32DeviceContext::drawIndexedIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept
		{
		}

		void
		GL32DeviceContext::present() noexcept
		{
			assert(_glcontext->getActive());
			_glcontext->present();
		}

		void
		GL32DeviceContext::startDebugControl() noexcept
		{
		#if !defined(__ANDROID__) && !defined(__AMD__)
			if (!GL32Types::isSupportFeature(GL32Features::GL32_KHR_debug))
			{
				GL_PLATFORM_LOG("Can't support GL_KHR_debug.");
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

			GL_CHECK(glEnable(GL_DEBUG_OUTPUT));

			GL_CHECK(glDebugMessageCallback(debugCallBack, this));

			// enable all
			GL_CHECK(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE));
		#endif
		}

		void
		GL32DeviceContext::enableDebugControl(bool enable) noexcept
		{
			if (enable)
			{
				_needEnableDebugControl = true;
				_needDisableDebugControl = false;
			}
			else
			{
				_needEnableDebugControl = false;
				_needDisableDebugControl = true;
			}
		}

		void
		GL32DeviceContext::stopDebugControl() noexcept
		{
		#if !defined(__ANDROID__) && !defined(__AMD__)
			if (!GL32Types::isSupportFeature(GL32Features::GL32_KHR_debug))
			{
				GL_CHECK(glDisable(GL_DEBUG_OUTPUT));
			}
			else
			{
				GL_PLATFORM_LOG("Can't support GL_KHR_debug.");
			}
		#endif
		}

		bool
		GL32DeviceContext::initStateSystem() noexcept
		{
			GL_CHECK(glClearDepthf(1.0));
			GL_CHECK(glClearColor(0.0, 0.0, 0.0, 0.0));
			GL_CHECK(glClearStencil(0));

			GL_CHECK(glViewport(0, 0, 0, 0));

			GL_CHECK(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));

			GL_CHECK(glEnable(GL_DEPTH_TEST));
			GL_CHECK(glDepthMask(GL_TRUE));
			GL_CHECK(glDepthFunc(GL_LEQUAL));

			GL_CHECK(glEnable(GL_CULL_FACE));
			GL_CHECK(glCullFace(GL_BACK));
			GL_CHECK(glFrontFace(GL_CW));
			GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

			GL_CHECK(glDisable(GL_STENCIL_TEST));
			GL_CHECK(glStencilMask(0xFFFFFFFF));
			GL_CHECK(glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP));
			GL_CHECK(glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF));

			GL_CHECK(glDisable(GL_BLEND));
			GL_CHECK(glBlendEquation(GL_FUNC_ADD));
			GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

			GL_CHECK(glGenVertexArrays(1, &_inputLayout));
			GL_CHECK(glBindVertexArray(_inputLayout));

			auto& deviceProperties = this->getDevice()->getDeviceProperty().getDeviceProperties();
			_vertexBuffers.resize(deviceProperties.maxVertexInputBindings);
			_clearColor.resize(deviceProperties.maxFramebufferColorAttachments, float4(0.0f, 0.0f, 0.0f, 0.0f));

			GraphicsColorBlends blends(deviceProperties.maxFramebufferColorAttachments);
			_stateCaptured.setColorBlends(blends);
			return true;
		}

		void
		GL32DeviceContext::debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept
		{
			if (severity == GL_DEBUG_SEVERITY_LOW)
				return;

			if (severity == GL_DEBUG_SEVERITY_MEDIUM)
				return;

			if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
				return;

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
		GL32DeviceContext::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL32DeviceContext::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}