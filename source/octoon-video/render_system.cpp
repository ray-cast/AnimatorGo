#include <octoon/video/render_system.h>
#include <octoon/video/camera.h>
#include <octoon/video/geometry.h>
#include <octoon/video/render_scene.h>
#include <octoon/video/text_system.h>
#include <octoon/video/text_material.h>
#include <octoon/runtime/except.h>

#if __WINDOWS__
#include <GL/glew.h>
#include <GL/wglew.h>

typedef BOOL(GLAPIENTRY * PFNWGLSWAPINTERVALEXTPROC) (int interval);
typedef HGLRC(GLAPIENTRY * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);
typedef BOOL(GLAPIENTRY * PFNWGLGETPIXELFORMATATTRIBIVARBPROC) (HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues);
typedef BOOL(WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int* piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

PFNWGLSWAPINTERVALEXTPROC __wglSwapIntervalEXT;
PFNWGLCREATECONTEXTATTRIBSARBPROC   __wglCreateContextAttribsARB;
PFNWGLGETPIXELFORMATATTRIBIVARBPROC __wglGetPixelFormatAttribivARB;
PFNWGLCHOOSEPIXELFORMATARBPROC __wglChoosePixelFormatARB;
#elif __LINUX__
#	include <X11/Xlib.h>
#elif __APPLE__
#	include <OpenGL/OpenGL.h>
#	include <OpenGL/CGLTypes.h>
#endif

using namespace octoon::graphics;

namespace octoon
{
	namespace video
	{
		const char* vert =
			"#version 330\n"
			"uniform mat4 proj;\n"
			"uniform mat4 model;\n"
			"layout(location  = 0) in vec4 POSITION0;\n"
			"void main()\n"
			"{\n"
			"gl_Position = proj * model * POSITION0;\n"
			"}\n";

		const char* frag =
			"#version 330\n"
			"layout(location  = 0) out vec4 oColor;\n"
			"void main()\n"
			"{\n"
			"	oColor = vec4(0.0,1.0,0.0,0.0);\n"
			"}";

		OctoonImplementSingleton(RenderSystem)

		RenderSystem::RenderSystem() noexcept
			: width_(0)
			, height_(0)
			, fbo_(0)
			, fboMSAA_(0)
			, colorTexture_(0)
			, colorTextureMSAA_(0)
			, depthTexture_(0)
			, depthTextureMSAA_(0)
		{
		}

		RenderSystem::~RenderSystem() noexcept
		{
			this->close();
		}

		void
		RenderSystem::setup(WindHandle hwnd, std::uint32_t w, std::uint32_t h) noexcept
		{
			TextSystem::instance()->setup();

			width_ = w;
			height_ = h;
			winhandle_ = hwnd;

			graphics::GraphicsDeviceDesc deviceDesc;
			deviceDesc.setDeviceType(graphics::GraphicsDeviceType::OpenGL);
			device_ = graphics::GraphicsSystem::instance()->createDevice(deviceDesc);
			if (!device_)
				return;

			graphics::GraphicsSwapchainDesc swapchainDesc;
			swapchainDesc.setWindHandle(winhandle_);
			swapchainDesc.setWidth(1);
			swapchainDesc.setHeight(1);
			swapchainDesc.setSwapInterval(graphics::GraphicsSwapInterval::Vsync);
			swapchainDesc.setImageNums(2);
			swapchainDesc.setColorFormat(graphics::GraphicsFormat::B8G8R8A8UNorm);
			swapchainDesc.setDepthStencilFormat(graphics::GraphicsFormat::D24UNorm_S8UInt);
			swapchain_ = device_->createSwapchain(swapchainDesc);
			if (!swapchain_)
				return;

			graphics::GraphicsContextDesc contextDesc;
			contextDesc.setSwapchain(swapchain_);
			context_ = device_->createDeviceContext(contextDesc);
			if (!context_)
				return;

			graphics::GraphicsDataDesc dataDesc;
			dataDesc.setType(graphics::GraphicsDataType::StorageVertexBuffer);
			dataDesc.setStreamSize(0xFFF);
			dataDesc.setUsage(graphics::GraphicsUsageFlagBits::WriteBit);

			vbo_ = device_->createGraphicsData(dataDesc);
			if (!vbo_)
				return;

			GraphicsProgramDesc programDesc;
			programDesc.addShader(device_->createShader(GraphicsShaderDesc(GraphicsShaderStageFlagBits::VertexBit, vert, "main", GraphicsShaderLang::GLSL)));
			programDesc.addShader(device_->createShader(GraphicsShaderDesc(GraphicsShaderStageFlagBits::FragmentBit, frag, "main", GraphicsShaderLang::GLSL)));
			auto program = device_->createProgram(programDesc);

			GraphicsInputLayoutDesc layoutDesc;
			layoutDesc.addVertexLayout(GraphicsVertexLayout(0, "POSITION", 0, GraphicsFormat::R32G32B32SFloat));
			layoutDesc.addVertexBinding(GraphicsVertexBinding(0, layoutDesc.getVertexSize()));

			GraphicsDescriptorSetLayoutDesc descriptor_set_layout;
			descriptor_set_layout.setUniformComponents(program->getActiveParams());

			GraphicsStateDesc stateDesc;
			stateDesc.setPrimitiveType(GraphicsVertexType::TriangleList);
			stateDesc.setCullMode(GraphicsCullMode::None);
			stateDesc.setDepthEnable(true);

			GraphicsPipelineDesc pipeline;
			pipeline.setGraphicsInputLayout(device_->createInputLayout(layoutDesc));
			pipeline.setGraphicsState(device_->createRenderState(stateDesc));
			pipeline.setGraphicsProgram(std::move(program));
			pipeline.setGraphicsDescriptorSetLayout(device_->createDescriptorSetLayout(descriptor_set_layout));

			pipeline_ = device_->createRenderPipeline(pipeline);
			if (!pipeline_)
				return ;

			GraphicsDescriptorSetDesc descriptorSet;
			descriptorSet.setGraphicsDescriptorSetLayout(pipeline.getGraphicsDescriptorSetLayout());

			descriptorSet_ = device_->createDescriptorSet(descriptorSet);
			if (!descriptorSet_)
				return;

			auto begin = descriptorSet_->getGraphicsUniformSets().begin();
			auto end = descriptorSet_->getGraphicsUniformSets().end();

			proj_ = *std::find_if(begin, end, [](const GraphicsUniformSetPtr& set) { return set->get_name() == "proj"; });
			model_ = *std::find_if(begin, end, [](const GraphicsUniformSetPtr& set) { return set->get_name() == "model"; });

			::glewInit();

			glGenTextures(1, &colorTexture_);
			glBindTexture(GL_TEXTURE_2D, colorTexture_);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			glBindTexture(GL_TEXTURE_2D, 0);

			glGenTextures(1, &depthTexture_);
			glBindTexture(GL_TEXTURE_2D, depthTexture_);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
			glBindTexture(GL_TEXTURE_2D, 0);

			glGenFramebuffers(1, &fbo_);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture_, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture_, 0);

			assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

#if !defined(__linux)
			glGenTextures(1, &colorTextureMSAA_);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorTextureMSAA_);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, w, h, GL_TRUE);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

			glGenTextures(1, &depthTextureMSAA_);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depthTextureMSAA_);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT24, w, h, GL_TRUE);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

			glGenFramebuffers(1, &fboMSAA_);
			glBindFramebuffer(GL_FRAMEBUFFER, fboMSAA_);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorTextureMSAA_, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depthTextureMSAA_, 0);

			assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
		}

		void
		RenderSystem::close() noexcept
		{
			TextSystem::instance()->close();
		}

		void
		RenderSystem::setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept
		{
			width_ = w;
			height_ = h;
		}

		void
		RenderSystem::getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept
		{
			w = width_;
			h = height_;
		}

		void
		RenderSystem::render() noexcept
		{
			context_->renderBegin();

			for (auto& camera : video::RenderScene::instance()->getCameraList())
			{
#if defined(__linux)
				glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
#else

				glBindFramebuffer(GL_FRAMEBUFFER, fboMSAA_);
#endif
				context_->setViewport(0, camera->getPixelViewport());
				context_->clearFramebuffer(0, octoon::graphics::GraphicsClearFlagBits::AllBit, camera->getClearColor(), 1.0f, 0);
				context_->setRenderPipeline(pipeline_);
				context_->setDescriptorSet(descriptorSet_);

				proj_->uniform4fmat(camera->getViewProjection());

				for (auto& object : video::RenderScene::instance()->getRenderObjects())
				{
					auto geometry = object->downcast<video::Geometry>();
					auto mesh = geometry->getMesh();
					if (!mesh)
						continue;

					auto material = geometry->getMaterial();
					if (!material)
						continue;

					auto textMaterial = std::dynamic_pointer_cast<TextMaterial>(material);
					if (!textMaterial)
						continue;

					vertices_ = mesh->getVertexArray();
					if (vbo_->getGraphicsDataDesc().getStreamSize() < vertices_.size() * sizeof(math::float3))
					{
						GraphicsDataDesc dataDesc;
						dataDesc.setType(GraphicsDataType::StorageVertexBuffer);
						dataDesc.setStream((std::uint8_t*)vertices_.data());
						dataDesc.setStreamSize(vertices_.size() * sizeof(math::float3));
						dataDesc.setUsage(vbo_->getGraphicsDataDesc().getUsage());

						vbo_ = device_->createGraphicsData(dataDesc);
						if (!vbo_)
						{
							context_->renderEnd();
							return;
						}
					}

					model_->uniform4fmat(geometry->getTransform());

					context_->setVertexBufferData(0, vbo_, 0);
					context_->draw(vertices_.size(), 1, 0, 0);
				}

				if (camera->getCameraOrder() == CameraOrder::Main)
				{
					if (winhandle_)
					{
#if defined(__linux)
						glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
#else
						glBindFramebuffer(GL_READ_FRAMEBUFFER, fboMSAA_);
#endif
						glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
						glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, GL_COLOR_BUFFER_BIT, GL_NEAREST);
					}
				}
			}

			context_->renderEnd();
			context_->present();
		}
	}
}