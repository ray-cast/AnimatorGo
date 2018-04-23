#include <octoon/video/render_system.h>
#include <octoon/video/camera.h>
#include <octoon/video/geometry.h>
#include <octoon/video/render_scene.h>
#include <octoon/video/text_system.h>
#include <octoon/video/text_material.h>
#include <octoon/runtime/except.h>

using namespace octoon::graphics;

namespace octoon
{
	namespace video
	{
		const char* vert =
			"#version 330\n"
			"uniform mat4 proj;\n"
			"uniform mat4 model;\n"
			"uniform float extrude;\n"
			"uniform float lean;\n"
			"uniform vec3 frontColor;\n"
			"uniform vec3 sideColor;\n"
			"layout(location  = 0) in vec4 POSITION0;\n"
			"layout(location  = 1) in vec4 NORMAL0;\n"
			"out vec3 oTexcoord0;\n"
			"void main()\n"
			"{\n"
			"vec4 P = POSITION0;\n"
			"P.x -= P.y * lean;\n"
			"P.z *= extrude;\n"
			"if (abs(NORMAL0.z) > 0.5)\n"
				"oTexcoord0 = frontColor;\n"
			"else\n"
				"oTexcoord0 = sideColor;\n"
			"gl_Position = proj * model * P;\n"
			"}\n";

		const char* frag =
			"#version 330\n"
			"uniform sampler2D decal;\n"
			"layout(location  = 0) out vec4 oColor;\n"
			"in vec3 oTexcoord0;\n"
			"void main()\n"
			"{\n"
			"	oColor = vec4(oTexcoord0, 1.0f);\n"
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

			graphics::GraphicsDeviceDesc deviceDesc;
			deviceDesc.setDeviceType(graphics::GraphicsDeviceType::OpenGL);
			device_ = graphics::GraphicsSystem::instance()->createDevice(deviceDesc);
			if (!device_)
				return;

			graphics::GraphicsSwapchainDesc swapchainDesc;
			swapchainDesc.setWindHandle(hwnd);
			swapchainDesc.setWidth(w);
			swapchainDesc.setHeight(h);
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

			GraphicsProgramDesc programDesc;
			programDesc.addShader(device_->createShader(GraphicsShaderDesc(GraphicsShaderStageFlagBits::VertexBit, vert, "main", GraphicsShaderLang::GLSL)));
			programDesc.addShader(device_->createShader(GraphicsShaderDesc(GraphicsShaderStageFlagBits::FragmentBit, frag, "main", GraphicsShaderLang::GLSL)));
			auto program = device_->createProgram(programDesc);

			GraphicsInputLayoutDesc layoutDesc;
			layoutDesc.addVertexLayout(GraphicsVertexLayout(0, "POSITION", 0, GraphicsFormat::R32G32B32SFloat));
			layoutDesc.addVertexLayout(GraphicsVertexLayout(0, "NORMAL", 0, GraphicsFormat::R32G32B32SFloat));
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
			lean_ = *std::find_if(begin, end, [](const GraphicsUniformSetPtr& set) { return set->get_name() == "lean"; });
			extrude_ = *std::find_if(begin, end, [](const GraphicsUniformSetPtr& set) { return set->get_name() == "extrude"; });
			frontColor_ = *std::find_if(begin, end, [](const GraphicsUniformSetPtr& set) { return set->get_name() == "frontColor"; });
			sideColor_ = *std::find_if(begin, end, [](const GraphicsUniformSetPtr& set) { return set->get_name() == "sideColor"; });

			graphics::GraphicsTextureDesc colorTextureDesc;
			colorTextureDesc.setWidth(w);
			colorTextureDesc.setHeight(h);
			colorTextureDesc.setTexFormat(graphics::GraphicsFormat::R8G8B8A8UNorm);
			colorTexture_ = device_->createTexture(colorTextureDesc);

			graphics::GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(w);
			depthTextureDesc.setHeight(h);
			depthTextureDesc.setTexFormat(graphics::GraphicsFormat::X8_D24UNormPack32);
			depthTexture_ = device_->createTexture(depthTextureDesc);

