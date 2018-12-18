#include "gl30_framebuffer.h"
#include "gl30_texture.h"
#include "gl30_device.h"

namespace octoon 
{
	namespace graphics
	{
		OctoonImplementSubClass(GL30Framebuffer, GraphicsFramebuffer, "GL30Framebuffer")
		OctoonImplementSubClass(GL30FramebufferLayout, GraphicsFramebufferLayout, "GL30FramebufferLayout")

		GL30FramebufferLayout::GL30FramebufferLayout() noexcept
		{
		}

		GL30FramebufferLayout::~GL30FramebufferLayout() noexcept
		{
			this->close();
		}

		bool
		GL30FramebufferLayout::setup(const GraphicsFramebufferLayoutDesc& framebufferDesc) noexcept
		{
			_framebufferLayoutDesc = framebufferDesc;
			return true;
		}

		void
		GL30FramebufferLayout::close() noexcept
		{
		}

		const GraphicsFramebufferLayoutDesc&
		GL30FramebufferLayout::getFramebufferLayoutDesc() const noexcept
		{
			return _framebufferLayoutDesc;
		}

		void
		GL30FramebufferLayout::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL30FramebufferLayout::getDevice() noexcept
		{
			return _device.lock();
		}

		GL30Framebuffer::GL30Framebuffer() noexcept
			: _fbo(GL_NONE)
		{
		}

		GL30Framebuffer::~GL30Framebuffer() noexcept
		{
			this->close();
		}

		bool
		GL30Framebuffer::setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept
		{
			assert(GL_NONE == _fbo);
			assert(framebufferDesc.getFramebufferLayout());
			assert(framebufferDesc.getFramebufferLayout()->isInstanceOf<GL30FramebufferLayout>());
			assert(framebufferDesc.getWidth() > 0 && framebufferDesc.getHeight() > 0);

			glGenFramebuffers(1, &_fbo);
			if (_fbo == GL_NONE)
			{
				this->getDevice()->downcast<GL30Device>()->message("glCreateFramebuffers() fail.");
				return false;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

			GLenum drawCount = 0;
			GLenum drawBuffers[GL_COLOR_ATTACHMENT15 - GL_COLOR_ATTACHMENT0];

			const auto& textureComponents = framebufferDesc.getFramebufferLayout()->getFramebufferLayoutDesc().getComponents();
			const auto& colorAttachments = framebufferDesc.getColorAttachments();
			if (colorAttachments.size() > (sizeof(drawBuffers) / sizeof(drawBuffers[0])))
			{
				this->getDevice()->downcast<GL30Device>()->message("The color attachment in framebuffer is out of range.");
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
						this->getDevice()->downcast<GL30Device>()->message("Need depth or stencil texture.");
						return false;
					}

					auto texture = depthStencilAttachment.getBindingTexture();
					auto format = texture->getTextureDesc().getTexFormat();
					auto level = depthStencilAttachment.getBindingLevel();
					auto layer = depthStencilAttachment.getBindingLayer();

					if (GL30Types::isDepthStencilFormat(format))
					{
						if (!this->bindRenderTexture(texture, GL_DEPTH_STENCIL_ATTACHMENT, level, layer))
							return false;
					}
					else if (GL30Types::isDepthFormat(format))
					{
						if (!this->bindRenderTexture(texture, GL_DEPTH_ATTACHMENT, level, layer))
							return false;
					}
					else if (GL30Types::isStencilFormat(format))
					{
						if (!this->bindRenderTexture(texture, GL_STENCIL_ATTACHMENT, level, layer))
							return false;
					}
					else
					{
						this->getDevice()->downcast<GL30Device>()->message("Invalid texture format");
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
			bool error = GL30Check::checkError();

			glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

			_framebufferDesc = framebufferDesc;
			return error;
		}

		void
		GL30Framebuffer::close() noexcept
		{
			if (_fbo != GL_NONE)
			{
				glDeleteFramebuffers(1, &_fbo);
				_fbo = GL_NONE;
			}
		}

		GLuint
		GL30Framebuffer::getInstanceID() noexcept
		{
			return _fbo;
		}

		bool
		GL30Framebuffer::bindRenderTexture(GraphicsTexturePtr renderTexture, GLenum attachment, GLint level, GLint layer) noexcept
		{
			assert(renderTexture);

			auto texture = renderTexture->downcast<GL30Texture>();
			auto textureID = texture->getInstanceID();
			auto textureTarget = texture->getTarget();
			auto& textureDesc = renderTexture->getTextureDesc();

			if (layer > 0)
			{
				if (textureDesc.getTexDim() != GraphicsTextureDim::Texture2D ||
					textureDesc.getTexDim() != GraphicsTextureDim::Texture2DMultisample ||
					textureDesc.getTexDim() != GraphicsTextureDim::Cube ||
					textureDesc.getTexDim() != GraphicsTextureDim::CubeArray)
				{
					GL_PLATFORM_LOG("Invalid texture target");
					return false;
				}
		
				GL_CHECK(glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, textureID, level, layer));
			}
			else
			{
				GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textureTarget, textureID, level));
			}

			return GL30Check::checkError();
		}

		const GraphicsFramebufferDesc&
		GL30Framebuffer::getGraphicsFramebufferDesc() const noexcept
		{
			return _framebufferDesc;
		}

		void
		GL30Framebuffer::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL30Framebuffer::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}
