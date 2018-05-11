#include "gl32_sampler.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(GL32Sampler, GraphicsSampler, "GL32Sampler")

		GL32Sampler::GL32Sampler() noexcept
			: _sampler(GL_NONE)
		{
		}

		GL32Sampler::~GL32Sampler() noexcept
		{
			this->close();
		}

		bool
		GL32Sampler::setup(const GraphicsSamplerDesc& samplerDesc) except
		{
			assert(_sampler == GL_NONE);

			glGenSamplers(1, &_sampler);
			if (_sampler == GL_NONE)
			{
				GL_PLATFORM_LOG("glGenSamplers() fail");
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
				GL_PLATFORM_LOG("Invalid SamplerWrap");
				return false;
			}

			GraphicsSamplerFilter filter = samplerDesc.getSamplerFilter();
			if (filter == GraphicsSamplerFilter::Nearest)
			{
				glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			}
			else if (filter == GraphicsSamplerFilter::Linear)
			{
				glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			else if (filter == GraphicsSamplerFilter::NearestMipmapLinear)
			{
				glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			}
			else if (filter == GraphicsSamplerFilter::NearestMipmapNearest)
			{
				glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			}
			else if (filter == GraphicsSamplerFilter::LinearMipmapNearest)
			{
				glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			}
			else if (filter == GraphicsSamplerFilter::LinearMipmapLinear)
			{
				glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			}
			else
			{
				GL_PLATFORM_LOG("Invalid SamplerFilter");
				return false;
			}

			_sampleDesc = samplerDesc;
			return true;
		}

		void
		GL32Sampler::close() noexcept
		{
			if (_sampler)
			{
				glDeleteSamplers(1, &_sampler);
				_sampler = 0;
			}
		}

		GLuint
		GL32Sampler::getInstanceID() noexcept
		{
			return _sampler;
		}

		const GraphicsSamplerDesc&
		GL32Sampler::getGraphicsSamplerDesc() const noexcept
		{
			return _sampleDesc;
		}

		void
		GL32Sampler::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL32Sampler::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}