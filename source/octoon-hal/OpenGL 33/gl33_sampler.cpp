#include "gl33_sampler.h"
#include "gl33_device.h"

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL33Sampler, GraphicsSampler, "GL33Sampler")

		GL33Sampler::GL33Sampler() noexcept
			: _sampler(GL_NONE)
		{
		}

		GL33Sampler::~GL33Sampler() noexcept
		{
			this->close();
		}

		bool
		GL33Sampler::setup(const GraphicsSamplerDesc& samplerDesc) except
		{
			assert(_sampler == GL_NONE);

			glGenSamplers(1, &_sampler);
			if (_sampler == GL_NONE)
			{
				this->getDevice()->downcast<GL33Device>()->message("glGenSamplers() fail");
				return false;
			}

			GraphicsSamplerWrap wrap = samplerDesc.getSamplerWrap();
			if (GraphicsSamplerWrap::Repeat == wrap)
			{
				glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_R, GL_REPEAT);
			}
			else if (GraphicsSamplerWrap::ClampToEdge == wrap)
			{
				glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			}
			else if (GraphicsSamplerWrap::Mirror == wrap)
			{
				glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
			}
			else
			{
				this->getDevice()->downcast<GL33Device>()->message("Invalid SamplerWrap");
				return false;
			}

			GLenum min = GL33Types::asSamplerMinFilter(samplerDesc.getSamplerMinFilter());
			GLenum mag = GL33Types::asSamplerMagFilter(samplerDesc.getSamplerMagFilter());

			if (min != GL_INVALID_ENUM && mag != GL_INVALID_ENUM)
			{
				glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, mag);
				glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, min);
			}
			else
			{
				this->getDevice()->downcast<GL33Device>()->message("Invalid SamplerFilter");
				return false;
			}

			if (GLEW_EXT_texture_filter_anisotropic)
			{
				GraphicsSamplerAnis anis = samplerDesc.getSamplerAnis();

				if (anis == GraphicsSamplerAnis::Anis1)
					glSamplerParameteri(_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
				else if (anis == GraphicsSamplerAnis::Anis2)
					glSamplerParameteri(_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2);
				else if (anis == GraphicsSamplerAnis::Anis4)
					glSamplerParameteri(_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
				else if (anis == GraphicsSamplerAnis::Anis8)
					glSamplerParameteri(_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
				else if (anis == GraphicsSamplerAnis::Anis16)
					glSamplerParameteri(_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
				else if (anis != GraphicsSamplerAnis::Anis0)
				{
					this->getDevice()->downcast<GL33Device>()->message("Invalid SamplerAnis");
					return false;
				}
			}

			_sampleDesc = samplerDesc;
			return true;
		}

		void
		GL33Sampler::close() noexcept
		{
			if (_sampler)
			{
				glDeleteSamplers(1, &_sampler);
				_sampler = 0;
			}
		}

		GLuint
		GL33Sampler::getInstanceID() noexcept
		{
			return _sampler;
		}

		const GraphicsSamplerDesc&
		GL33Sampler::getSamplerDesc() const noexcept
		{
			return _sampleDesc;
		}

		void
		GL33Sampler::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL33Sampler::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}