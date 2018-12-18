#include "gl32_framebuffer.h"
#include "gl32_texture.h"

namespace octoon 
{
	namespace graphics
	{
		OctoonImplementSubClass(GL32Framebuffer, GraphicsFramebuffer, "GL32Framebuffer")
		OctoonImplementSubClass(GL32FramebufferLayout, GraphicsFramebufferLayout, "GL32FramebufferLayout")

		GL32FramebufferLayout::GL32FramebufferLayout() noexcept
		{
		}

		GL32FramebufferLayout::~GL32FramebufferLayout() noexcept
		{
			this->close();
		}

		bool
		GL32FramebufferLayout::setup(const GraphicsFramebufferLayoutDesc& framebufferDesc) noexcept
		{
			_framebufferLayoutDesc = framebufferDesc;
			return true;
		}

		void
		GL32FramebufferLayout::close() noexcept
		{
		}

		const GraphicsFramebufferLayoutDesc&
		GL32FramebufferLayout::getFramebufferLayoutDesc() const noexcept
		{
			return _framebufferLayoutDesc;
		}

		void
		GL32FramebufferLayout::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL32FramebufferLayout::getDevice() noexcept
		{
			return _device.lock();
		}

		GL32Framebuffer::GL32Framebuffer() noexcept
			: _fbo(GL_NONE)
		{
		}

		GL32Framebuffer::~GL32Framebuffer() noexcept
		{
			this->close();
		}

		bool
		GL32Framebuffer::setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept
		{
			assert(GL_NONE == _fbo);
			assert(framebufferDesc.getFramebufferLayout());
			assert(framebufferDesc.getFramebufferLayout()->isInstanceOf<GL32FramebufferLayout>());
			assert(framebufferDesc.getWidth() > 0 && framebufferDesc.getHeight() > 0);

			GL_CHECK(glGenFramebuffers(1, &_fbo));
			if (_fbo == GL_NONE)
			{
				GL_PLATFORM_LOG("glCreateFramebuffers() fail.");
				return false;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

			GLenum drawCount = 0;
			GLenum drawBuffers[GL_COLOR_ATTACHMENT15 - GL_COLOR_ATTACHMENT0];

			const auto& textureComponents = framebufferDesc.getFramebufferLayout()->getFramebufferLayoutDesc().getComponents();
			const auto& colorAttachments = framebufferDesc.getColorAttachments();
			if (colorAttachments.size() > (sizeof(drawBuffers) / sizeof(drawBuffers[0])))
			{
				GL_PLATFORM_LOG("The color attachment in framebuffer is out of range.");
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
						GL_PLATFORM_LOG("Need depth or stencil texture.");
						return false;
					}

					auto texture = depthStencilAttachment.getBindingTexture();
					auto format = texture->getTextureDesc().getTexFormat();
					auto level = depthStencilAttachment.getBindingLevel();
					auto layer = depthStencilAttachment.getBindingLayer();

					if (GL32Types::isDepthStencilFormat(format))
					{
						if (!this->bindRenderTexture(texture, GL_DEPTH_STENCIL_ATTACHMENT, level, layer))
							return false;
					}
					else if (GL32Types::isDepthFormat(format))
					{
						if (!this->bindRenderTexture(texture, GL_DEPTH_ATTACHMENT, level, layer))
							return false;
					}
					else if (GL32Types::isStencilFormat(format))
					{
						if (!this->bindRenderTexture(texture, GL_STENCIL_ATTACHMENT, level, layer))
							return false;
					}
					else
					{
						GL_PLATFORM_LOG("Invalid texture format");
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

			GL_CHECK(glDrawBuffers(drawCount, drawBuffers));
			GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE));

			_framebufferDesc = framebufferDesc;
			return GL32Check::checkError();
		}

		void
		GL32Framebuffer::close() noexcept
		{
			if (_fbo != GL_NONE)
			{
				glDeleteFramebuffers(1, &_fbo);
				_fbo = GL_NONE;
			}
		}

		GLuint
		GL32Framebuffer::getInstanceID() noexcept
		{
			return _fbo;
		}

		bool
		GL32Framebuffer::bindRenderTexture(GraphicsTexturePtr renderTexture, GLenum attachment, GLint level, GLint layer) noexcept
		{
			assert(renderTexture);

			auto texture = renderTexture->downcast<GL32Texture>();
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

			return GL32Check::checkError();
		}

		const GraphicsFramebufferDesc&
		GL32Framebuffer::getGraphicsFramebufferDesc() const noexcept
		{
			return _framebufferDesc;
		}

		void
		GL32Framebuffer::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL32Framebuffer::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}
