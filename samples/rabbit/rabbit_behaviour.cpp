#include "rabbit_behaviour.h"

namespace rabbit
{
	OctoonImplementSubClass(RabbitBehaviour, octoon::GameComponent, "RabbitBehaviour")

	RabbitBehaviour::RabbitBehaviour() noexcept
	{
#if defined(_WINDOWS) && !defined(_DEBUG)
		if (IsDebuggerPresent())
			std::exit(0);
#endif
	}

	RabbitBehaviour::RabbitBehaviour(const std::shared_ptr<RabbitProfile>& profile) noexcept
		: profile_(profile)
	{
#if defined(_WINDOWS) && !defined(_DEBUG)
		if (IsDebuggerPresent())
			std::exit(0);
#endif
	}

	RabbitBehaviour::~RabbitBehaviour() noexcept
	{
	}

	void
	RabbitBehaviour::addComponent(IRabbitComponent* component) noexcept
	{
		components_.push_back(component);
	}

	void
	RabbitBehaviour::removeComponent(const IRabbitComponent* component) noexcept
	{
		auto it = std::find(components_.begin(), components_.end(), component);
		if (it != components_.end())
			components_.erase(it);
	}

	const std::vector<IRabbitComponent*>&
	RabbitBehaviour::getComponents() const noexcept
	{
		return components_;
	}

	IRabbitComponent*
	RabbitBehaviour::getComponent(const std::type_info& type) const noexcept
	{
		for (auto& it : components_)
		{
			if (it->type_info() == type)
				return it;
		}

		return nullptr;
	}

	void
	RabbitBehaviour::enableComponents() noexcept
	{
		for (auto& it : components_)
			it->onEnable();
	}

	void
	RabbitBehaviour::disableComponents() noexcept
	{
		for (auto& it : components_)
			it->onDisable();
	}

	void
	RabbitBehaviour::sendMessage(const std::string_view& event, const std::any& data) noexcept
	{
		auto it = dispatchEvents_.find(event);
		if (it != dispatchEvents_.end())
			(*it).second.call_all_slots(data);
	}

	void
	RabbitBehaviour::addMessageListener(const std::string_view& event, std::function<void(const std::any&)> listener) noexcept
	{
		auto it = dispatchEvents_.find(event);
		if (it != dispatchEvents_.end())
			(*it).second.connect(listener);
		else
			dispatchEvents_[std::string(event)].connect(listener);
	}

	void
	RabbitBehaviour::removeMessageListener(const std::string_view& event, std::function<void(const std::any&)> listener) noexcept
	{
		auto it = dispatchEvents_.find(event);
		if (it != dispatchEvents_.end())
			(*it).second.disconnect(listener);
		else
			dispatchEvents_[std::string(event)].disconnect(listener);
	}

	void
	RabbitBehaviour::onActivate() noexcept
	{
		if (!profile_)
			profile_ = RabbitProfile::load("sys:config/config.conf");

		context_ = std::make_shared<RabbitContext>();
		context_->behaviour = this;
		context_->profile = profile_.get();

		fileComponent_ = std::make_unique<FileComponent>();
		canvasComponent_ = std::make_unique<CanvasComponent>();
		entitiesComponent_ = std::make_unique<EntitiesComponent>();
		offlineComponent_ = std::make_unique<OfflineComponent>();
		playerComponent_ = std::make_unique<PlayerComponent>();
		denoiseComponent_ = std::make_unique<DenoiseComponent>();
		h264Component_ = std::make_unique<H264Component>();
		uiComponent_ = std::make_unique<UIComponent>();
		markComponent_ = std::make_unique<MarkComponent>();
		clientComponent_ = std::make_unique<ClientComponent>();
		materialComponent_ = std::make_unique<MaterialComponent>();

		fileComponent_->init(context_, profile_->fileModule);
		canvasComponent_->init(context_, profile_->canvasModule);
		entitiesComponent_->init(context_, profile_->entitiesModule);
		offlineComponent_->init(context_, profile_->offlineModule);
		playerComponent_->init(context_, profile_->timeModule);
		denoiseComponent_->init(context_, profile_->denoiseModule);
		h264Component_->init(context_, profile_->h264Module);
		uiComponent_->init(context_, profile_->canvasModule);
		markComponent_->init(context_, profile_->markModule);
		clientComponent_->init(context_, profile_->clientModule);
		materialComponent_->init(context_, profile_->materialModule);

		this->addComponent(fileComponent_.get());
		this->addComponent(canvasComponent_.get());
		this->addComponent(entitiesComponent_.get());
		this->addComponent(offlineComponent_.get());
		this->addComponent(playerComponent_.get());
		this->addComponent(denoiseComponent_.get());
		this->addComponent(markComponent_.get());
		this->addComponent(h264Component_.get());
		this->addComponent(uiComponent_.get());
		this->addComponent(clientComponent_.get());
		this->addComponent(materialComponent_.get());

		this->enableComponents();

		this->addComponentDispatch(octoon::GameDispatchType::FixedUpdate);
		this->addComponentDispatch(octoon::GameDispatchType::Frame);
		this->addComponentDispatch(octoon::GameDispatchType::LateUpdate);

		auto baseFeature = this->getFeature<octoon::GameBaseFeature>();
		if (baseFeature)
		{
			auto gameObjectManager = baseFeature->getGameObjectManager();
			if (gameObjectManager)
				gameObjectManager->addMessageListener("feature:input:drop", std::bind(&RabbitBehaviour::onDrop, this, std::placeholders::_1));
		}
	}

