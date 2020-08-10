#include "rtx_manager.h"
#include <RadeonProRender.h>

#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#include <OpenGL/OpenGL.h>
#elif WIN32
#include <Windows.h>
#include "GL/glew.h"
#else
#include <CL/cl.h>
#include <GL/glew.h>
#include <GL/glx.h>
#endif

namespace octoon::video
{
	RtxManager::RtxManager()
	{
		std::vector<CLWPlatform> platforms;

		CLWPlatform::CreateAllPlatforms(platforms);

		if (platforms.size() == 0)
		{
			throw std::runtime_error("No OpenCL platforms installed.");
		}

		this->configs_.clear();

		static const std::vector<rpr_uint> kGpuFlags =
		{
			RPR_CREATION_FLAGS_ENABLE_GPU0,
			RPR_CREATION_FLAGS_ENABLE_GPU1,
			RPR_CREATION_FLAGS_ENABLE_GPU2,
			RPR_CREATION_FLAGS_ENABLE_GPU3,
			RPR_CREATION_FLAGS_ENABLE_GPU4,
			RPR_CREATION_FLAGS_ENABLE_GPU5,
			RPR_CREATION_FLAGS_ENABLE_GPU6,
			RPR_CREATION_FLAGS_ENABLE_GPU7
		};

		rpr_creation_flags flags = RPR_CREATION_FLAGS_ENABLE_GPU0 | RPR_CREATION_FLAGS_ENABLE_GPU1 | RPR_CREATION_FLAGS_ENABLE_GL_INTEROP;
		bool use_cpu = (flags & RPR_CREATION_FLAGS_ENABLE_CPU) == RPR_CREATION_FLAGS_ENABLE_CPU;
		bool interop = (flags & RPR_CREATION_FLAGS_ENABLE_GL_INTEROP) == RPR_CREATION_FLAGS_ENABLE_GL_INTEROP;
		bool hasprimary = false;

		rpr_uint gpu_counter = 0;

		for (std::size_t i = 0; i < platforms.size(); ++i)
		{
			for (unsigned int d = 0; d < platforms[i].GetDeviceCount(); ++d)
			{
				cl_device_type device_type = platforms[i].GetDevice(d).GetType();

				if (device_type == CL_DEVICE_TYPE_GPU)
				{
					bool comp = (kGpuFlags[gpu_counter] & flags) == kGpuFlags[gpu_counter];
					++gpu_counter;
					if (!comp)
					{
						continue;
					}
				}

				if (!use_cpu && (device_type == CL_DEVICE_TYPE_CPU))
				{
					continue;
				}

				Config cfg;
				cfg.caninterop = false;

				if (interop)
				{
					if (platforms[i].GetName().find("Intel") != std::string::npos)
						continue;
				}

				if (platforms[i].GetDevice(d).HasGlInterop() && !hasprimary && interop)
				{
#ifdef WIN32
					cl_context_properties props[] =
					{
						CL_CONTEXT_PLATFORM, (cl_context_properties)((cl_platform_id)platforms[i]),
						CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
						CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
						0
					};
#elif __linux__
					cl_context_properties props[] =
					{
						CL_CONTEXT_PLATFORM, (cl_context_properties)((cl_platform_id)platforms[i]),
						CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
						CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
						0
					};
#elif __APPLE__
					CGLContextObj kCGLContext = CGLGetCurrentContext();
					CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
					cl_context_properties props[] = { CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup, 0};
#endif
					cfg.context = CLWContext::Create(platforms[i].GetDevice(d), props);
					cfg.type = kPrimary;
					cfg.caninterop = true;
					hasprimary = true;
				}
				else
				{
					cfg.context = CLWContext::Create(platforms[i].GetDevice(d));
					cfg.type = kSecondary;
				}

				this->configs_.push_back(std::move(cfg));
			}
		}

		if (this->configs_.size() == 0)
		{
			throw std::runtime_error("No devices was selected (probably device index type does not correspond with real device type).");
		}

		if (!hasprimary)
		{
			this->configs_[0].type = kPrimary;
		}

		for (std::size_t i = 0; i < this->configs_.size(); ++i)
		{
			this->configs_[i].factory = std::make_unique<ClwRenderFactory>(this->configs_[i].context, "cache");
			this->configs_[i].controller = this->configs_[i].factory->createSceneController();
			this->configs_[i].pipeline = this->configs_[i].factory->createPipeline();
		}
	}

	void
	RtxManager::createFrameBufferFromGLTexture(GLenum target, GLint miplevel, GLuint texture)
	{
		if (target != GL_TEXTURE_2D)
		{
			throw std::runtime_error("Unsupported GL texture target.");
		}

		GLint width, height;
		GLint backup_tex = 0;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &backup_tex);
		glBindTexture(target, texture);
		glGetTexLevelParameteriv(target, miplevel, GL_TEXTURE_WIDTH, (GLint*)&width);
		glGetTexLevelParameteriv(target, miplevel, GL_TEXTURE_HEIGHT, (GLint*)&height);

		//create interop image
		this->configs_[0].context.CreateImage2DFromGLTexture(texture);
		glBindTexture(target, backup_tex);
	}

	void
	RtxManager::setRenderScene(RenderScene* scene) noexcept
	{
		this->scene_ = scene;
	}

	const RenderScene*
	RtxManager::getRenderScene() const noexcept
	{
		return this->scene_;
	}

	void
	RtxManager::prepareScene(RenderScene* scene) noexcept
	{
		for (auto& c : configs_)
			c.controller->compileScene(scene);
	}

	void
	RtxManager::render(RenderScene* scene)
	{
		this->prepareScene(scene);

		for (auto& c : configs_)
			c.pipeline->render(c.controller->getCachedScene(scene));
	}
}