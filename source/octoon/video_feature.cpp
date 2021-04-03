#if defined(OCTOON_FEATURE_VIDEO_ENABLE)
#include <octoon/video_feature.h>
#include <octoon/video/renderer.h>

#include <octoon/input_feature.h>
#include <octoon/input/input_event.h>

#include <octoon/game_app.h>
#include <octoon/game_server.h>
#include <octoon/game_listener.h>

#include <octoon/hal_feature.h>
#include <octoon/runtime/except.h>

namespace octoon
{
	OctoonImplementSubClass(VideoFeature, GameFeature, "VideoFeature")

	VideoFeature::VideoFeature() noexcept
		: framebuffer_w_(0)
		, framebuffer_h_(0)
		, camera_(nullptr)
		, enableGlobalIllumination_(false)
	{
	}

	VideoFeature::VideoFeature(std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept
		: framebuffer_w_(framebuffer_w)
		, framebuffer_h_(framebuffer_h)
		, camera_(nullptr)
		, enableGlobalIllumination_(false)
	{
	}

	VideoFeature::~VideoFeature() noexcept
	{
	}

	void
	VideoFeature::setMainCamera(CameraComponent* camera) noexcept
	{
		if (camera_ != camera)
		{
			if (camera_ && camera)
				camera_->setCameraType(CameraType::Custom);
			camera_ = camera;
		}
	}
	
	CameraComponent*
	VideoFeature::getMainCamera() const noexcept
	{
		return camera_;
	}

	void
	VideoFeature::setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (framebuffer_w_ != w || framebuffer_h_ != h)
		{
			framebuffer_w_ = w;
			framebuffer_h_ = h;

			Renderer::instance()->setFramebufferSize(w, h);
		}
	}

	void
	VideoFeature::getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept
	{
		Renderer::instance()->getFramebufferSize(w, h);
	}

	const hal::GraphicsFramebufferPtr&
	VideoFeature::getFramebuffer() const noexcept
	{
		return Renderer::instance()->getFramebuffer();
	}

	void
	VideoFeature::setGlobalIllumination(bool enable) noexcept
	{
		this->enableGlobalIllumination_ = enable;

		if (this->renderScene_)
			this->renderScene_->setGlobalIllumination(enable);
	}

	bool
	VideoFeature::getGlobalIllumination() const noexcept
	{
		return enableGlobalIllumination_;
	}

	void
	VideoFeature::setMaxBounces(std::uint32_t num_bounces)
	{
		Renderer::instance()->setMaxBounces(num_bounces);
	}

	std::uint32_t
	VideoFeature::getMaxBounces() const
	{
		return Renderer::instance()->getMaxBounces();
	}

	void
	VideoFeature::setOverrideMaterial(const std::shared_ptr<Material>& material) noexcept
	{
		Renderer::instance()->setOverrideMaterial(material);
	}

	std::shared_ptr<Material>
	VideoFeature::getOverrideMaterial() const noexcept
	{
		return Renderer::instance()->getOverrideMaterial();
	}

	void
	VideoFeature::setMainScene(const std::shared_ptr<RenderScene>& scene) noexcept
	{
		this->mainScene_ = scene ? scene : this->mainSceneDefault_;
		this->mainScene_->setGlobalIllumination(this->enableGlobalIllumination_);
	}
	
	const std::shared_ptr<RenderScene>&
	VideoFeature::getMainScene() const noexcept
	{
		return this->mainScene_;
	}

	void
	VideoFeature::setRenderScene(const std::shared_ptr<RenderScene>& scene) noexcept
	{
		this->renderScene_ = scene;
	}

	const std::shared_ptr<RenderScene>&
	VideoFeature::getRenderScene() const noexcept
	{
		return this->renderScene_;
	}

	Renderer*
	VideoFeature::getRenderer() noexcept
	{
		return Renderer::instance();
	}

	const Renderer*
	VideoFeature::getRenderer() const noexcept
	{
		return Renderer::instance();
	}

	void
	VideoFeature::readColorBuffer(math::float3 data[])
	{
		return Renderer::instance()->readColorBuffer(data);
	}

	void
	VideoFeature::readAlbedoBuffer(math::float3 data[])
	{
		return Renderer::instance()->readAlbedoBuffer(data);
	}

	void
	VideoFeature::readNormalBuffer(math::float3 data[])
	{
		return Renderer::instance()->readNormalBuffer(data);
	}

	void
	VideoFeature::onActivate() except
	{
		auto graphics = this->getFeature<GraphicsFeature>();
		if (graphics)
		{
			Renderer::instance()->setup(graphics->getContext(), framebuffer_w_, framebuffer_h_);
			
			this->mainSceneDefault_ = std::make_shared<RenderScene>();
			this->mainSceneDefault_->setGlobalIllumination(this->enableGlobalIllumination_);

			this->setMainScene(this->mainSceneDefault_);
			this->setRenderScene(this->mainSceneDefault_);
			this->addMessageListener("feature:input:event", std::bind(&VideoFeature::onInputEvent, this, std::placeholders::_1));
		}
		else
		{
			throw runtime::runtime_error::create("failure to get the HAL from features");
		}
	}

	void
	VideoFeature::onDeactivate() noexcept
	{
		this->setMainScene(nullptr);
		this->removeMessageListener("feature:input:event", std::bind(&VideoFeature::onInputEvent, this, std::placeholders::_1));
		Renderer::instance()->close();
	}

	void
	VideoFeature::onInputEvent(const std::any& data) noexcept
	{
		auto event = std::any_cast<input::InputEvent>(data);
		switch (event.event)
		{
		case input::InputEvent::SizeChange:
		case input::InputEvent::SizeChangeDPI:
			if (event.change.w > 0 && event.change.h > 0)
				Renderer::instance()->setFramebufferSize(event.change.w, event.change.h);
			break;
		default:
			return;
		}
	}

	void
	VideoFeature::onFrameBegin() noexcept
	{
	}

	void
	VideoFeature::onFrame() noexcept(false)
	{
		try
		{
			auto scene = this->getRenderScene();
			if (scene)
				Renderer::instance()->render(scene);
		}
		catch (const std::exception& e)
		{
			this->getGameListener()->onMessage(e.what());
		}
	}

	void
	VideoFeature::onFrameEnd() noexcept
	{
	}
}
#endif