			graphics::GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::ColorAttachmentOptimal, graphics::GraphicsFormat::R8G8B8A8UNorm));
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::DepthStencilAttachmentOptimal, graphics::GraphicsFormat::X8_D24UNormPack32));

			graphics::GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(w);
			framebufferDesc.setHeight(h);
			framebufferDesc.setGraphicsFramebufferLayout(device_->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(graphics::GraphicsAttachmentBinding(depthTexture_, 0, 0));
			framebufferDesc.addColorAttachment(graphics::GraphicsAttachmentBinding(colorTexture_, 0, 0));

			fbo_ = device_->createFramebuffer(framebufferDesc);

#if !defined(__linux)
			graphics::GraphicsTextureDesc colorTextureDescMSAA;
			colorTextureDescMSAA.setWidth(w);
			colorTextureDescMSAA.setHeight(h);
			colorTextureDescMSAA.setTexMultisample(4);
			colorTextureDescMSAA.setTexFormat(graphics::GraphicsFormat::R8G8B8A8UNorm);
			colorTextureMSAA_ = device_->createTexture(colorTextureDescMSAA);

			graphics::GraphicsTextureDesc depthTextureDescMSAA;
			depthTextureDescMSAA.setWidth(w);
			depthTextureDescMSAA.setHeight(h);
			depthTextureDescMSAA.setTexMultisample(4);
			depthTextureDescMSAA.setTexFormat(graphics::GraphicsFormat::X8_D24UNormPack32);
			depthTextureMSAA_ = device_->createTexture(depthTextureDescMSAA);

			graphics::GraphicsFramebufferDesc framebufferDescMSAA;
			framebufferDescMSAA.setWidth(w);
			framebufferDescMSAA.setHeight(h);
			framebufferDescMSAA.setGraphicsFramebufferLayout(device_->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDescMSAA.setDepthStencilAttachment(graphics::GraphicsAttachmentBinding(depthTextureMSAA_, 0, 0));
			framebufferDescMSAA.addColorAttachment(graphics::GraphicsAttachmentBinding(colorTextureMSAA_, 0, 0));

			fboMSAA_ = device_->createFramebuffer(framebufferDescMSAA);
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

		GraphicsInputLayoutPtr
		RenderSystem::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
		{
			assert(device_);
			return device_->createInputLayout(desc);
		}

		GraphicsDataPtr
		RenderSystem::createGraphicsData(const GraphicsDataDesc& desc) noexcept
		{
			assert(device_);
			return device_->createGraphicsData(desc);
		}

		GraphicsTexturePtr
		RenderSystem::createTexture(const GraphicsTextureDesc& desc) noexcept
		{
			assert(device_);
			return device_->createTexture(desc);
		}

		GraphicsSamplerPtr
		RenderSystem::createSampler(const GraphicsSamplerDesc& desc) noexcept
		{
			assert(device_);
			return device_->createSampler(desc);
		}

		GraphicsFramebufferPtr
		RenderSystem::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
		{
			assert(device_);
			return device_->createFramebuffer(desc);
		}

		GraphicsFramebufferLayoutPtr
		RenderSystem::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
		{
			assert(device_);
			return device_->createFramebufferLayout(desc);
		}

		GraphicsShaderPtr
		RenderSystem::createShader(const GraphicsShaderDesc& desc) noexcept
		{
			assert(device_);
			return device_->createShader(desc);
		}

		GraphicsProgramPtr
		RenderSystem::createProgram(const GraphicsProgramDesc& desc) noexcept
		{
			assert(device_);
			return device_->createProgram(desc);
		}

		GraphicsStatePtr
		RenderSystem::createRenderState(const GraphicsStateDesc& desc) noexcept
		{
			assert(device_);
			return device_->createRenderState(desc);
		}

		GraphicsPipelinePtr
		RenderSystem::createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept
		{
			assert(device_);
			return device_->createRenderPipeline(desc);
		}

		GraphicsDescriptorSetPtr
		RenderSystem::createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept
		{
			assert(device_);
			return device_->createDescriptorSet(desc);
		}

		GraphicsDescriptorSetLayoutPtr
		RenderSystem::createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept
		{
			assert(device_);
			return device_->createDescriptorSetLayout(desc);
		}

		GraphicsDescriptorPoolPtr
		RenderSystem::createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept
		{
			assert(device_);
			return device_->createDescriptorPool(desc);
		}

		void
		RenderSystem::render() noexcept
		{
			context_->renderBegin();

			for (auto& camera : video::RenderScene::instance()->getCameraList())
			{
#if (__linux)
				context_->setFramebuffer(fbo_);
#else
				context_->setFramebuffer(fboMSAA_);
#endif
				context_->setViewport(0, camera->getPixelViewport());
				context_->clearFramebuffer(0, octoon::graphics::GraphicsClearFlagBits::ColorDepthBit, camera->getClearColor(), 1.0f, 0);
				context_->setRenderPipeline(pipeline_);
				context_->setDescriptorSet(descriptorSet_);

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

					proj_->uniform4fmat(camera->getViewProjection());
					frontColor_->uniform3f(textMaterial->getTextColor(TextColor::FrontColor));
					sideColor_->uniform3f(textMaterial->getTextColor(TextColor::SideColor));
					lean_->uniform1f(textMaterial->getLean());
					extrude_->uniform1f(textMaterial->getExtrude());
					model_->uniform4fmat(geometry->getTransform());

					context_->setVertexBufferData(0, geometry->getVertexBuffer(), 0);
					context_->draw(mesh->getVertexArray().size(), 1, 0, 0);
				}

				if (camera->getCameraOrder() == CameraOrder::Main)
				{
						auto& v = camera->getPixelViewport();
#if (__linux)
						context_->blitFramebuffer(fbo_, v, nullptr, v);
#else
						context_->blitFramebuffer(fboMSAA_, v, nullptr, v);
#endif
				}
			}

			context_->renderEnd();
			context_->present();
		}
	}
}