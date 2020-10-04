#include "rabbit_behaviour.h"

namespace rabbit
{
	OctoonImplementSubClass(RabbitBehaviour, octoon::GameComponent, "RabbitBehaviour")

	RabbitBehaviour::RabbitBehaviour() noexcept
	{
	}

	RabbitBehaviour::RabbitBehaviour(const std::shared_ptr<RabbitProfile>& profile) noexcept
		: profile_(profile)
	{
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
	RabbitBehaviour::enableComponents() noexcept(false)
	{
		auto feature = this->tryGetFeature<octoon::GameBaseFeature>();

		for (auto& it : components_)
		{
			if (feature)
				feature->log("Initialize :" + std::string(it->type_info().name()));
			
			it->onEnable();
		}

		if (feature)
			feature->log("End of the components initialization process.");
	}

	void
	RabbitBehaviour::disableComponents() noexcept
	{
		for (auto& it : components_)
			it->onDisable();
	}

	void
	RabbitBehaviour::onActivate() noexcept(false)
	{
		if (!profile_)
			profile_ = RabbitProfile::load("sys:config/config.conf");

		context_ = std::make_shared<RabbitContext>();
		context_->behaviour = this;
		context_->profile = profile_.get();

		canvasComponent_ = std::make_unique<CanvasComponent>();
		entitiesComponent_ = std::make_unique<EntitiesComponent>();
		offlineComponent_ = std::make_unique<OfflineComponent>();
		playerComponent_ = std::make_unique<PlayerComponent>();
		denoiseComponent_ = std::make_unique<DenoiseComponent>();
		h265Component_ = std::make_unique<H265Component>();
		uiComponent_ = std::make_unique<UIComponent>();
		markComponent_ = std::make_unique<MarkComponent>();
		materialComponent_ = std::make_unique<MaterialComponent>();
		dragComponent_ = std::make_unique<DragComponent>();
		gridComponent_ = std::make_unique<GridComponent>();

		canvasComponent_->init(context_, profile_->canvasModule);
		entitiesComponent_->init(context_, profile_->entitiesModule);
		offlineComponent_->init(context_, profile_->offlineModule);
		playerComponent_->init(context_, profile_->timeModule);
		denoiseComponent_->init(context_, profile_->denoiseModule);
		h265Component_->init(context_, profile_->h265Module);
		uiComponent_->init(context_, profile_->canvasModule);
		markComponent_->init(context_, profile_->markModule);
		materialComponent_->init(context_, profile_->materialModule);
		dragComponent_->init(context_, profile_->dragModule);
		gridComponent_->init(context_, profile_->gridModule);

		this->addComponent(canvasComponent_.get());
		this->addComponent(entitiesComponent_.get());
		this->addComponent(offlineComponent_.get());
		this->addComponent(playerComponent_.get());
		this->addComponent(denoiseComponent_.get());
		this->addComponent(markComponent_.get());
		this->addComponent(h265Component_.get());
		this->addComponent(uiComponent_.get());
		this->addComponent(materialComponent_.get());
		this->addComponent(dragComponent_.get());
		this->addComponent(gridComponent_.get());

		this->enableComponents();

		this->addComponentDispatch(octoon::GameDispatchType::FixedUpdate);
		this->addComponentDispatch(octoon::GameDispatchType::Frame);
		this->addComponentDispatch(octoon::GameDispatchType::LateUpdate);

		auto baseFeature = this->getFeature<octoon::GameBaseFeature>();
		if (baseFeature)
		{
			auto gameObjectManager = baseFeature->getGameObjectManager();
			if (gameObjectManager)
			{
				gameObjectManager->addMessageListener("feature:input:drop", std::bind(&RabbitBehaviour::onDrop, this, std::placeholders::_1));
				gameObjectManager->addMessageListener("feature:input:event", std::bind(&RabbitBehaviour::onInputEvent, this, std::placeholders::_1));
			}
		}
	}

	void
	RabbitBehaviour::onDeactivate() noexcept
	{
		this->disableComponents();

		canvasComponent_.reset();
		entitiesComponent_.reset();
		offlineComponent_.reset();
		playerComponent_.reset();
		denoiseComponent_.reset();
		h265Component_.reset();
		context_.reset();
		profile_.reset();
		uiComponent_.reset();
		materialComponent_.reset();
		dragComponent_.reset();
		gridComponent_.reset();

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
		{
			if (it->getActive())
				it->onFixedUpdate();
		}
	}

	void
	RabbitBehaviour::onUpdate() noexcept
	{
		for (auto& it : components_)
		{
			if (it->getActive())
				it->onUpdate();
		}
	}

	void
	RabbitBehaviour::onLateUpdate() noexcept
	{
		for (auto& it : components_)
		{
			if (it->getActive())
				it->onLateUpdate();
		}
	}

	void
	RabbitBehaviour::onDrop(const std::any& data) noexcept
	{
		if (data.type() == typeid(std::vector<const char*>))
		{
			auto files = std::any_cast<std::vector<const char*>>(data);
			for (auto& path : files)
				this->open(path);
		}
	}

	const std::shared_ptr<RabbitProfile>&
	RabbitBehaviour::getProfile() const noexcept
	{
		return profile_;
	}

	void
	RabbitBehaviour::open(std::string_view path) noexcept(false)
	{
		auto ext = path.substr(path.find_last_of("."));
		if (ext == ".pmm")
			entitiesComponent_->importPMM(path);
		else if (ext == ".pmx")
			entitiesComponent_->importModel(path);
		else if (ext == ".hdr")
			entitiesComponent_->importHDRi(path);
		else if (ext == ".abc")
			entitiesComponent_->importAbc(path);
		else if (ext == ".ogg" || ext == ".mp3" || ext == ".wav" || ext == ".flac")
			entitiesComponent_->importAudio(path);
		else if (ext == ".mtl")
			materialComponent_->importMtl(path);
		else if (ext == ".mdl")
			materialComponent_->importMdl(path);
	}

	void
	RabbitBehaviour::close() noexcept
	{
		this->profile_->entitiesModule->objects.clear();
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
		auto pathLimits = this->profile_->fileModule->PATHLIMIT;
		std::vector<std::string::value_type> filepath(pathLimits);
		if (entitiesComponent_->showFileOpenBrowse(filepath.data(), pathLimits, this->profile_->fileModule->modelExtensions[0]))
			entitiesComponent_->importModel(filepath.data());
	}

	void
	RabbitBehaviour::saveModel() noexcept
	{
		auto pathLimits = this->profile_->fileModule->PATHLIMIT;
		std::vector<std::string::value_type> filepath(pathLimits);
		if (!entitiesComponent_->showFileSaveBrowse(filepath.data(), pathLimits, this->profile_->fileModule->modelExtensions[0]))
			entitiesComponent_->exportModel(filepath.data());
	}

	void
	RabbitBehaviour::play() noexcept
	{
		playerComponent_->play();
		dragComponent_->setActive(false);
	}

	void
	RabbitBehaviour::pause() noexcept
	{
		playerComponent_->pause();
		dragComponent_->setActive(true);
	}

	bool
	RabbitBehaviour::startRecord(std::string_view filepath) noexcept
	{
		try
		{
			canvasComponent_->setActive(true);
			denoiseComponent_->setActive(true);
			h265Component_->setActive(true);
			dragComponent_->setActive(false);
			playerComponent_->render();

			if (profile_->h265Module->quality == VideoQuality::High)
				offlineComponent_->setActive(true);
			else
				offlineComponent_->setActive(false);

			if (h265Component_->record(filepath))
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
		h265Component_->setActive(false);
		dragComponent_->setActive(true);
		playerComponent_->pause();
	}

	void
	RabbitBehaviour::loadAudio(std::string_view filepath) noexcept
	{
		entitiesComponent_->importAudio(filepath);
	}

	void
	RabbitBehaviour::clearAudio() noexcept
	{
		entitiesComponent_->clearAudio();
	}

	void
	RabbitBehaviour::renderPicture(std::string_view filepath) noexcept(false)
	{
		canvasComponent_->setActive(true);
		denoiseComponent_->setActive(true);

		for (auto& it : components_)
		{
			if (it->getActive())
				it->onPostProcess();
		}

		canvasComponent_->save(filepath);
	}

	void
	RabbitBehaviour::loadHDRi(std::string_view path) noexcept
	{
		entitiesComponent_->importHDRi(path);
	}

	void
	RabbitBehaviour::loadHDRi(const octoon::hal::GraphicsTexturePtr& texture) noexcept
	{
		entitiesComponent_->importHDRi(texture);
	}

	void 
	RabbitBehaviour::clearHDRi() noexcept
	{
		entitiesComponent_->clearHDRi();
	}

	void
	RabbitBehaviour::loadMaterial(std::string_view path) noexcept(false)
	{
		materialComponent_->importMtl(path);
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

	void
	RabbitBehaviour::onInputEvent(const std::any& data) noexcept
	{
		auto event = std::any_cast<octoon::input::InputEvent>(data);
		switch (event.event)
		{
		case octoon::input::InputEvent::SizeChange:
		case octoon::input::InputEvent::SizeChangeDPI:
			if (event.change.w > 0 && event.change.h > 0)
				this->profile_->canvasModule->resize(event.change.w, event.change.h);
			break;
		default:
			return;
		}
	}

	octoon::GameComponentPtr
	RabbitBehaviour::clone() const noexcept
	{
		return std::make_shared<RabbitBehaviour>();
	}
}