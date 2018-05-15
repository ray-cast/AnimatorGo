#include "gl20_texture.h"
#include "gl20_sampler.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(GL20Texture, GraphicsTexture, "GL20Texture")

		GL20Texture::GL20Texture() noexcept
			: _texture(GL_NONE)
			, _target(GL_INVALID_ENUM)
		{
		}

		GL20Texture::~GL20Texture() noexcept
		{
			this->close();
		}

		bool
		GL20Texture::setup(const GraphicsTextureDesc& textureDesc) noexcept
		{
			assert(_texture == GL_NONE);

			GLenum target = GL20Types::asTextureTarget(textureDesc.getTexDim());
			if (target == GL_INVALID_ENUM)
				return false;

			GLenum internalFormat = GL20Types::asTextureInternalFormat(textureDesc.getTexFormat());
			if (internalFormat == GL_INVALID_ENUM)
				return false;

			GL_CHECK(glGenTextures(1, &_texture));
			if (_texture == GL_NONE)
			{
				GL_PLATFORM_LOG("glGenTextures() fail");
				return false;
			}

			GL_CHECK(glBindTexture(target, _texture));

			GLsizei width = (GLsizei)textureDesc.getWidth();
			GLsizei height = (GLsizei)textureDesc.getHeight();

			GLsizei mipBase = textureDesc.getMipBase();
			GLsizei mipLevel = textureDesc.getMipNums();

			const char* stream = (const char*)textureDesc.getStream();
			if (GL20Types::isCompressedTexture(textureDesc.getTexFormat()))
			{
				std::size_t offset = 0;

				GLint oldPackStore = 1;
				glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldPackStore);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 8);

				for (GLint mip = mipBase; mip < mipBase + mipLevel; mip++)
				{
					GLsizei w = std::max(width / (1 << mip), 1);
					GLsizei h = std::max(height / (1 << mip), 1);
					GLsizei mipSize = GL20Types::getCompressedTextureSize(w, h, 1, internalFormat);

					GL_CHECK(glCompressedTexImage2D(GL_TEXTURE_2D, mip, internalFormat, w, h, 0, mipSize, (char*)stream + offset));

					offset += stream ? mipSize : 0;
				}

				glPixelStorei(GL_UNPACK_ALIGNMENT, oldPackStore);
			}
			else
			{
				GLenum format = GL20Types::asTextureFormat(textureDesc.getTexFormat());
				GLenum type = GL20Types::asTextureType(textureDesc.getTexFormat());

				GLsizei offset = 0;
				GLsizei pixelSize = stream ? GL20Types::getFormatNum(format, type) : 1;

				GLenum cubeFace[] =
				{
					GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
					GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
					GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
				};

				GLint oldPackStore = 1;
				glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldPackStore);

				if (pixelSize == 1)
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				else if (pixelSize == 2)
					glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
				else if (pixelSize == 4 || pixelSize == 12)
					glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
				else if (pixelSize == 8 || pixelSize == 16)
					glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
				else
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

				for (GLsizei mip = mipBase; mip < mipBase + mipLevel; mip++)
				{
					GLsizei w = std::max(width / (1 << mip), 1);
					GLsizei h = std::max(height / (1 << mip), 1);
					GLsizei mipSize = w * h * pixelSize;
					GLsizei layerBase = textureDesc.getLayerBase() + 1;
					GLsizei layerLevel = textureDesc.getLayerNums();

					for (GLsizei layer = layerBase; layer < layerBase + layerLevel; layer++)
					{
						if (target == GL_TEXTURE_2D)
						{
							GL_CHECK(glTexImage2D(target, mip, internalFormat, w, h, 0, format, type, (char*)stream + offset));
							offset += mipSize;
						}
						else
						{
							if (target == GL_TEXTURE_CUBE_MAP)
							{
								for (std::size_t i = 0; i < 6; i++)
								{
									if (target == GL_TEXTURE_CUBE_MAP)
										GL_CHECK(glTexImage2D(cubeFace[i], mip, internalFormat, w, h, 0, format, type, (char*)stream + offset));

									offset += mipSize;
								}
							}
						}

						w = std::max(w >> 1, 1);
						h = std::max(h >> 1, 1);
					}
				}

				glPixelStorei(GL_UNPACK_ALIGNMENT, oldPackStore);
			}

			GL_CHECK(glBindTexture(target, GL_NONE));

			_target = target;
			_textureDesc = textureDesc;

			return GL20Check::checkError();
		}

		void
		GL20Texture::close() noexcept
		{
			if (_texture != GL_NONE)
			{
				glDeleteTextures(1, &_texture);
				_texture = GL_NONE;
			}
		}

		GLenum
		GL20Texture::getTarget() const noexcept
		{
			return _target;
		}

		GLuint
		GL20Texture::getInstanceID() const noexcept
		{
			return _texture;
		}

		bool
		GL20Texture::map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint32_t mipLevel, void** data) noexcept
		{
			assert(false);
			return false;
		}

		void
		GL20Texture::unmap() noexcept
		{
		}

		void
		GL20Texture::setSampler(const GraphicsSamplerPtr& sampler) noexcept
		{
			if (sampler != _sampler)
			{
				if (sampler)
				{
					auto& desc = sampler->getGraphicsSamplerDesc();
					if (!applySamplerWrap(_target, desc.getSamplerWrap()))
						return;

					if (!applySamplerFilter(_target, desc.getSamplerMinFilter(), desc.getSamplerMagFilter()))
						return;
				}

				_sampler = sampler;
			}
		}

		bool
		GL20Texture::applySamplerWrap(GLenum target, GraphicsSamplerWrap wrap) noexcept
		{
			GLenum glwrap = GL20Types::asSamplerWrap(wrap);
			if (glwrap != GL_INVALID_ENUM)
			{
				GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_S, glwrap));
				GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_T, glwrap));

				return true;
			}

			return false;
		}

		bool
		GL20Texture::applySamplerFilter(GLenum target, GraphicsSamplerFilter minFilter, GraphicsSamplerFilter magFilter) noexcept
		{
			GLenum min = GL20Types::asSamplerMinFilter(minFilter);
			GLenum mag = GL20Types::asSamplerMagFilter(magFilter);
			if (min != GL_INVALID_ENUM && mag != GL_INVALID_ENUM)
			{
				GL_CHECK(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min));
				GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag));
				return true;
			}

			return false;
		}

		const GraphicsTextureDesc&
		GL20Texture::getGraphicsTextureDesc() const noexcept
		{
			return _textureDesc;
		}

		void
		GL20Texture::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL20Texture::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}