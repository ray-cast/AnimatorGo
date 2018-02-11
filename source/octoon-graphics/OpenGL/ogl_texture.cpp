#include "ogl_texture.h"
#include "ogl_device.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(OGLTexture, GraphicsTexture, "OGLTexture")

		OGLTexture::OGLTexture() noexcept
			: _target(GL_INVALID_ENUM)
			, _pbo(GL_NONE)
			, _pboSize(0)
			, _upbo(GL_NONE)
			, _upboSize(0)
			, _texture(GL_NONE)
		{
		}

		OGLTexture::~OGLTexture() noexcept
		{
			this->close();
		}

		bool
		OGLTexture::setup(const GraphicsTextureDesc& textureDesc) noexcept
		{
			assert(_texture == GL_NONE);

			GLenum target = OGLTypes::asTextureTarget(textureDesc.getTexDim(), textureDesc.isMultiSample());
			if (target == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Invalid texture target");
				return false;
			}

			GLenum internalFormat = OGLTypes::asTextureInternalFormat(textureDesc.getTexFormat());
			if (internalFormat == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Invalid texture internal format.");
				return false;
			}

			glGenTextures(1, &_texture);
			if (_texture == GL_NONE)
			{
				this->getDevice()->downcast<OGLDevice>()->message("glGenTextures() fail");
				return false;
			}

			glBindTexture(target, _texture);

			GLsizei width = (GLsizei)textureDesc.getWidth();
			GLsizei height = (GLsizei)textureDesc.getHeight();
			GLsizei depth = (GLsizei)textureDesc.getDepth();

			GLsizei mipBase = textureDesc.getMipBase();
			GLsizei mipLevel = textureDesc.getMipNums();

			if (!applySamplerWrap(target, textureDesc.getSamplerWrap()))
				return false;

			if (!applySamplerFilter(target, textureDesc.getSamplerMinFilter(), textureDesc.getSamplerMagFilter()))
				return false;

			if (!applySamplerAnis(target, textureDesc.getSamplerAnis()))
				return false;

			if (!applyMipmapLimit(target, mipBase, mipLevel))
				return false;

			const char* stream = (const char*)textureDesc.getStream();
			if (OGLTypes::isCompressedTexture(textureDesc.getTexFormat()))
			{
				std::size_t offset = 0;

				GLint oldPackStore = 1;
				glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldPackStore);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 8);

				for (GLint mip = mipBase; mip < mipBase + mipLevel; mip++)
				{
					GLsizei w = std::max(width / (1 << mip), 1);
					GLsizei h = std::max(height / (1 << mip), 1);
					GLsizei mipSize = OGLTypes::getCompressedTextureSize(w, h, 1, internalFormat);
					if (mipSize > 0)
						glCompressedTexImage2D(GL_TEXTURE_2D, mip, internalFormat, w, h, 0, mipSize, (char*)stream + offset);
					else
						this->getDevice()->downcast<OGLDevice>()->message("bad texformat in compressed_texture_size");

					offset += stream ? mipSize : 0;
				}

				glPixelStorei(GL_UNPACK_ALIGNMENT, oldPackStore);
			}
			else
			{
				GLenum format = OGLTypes::asTextureFormat(textureDesc.getTexFormat());
				if (format == GL_INVALID_ENUM)
				{
					this->getDevice()->downcast<OGLDevice>()->message("Invalid texture format");
					return false;
				}

				GLenum type = OGLTypes::asTextureType(textureDesc.getTexFormat());
				if (type == GL_INVALID_ENUM)
				{
					this->getDevice()->downcast<OGLDevice>()->message("Invalid texture type");
					return false;
				}

				GLsizei offset = 0;
				GLsizei pixelSize = stream ? OGLTypes::getFormatNum(format, type) : 1;

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
					GLsizei layerBase = textureDesc.getLayerBase();
					GLsizei layerLevel = textureDesc.getLayerNums();

					for (GLsizei layer = layerBase; layer < layerBase + layerLevel; layer++)
					{
						if (target == GL_TEXTURE_2D ||
							target == GL_TEXTURE_2D_MULTISAMPLE)
						{
							glTexImage2D(target, mip, internalFormat, w, h, 0, format, type, stream ? stream + offset : nullptr);
						}
						else
						{
							if (target == GL_TEXTURE_CUBE_MAP_ARRAY ||
								target == GL_TEXTURE_CUBE_MAP)
							{
								GLenum cubeFace[] =
								{
									GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
									GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
									GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
								};

								for (GLsizei face = 0; face < 6; face++)
								{
									if (target == GL_TEXTURE_CUBE_MAP)
										glTexImage2D(cubeFace[face], mip, internalFormat, w, h, 0, format, type, stream ? (char*)stream + offset : nullptr);
									else
										glTexImage3D(cubeFace[face], mip, internalFormat, w, h, layer + 1, 0, format, type, stream ? (char*)stream + offset : nullptr);

									offset += mipSize;
								}
							}
							else
							{
								if (target == GL_TEXTURE_2D_ARRAY)
									glTexImage3D(target, mip, internalFormat, w, h, layer + 1, 0, format, type, stream ? (char*)stream + offset : nullptr);
								else
									glTexImage3D(target, mip, internalFormat, w, h, depth, 0, format, type, stream ? (char*)stream + offset : nullptr);

								offset += mipSize;
							}
						}
					}
				}

				glPixelStorei(GL_UNPACK_ALIGNMENT, oldPackStore);
			}

			glBindTexture(target, GL_NONE);

			_target = target;
			_textureDesc = textureDesc;

			return OGLCheck::checkError();
		}

		void
		OGLTexture::close() noexcept
		{
			if (_texture != GL_NONE)
			{
				glDeleteTextures(1, &_texture);
				_texture = GL_NONE;
			}

			if (_pbo != GL_NONE)
			{
				glDeleteBuffers(1, &_pbo);
				_pbo = GL_NONE;
			}

			if (_upbo != GL_NONE)
			{
				glDeleteBuffers(1, &_upbo);
				_upbo = GL_NONE;
			}
		}

		bool
		OGLTexture::map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint32_t mipLevel, void** data) noexcept
		{
			assert(data);

			GLenum format = OGLTypes::asTextureFormat(_textureDesc.getTexFormat());
			if (format == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Invalid texture format");
				return false;
			}

			GLenum type = OGLTypes::asTextureType(_textureDesc.getTexFormat());
			if (type == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Invalid texture type");
				return false;
			}

			if (type == GL_HALF_FLOAT)
				type = GL_FLOAT;

			GLsizei num = OGLTypes::getFormatNum(format, type);
			if (num == 0)
				return false;

			if (_pbo == GL_NONE)
				glGenBuffers(1, &_pbo);

			glBindBuffer(GL_PIXEL_PACK_BUFFER, _pbo);

			GLsizei mapSize = w * h * num;
			if (_pboSize < mapSize)
			{
				glBufferData(GL_PIXEL_PACK_BUFFER, mapSize, nullptr, GL_STREAM_READ);
				_pboSize = mapSize;
			}

			glBindTexture(_target, _texture);
			glGetTexImage(_target, mipLevel, format, type, 0);

			*data = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, mapSize, GL_MAP_READ_BIT);
			data += (y * _textureDesc.getWidth() * num) * x;

			return *data ? true : false;
		}

		void
		OGLTexture::unmap() noexcept
		{
			glUnmapNamedBuffer(_pbo);
		}

		GLenum
		OGLTexture::getTarget() const noexcept
		{
			return _target;
		}

		GLuint
		OGLTexture::getInstanceID() const noexcept
		{
			return _texture;
		}

		bool
		OGLTexture::applyMipmapLimit(GLenum target, std::uint32_t min, std::uint32_t count) noexcept
		{
			glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, min);
			glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, min + count - 1);
			return true;
		}

		bool
		OGLTexture::applySamplerWrap(GLenum target, GraphicsSamplerWrap wrap) noexcept
		{
			GLenum glwrap = OGLTypes::asSamplerWrap(wrap);
			if (glwrap == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Invalid sampler wrap");
				return false;
			}

			glTexParameteri(target, GL_TEXTURE_WRAP_S, glwrap);
			glTexParameteri(target, GL_TEXTURE_WRAP_T, glwrap);
			glTexParameteri(target, GL_TEXTURE_WRAP_R, glwrap);

			return true;
		}

		bool
		OGLTexture::applySamplerFilter(GLenum target, GraphicsSamplerFilter minFilter, GraphicsSamplerFilter magFilter) noexcept
		{
			GLenum min = OGLTypes::asSamplerMinFilter(minFilter);
			GLenum mag = OGLTypes::asSamplerMagFilter(magFilter);
			if (min == GL_INVALID_ENUM || mag == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Invalid sampler filter");
				return false;
			}

			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min);
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag);
			return true;
		}

		bool
		OGLTexture::applySamplerAnis(GLenum target, GraphicsSamplerAnis anis) noexcept
		{
			if (GLEW_EXT_texture_filter_anisotropic)
			{
				if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis1)
					glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
				else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis2)
					glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2);
				else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis4)
					glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
				else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis8)
					glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
				else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis16)
					glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
				else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis32)
					glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 32);
				else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis64)
					glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 64);
			}

			return true;
		}

		const GraphicsTextureDesc&
		OGLTexture::getGraphicsTextureDesc() const noexcept
		{
			return _textureDesc;
		}

		void
		OGLTexture::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		OGLTexture::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}