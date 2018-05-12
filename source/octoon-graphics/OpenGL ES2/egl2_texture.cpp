#include "egl2_texture.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(EGL2Texture, GraphicsTexture, "EGL2Texture")

		EGL2Texture::EGL2Texture() noexcept
			: _texture(GL_NONE)
			, _target(GL_INVALID_ENUM)
		{
		}

		EGL2Texture::~EGL2Texture() noexcept
		{
			this->close();
		}

		bool
		EGL2Texture::setup(const GraphicsTextureDesc& textureDesc) noexcept
		{
			assert(_texture == GL_NONE);

			GLenum target = EGL2Types::asTextureTarget(textureDesc.getTexDim());
			if (target == GL_INVALID_ENUM)
				return false;

			GLenum internalFormat = EGL2Types::asTextureInternalFormat(textureDesc.getTexFormat());
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

			if (!applySamplerWrap(target, textureDesc.getSamplerWrap()))
				return false;

			if (!applySamplerFilter(target, textureDesc.getSamplerMinFilter(), textureDesc.getSamplerMagFilter()))
				return false;

			if (!applySamplerAnis(target, textureDesc.getSamplerAnis()))
				return false;

			const char* stream = (const char*)textureDesc.getStream();
			if (EGL2Types::isCompressedTexture(textureDesc.getTexFormat()))
			{
				std::size_t offset = 0;

				GLint oldPackStore = 1;
				glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldPackStore);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 8);

				for (GLint mip = mipBase; mip < mipBase + mipLevel; mip++)
				{
					GLsizei w = std::max(width / (1 << mip), 1);
					GLsizei h = std::max(height / (1 << mip), 1);
					GLsizei mipSize = EGL2Types::getCompressedTextureSize(w, h, 1, internalFormat);

					GL_CHECK(glCompressedTexImage2D(GL_TEXTURE_2D, mip, internalFormat, w, h, 0, mipSize, (char*)stream + offset));

					offset += stream ? mipSize : 0;
				}

				glPixelStorei(GL_UNPACK_ALIGNMENT, oldPackStore);
			}
			else
			{
				GLenum format = EGL2Types::asTextureFormat(textureDesc.getTexFormat());
				GLenum type = EGL2Types::asTextureType(textureDesc.getTexFormat());

				GLsizei offset = 0;
				GLsizei pixelSize = EGL2Types::getFormatNum(format, type);

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
						if (target == GL_TEXTURE_2D || target == GL_TEXTURE_2D_MULTISAMPLE)
						{
							GL_CHECK(glTexImage2D(target, mip, internalFormat, w, h, 0, format, type, (char*)stream + offset));
							offset += mipSize;
						}
						else
						{
							if (target == GL_TEXTURE_CUBE_MAP || target == GL_TEXTURE_CUBE_MAP_ARRAY_EXT)
							{
								for (std::size_t i = 0; i < 6; i++)
								{
									if (target == GL_TEXTURE_CUBE_MAP)
										GL_CHECK(glTexImage2D(cubeFace[i], mip, internalFormat, w, h, 0, format, type, (char*)stream + offset));
									else
									{
		#ifndef __AMD__
										GL_CHECK(glTexImage3DOES(cubeFace[i], mip, internalFormat, w, h, layer, 0, format, type, (char*)stream + offset));
		#else
										GL_PLATFORM_LOG("Can't support cubemap array");
										return false;
		#endif
									}

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

			return EGL2Check::checkError();
		}

		void
		EGL2Texture::close() noexcept
		{
			if (_texture != GL_NONE)
			{
				glDeleteTextures(1, &_texture);
				_texture = GL_NONE;
			}
		}

		GLenum
		EGL2Texture::getTarget() const noexcept
		{
			return _target;
		}

		GLuint
		EGL2Texture::getInstanceID() const noexcept
		{
			return _texture;
		}

		bool
		EGL2Texture::map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint32_t mipLevel, void** data) noexcept
		{
			assert(false);
			return false;
		}

		void
		EGL2Texture::unmap() noexcept
		{
		}

		bool
		EGL2Texture::applySamplerWrap(GLenum target, GraphicsSamplerWrap wrap) noexcept
		{
			GLenum glwrap = EGL2Types::asSamplerWrap(wrap);
			if (glwrap != GL_INVALID_ENUM)
			{
				GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_S, glwrap));
				GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_T, glwrap));

				return true;
			}

			return false;
		}

		bool
		EGL2Texture::applySamplerFilter(GLenum target, GraphicsSamplerFilter minFilter, GraphicsSamplerFilter magFilter) noexcept
		{
			GLenum min = EGL2Types::asSamplerMinFilter(minFilter);
			GLenum mag = EGL2Types::asSamplerMagFilter(magFilter);
			if (min != GL_INVALID_ENUM && mag != GL_INVALID_ENUM)
			{
				GL_CHECK(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min));
				GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag));
				return true;
			}

			return false;
		}

		bool
		EGL2Texture::applySamplerAnis(GLenum target, GraphicsSamplerAnis anis) noexcept
		{
			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_filter_anisotropic))
			{
				if (anis == GraphicsSamplerAnis::Anis1)
					GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1));
				else if (anis == GraphicsSamplerAnis::Anis2)
					GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2));
				else if (anis == GraphicsSamplerAnis::Anis4)
					GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4));
				else if (anis == GraphicsSamplerAnis::Anis8)
					GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8));
				else if (anis == GraphicsSamplerAnis::Anis16)
					GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16));
				else
				{
					GL_PLATFORM_LOG("Can't support anisotropy format");
					return false;
				}
			}
			return true;
		}

		const GraphicsTextureDesc&
		EGL2Texture::getGraphicsTextureDesc() const noexcept
		{
			return _textureDesc;
		}

		void
		EGL2Texture::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		EGL2Texture::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}