	void
	RabbitBehaviour::onDeactivate() noexcept
	{
		fileComponent_.reset();
		canvasComponent_.reset();
		entitiesComponent_.reset();
		offlineComponent_.reset();
		playerComponent_.reset();
		denoiseComponent_.reset();
		h264Component_.reset();
		context_.reset();
		profile_.reset();
		uiComponent_.reset();

		auto baseFeature = this->getFeature<octoon::GameBaseFeature>();
		if (baseFeature)
		{
			auto gameObjectManager = baseFeature->getGameObjectManager();
			if (gameObjectManager)
				gameObjectManager->removeMessageListener("feature:input:drop", std::bind(&RabbitBehaviour::onDrop, this, std::placeholders::_1));
		}
	}

	void
	RabbitBehaviour::onFixedUpdate() noexcept
	{
		for (auto& it : components_)
			it->onFixedUpdate();
	}

	void
	RabbitBehaviour::onUpdate() noexcept
	{
		for (auto& it : components_)
			it->onUpdate();
	}

	void
	RabbitBehaviour::onLateUpdate() noexcept
	{
		for (auto& it : components_)
			it->onLateUpdate();
	}

	void
	RabbitBehaviour::onDrop(const octoon::runtime::any& data) noexcept
	{
		if (data.type() == typeid(std::vector<const char*>))
		{
			auto files = octoon::runtime::any_cast<std::vector<const char*>>(data);
			for (auto& path : files)
			{
				for (auto& it : components_)
				{
					if (it->getActive())
						it->onDrop(std::string_view(path));
				}
			}
		}
	}

	const std::shared_ptr<RabbitProfile>&
	RabbitBehaviour::getProfile() const noexcept
	{
		return profile_;
	}

	void
	RabbitBehaviour::open(const std::string_view& filepath) noexcept(false)
	{
		fileComponent_->open(filepath.data());
	}

	void
	RabbitBehaviour::close() noexcept
	{
		this->profile_->entitiesModule->objects.clear();
		this->profile_->entitiesModule->objects.push_back(this->profile_->entitiesModule->camera);
		this->profile_->entitiesModule->objects.push_back(this->profile_->entitiesModule->sunLight);
		this->profile_->entitiesModule->objects.push_back(this->profile_->entitiesModule->enviromentLight);
	}

	bool
	RabbitBehaviour::isOpen() const noexcept
	{
		return !profile_->entitiesModule->objects.empty();
	}

	void
	RabbitBehaviour::openModel() noexcept
	{
		auto pathLimits = fileComponent_->getModel()->PATHLIMIT;
		std::vector<std::string::value_type> filepath(pathLimits);
		if (fileComponent_->showFileOpenBrowse(filepath.data(), pathLimits, fileComponent_->getModel()->modelExtensions[0]))
			fileComponent_->importModel(filepath.data());
	}

	void
	RabbitBehaviour::saveModel() noexcept
	{
		auto pathLimits = fileComponent_->getModel()->PATHLIMIT;
		std::vector<std::string::value_type> filepath(pathLimits);
		if (!fileComponent_->showFileSaveBrowse(filepath.data(), pathLimits, fileComponent_->getModel()->modelExtensions[0]))
			return;
	}

	bool
	RabbitBehaviour::startRecord(const std::string_view& filepath) noexcept
	{
		try
		{
			canvasComponent_->setActive(true);
			denoiseComponent_->setActive(true);
			h264Component_->setActive(true);
			playerComponent_->render();

			if (profile_->h264Module->quality == VideoQuality::High)
				offlineComponent_->setActive(true);
			else
				offlineComponent_->setActive(false);

			if (h264Component_->record(filepath.data()))
				return true;

			this->stopRecord();
			return false;
		}
		catch (...)
		{
			this->stopRecord();
			return false;
		}
	}

	void
	RabbitBehaviour::stopRecord() noexcept
	{
		canvasComponent_->setActive(false);
		denoiseComponent_->setActive(false);
		h264Component_->setActive(false);
		playerComponent_->stop();
	}

	void
	RabbitBehaviour::renderPicture(const std::string_view& filepath) noexcept(false)
	{
		canvasComponent_->setActive(true);
		denoiseComponent_->setActive(true);

		for (auto& it : components_)
		{
			if (it->getActive())
				it->onPostProcess();
		}

		canvasComponent_->save(filepath);

		if (profile_->offlineModule->offlineEnable)
			clientComponent_->coin(1);
	}

	void
	RabbitBehaviour::loadHDRi(const std::string_view& path) noexcept
	{
		fileComponent_->importHDRi(path);
	}

	void 
	RabbitBehaviour::clearHDRi() noexcept
	{
		fileComponent_->clearHDRi();
	}

	void
	RabbitBehaviour::loadMaterial(const std::string_view& path) noexcept(false)
	{
		materialComponent_->loadMaterial(path);
	}

	std::optional<octoon::RaycastHit>
	RabbitBehaviour::raycastHit(const octoon::math::float2& pos) noexcept
	{
		if (this->profile_->entitiesModule->camera)
		{
			auto cameraComponent = this->profile_->entitiesModule->camera->getComponent<octoon::CameraComponent>();
			if (cameraComponent)
			{
				octoon::Raycaster raycaster(cameraComponent->screenToRay(pos));
				auto& hits = raycaster.intersectObjects(this->profile_->entitiesModule->objects);
				if (!hits.empty())
					return hits.front();
			}
		}

		return std::nullopt;
	}

	octoon::GameComponentPtr
	RabbitBehaviour::clone() const noexcept
	{
		return std::make_shared<RabbitBehaviour>();
	}
}