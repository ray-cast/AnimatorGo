#include "gl33_framebuffer.h"
#include "gl33_texture.h"
#include "gl33_device.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(GL33Framebuffer, GraphicsFramebuffer, "GL33Framebuffer")
		OctoonImplementSubClass(GL33FramebufferLayout, GraphicsFramebufferLayout, "GL33FramebufferLayout")

		GL33FramebufferLayout::GL33FramebufferLayout() noexcept
		{
		}

		GL33FramebufferLayout::~GL33FramebufferLayout() noexcept
		{
			this->close();
		}

		bool
		GL33FramebufferLayout::setup(const GraphicsFramebufferLayoutDesc& framebufferDesc) noexcept
		{
			_framebufferLayoutDesc = framebufferDesc;
			return true;
		}

		void
		GL33FramebufferLayout::close() noexcept
		{
		}

		const GraphicsFramebufferLayoutDesc&
		GL33FramebufferLayout::getFramebufferLayoutDesc() const noexcept
		{
			return _framebufferLayoutDesc;
		}

		void
		GL33FramebufferLayout::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL33FramebufferLayout::getDevice() noexcept
		{
			return _device.lock();
		}

		GL33Framebuffer::GL33Framebuffer() noexcept
			: _fbo(GL_NONE)
		{
		}

		GL33Framebuffer::~GL33Framebuffer() noexcept
		{
			this->close();
		}

		bool
		GL33Framebuffer::setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept
		{
			assert(GL_NONE == _fbo);
			assert(framebufferDesc.getFramebufferLayout());
			assert(framebufferDesc.getFramebufferLayout()->isInstanceOf<GL33FramebufferLayout>());
			assert(framebufferDesc.getWidth() > 0 && framebufferDesc.getHeight() > 0);

			glGenFramebuffers(1, &_fbo);
			if (_fbo == GL_NONE)
			{
				this->getDevice()->downcast<GL33Device>()->message("glCreateFramebuffers() fail.");
				return false;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

			GLenum drawCount = 0;
			GLenum drawBuffers[GL_COLOR_ATTACHMENT15 - GL_COLOR_ATTACHMENT0];

			const auto& textureComponents = framebufferDesc.getFramebufferLayout()->getFramebufferLayoutDesc().getComponents();
			const auto& colorAttachments = framebufferDesc.getColorAttachments();
			if (colorAttachments.size() > (sizeof(drawBuffers) / sizeof(drawBuffers[0])))
			{
				this->getDevice()->downcast<GL33Device>()->message("The color attachment in framebuffer is out of range.");
				return false;
			}

			for (std::size_t i = 0; i < textureComponents.size(); i++)
			{
				auto type = textureComponents[i].getAttachType();
				switch (type)
				{
				case GraphicsImageLayout::General:
					break;
				case GraphicsImageLayout::ColorAttachmentOptimal:
				{
					GLint slot = GL_COLOR_ATTACHMENT0 + textureComponents[i].getAttachSlot();
					GLint mipLevel = colorAttachments[drawCount].getBindingLevel();
					GLint layer = colorAttachments[drawCount].getBindingLayer();

					if (!this->bindRenderTexture(colorAttachments[drawCount].getBindingTexture(), slot, mipLevel, layer))
						return false;

					drawBuffers[drawCount++] = slot;
				}
				break;
				case GraphicsImageLayout::DepthStencilAttachmentOptimal:
				case GraphicsImageLayout::DepthStencilReadOnlyOptimal:
				{
					const auto& depthStencilAttachment = framebufferDesc.getDepthStencilAttachment();
					if (!depthStencilAttachment.getBindingTexture())
					{
						this->getDevice()->downcast<GL33Device>()->message("Need depth or stencil texture.");
						return false;
					}

					auto texture = depthStencilAttachment.getBindingTexture();
					auto format = texture->getGraphicsTextureDesc().getTexFormat();
					auto level = depthStencilAttachment.getBindingLevel();
					auto layer = depthStencilAttachment.getBindingLayer();

					if (GL33Types::isDepthStencilFormat(format))
					{
						if (!this->bindRenderTexture(texture, GL_DEPTH_STENCIL_ATTACHMENT, level, layer))
							return false;
					}
					else if (GL33Types::isDepthFormat(format))
					{
						if (!this->bindRenderTexture(texture, GL_DEPTH_ATTACHMENT, level, layer))
							return false;
					}
					else if (GL33Types::isStencilFormat(format))
					{
						if (!this->bindRenderTexture(texture, GL_STENCIL_ATTACHMENT, level, layer))
							return false;
					}
					else
					{
						this->getDevice()->downcast<GL33Device>()->message("Invalid texture format");
						return false;
					}
				}
				case GraphicsImageLayout::ShaderReadOnlyOptimal:
					break;
				case GraphicsImageLayout::TransferSrcOptimal:
					break;
				case GraphicsImageLayout::TransferDstOptimal:
					break;
				case GraphicsImageLayout::Preinitialized:
					break;
				case GraphicsImageLayout::PresentSrcKhr:
					break;
				default:
					break;
				}
			}

			glDrawBuffers(drawCount, drawBuffers);
			bool error = GL33Check::checkError();

			glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

			_framebufferDesc = framebufferDesc;
			return error;
		}

		void
		GL33Framebuffer::close() noexcept
		{
			if (_fbo != GL_NONE)
			{
				glDeleteFramebuffers(1, &_fbo);
				_fbo = GL_NONE;
			}
		}

		GLuint
		GL33Framebuffer::getInstanceID() noexcept
		{
			return _fbo;
		}

		bool
		GL33Framebuffer::bindRenderTexture(GraphicsTexturePtr renderTexture, GLenum attachment, GLint level, GLint layer) noexcept
		{
			assert(renderTexture);

			auto texture = renderTexture->downcast<GL33Texture>();
			auto textureID = texture->getInstanceID();
			auto textureTarget = texture->getTarget();
			auto& textureDesc = renderTexture->getGraphicsTextureDesc();

			if (textureDesc.getTexDim() == GraphicsTextureDim::Texture2DArray || textureDesc.getTexDim() == GraphicsTextureDim::Texture2DArrayMultisample)
				glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, textureID, level, layer);
			else if (textureDesc.getTexDim() == GraphicsTextureDim::Cube)
				glFramebufferTexture(GL_FRAMEBUFFER, attachment, textureID, level);
			else if (textureDesc.getTexDim() == GraphicsTextureDim::CubeArray)
				glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, textureTarget, textureID, level, layer);
			else
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textureTarget, textureID, level);

			return GL33Check::checkError();
		}

		const GraphicsFramebufferDesc&
		GL33Framebuffer::getGraphicsFramebufferDesc() const noexcept
		{
			return _framebufferDesc;
		}

		void
		GL33Framebuffer::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL33Framebuffer::getDevice() noexcept
		{
			return _device.lock();
		}
}
}