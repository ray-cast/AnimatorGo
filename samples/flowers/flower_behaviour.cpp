#include "flower_behaviour.h"

namespace flower
{
	OctoonImplementSubClass(FlowerBehaviour, octoon::GameComponent, "FlowerBehaviour")

	FlowerBehaviour::FlowerBehaviour() noexcept
	{
	}

	FlowerBehaviour::FlowerBehaviour(const std::shared_ptr<FlowerProfile>& profile) noexcept
		: profile_(profile)
	{
	}

	FlowerBehaviour::~FlowerBehaviour() noexcept
	{
	}

	void
	FlowerBehaviour::addComponent(IFlowerComponent* component) noexcept
	{
		components_.push_back(component);
	}

	void
	FlowerBehaviour::removeComponent(const IFlowerComponent* component) noexcept
	{
		auto it = std::find(components_.begin(), components_.end(), component);
		if (it != components_.end())
			components_.erase(it);
	}

	const std::vector<IFlowerComponent*>&
	FlowerBehaviour::getComponents() const noexcept
	{
		return components_;
	}

	IFlowerComponent*
	FlowerBehaviour::getComponent(const std::type_info& type) const noexcept
	{
		for (auto& it : components_)
		{
			if (it->type_info() == type)
				return it;
		}

		return nullptr;
	}

	void
	FlowerBehaviour::enableComponents() noexcept(false)
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
	FlowerBehaviour::disableComponents() noexcept
	{
		for (auto& it : components_)
			it->onDisable();
	}

	void
	FlowerBehaviour::onActivate() noexcept(false)
	{
		if (!profile_)
			profile_ = FlowerProfile::load("sys:config/config.conf");

		context_ = std::make_shared<RabbitContext>();
		context_->behaviour = this;
		context_->profile = profile_.get();

		recordComponent_ = std::make_unique<RecordComponent>();
		entitiesComponent_ = std::make_unique<EntitiesComponent>();
		offlineComponent_ = std::make_unique<OfflineComponent>();
		playerComponent_ = std::make_unique<PlayerComponent>();
		h265Component_ = std::make_unique<H265Component>();
		uiComponent_ = std::make_unique<UIComponent>();
		markComponent_ = std::make_unique<MarkComponent>();
		materialComponent_ = std::make_unique<MaterialComponent>();
		dragComponent_ = std::make_unique<DragComponent>();
		gizmoComponent_ = std::make_unique<GizmoComponent>();
		gridComponent_ = std::make_unique<GridComponent>();
		lightComponent_ = std::make_unique<LightComponent>();

		recordComponent_->init(context_, profile_->recordModule);
		entitiesComponent_->init(context_, profile_->entitiesModule);
		offlineComponent_->init(context_, profile_->offlineModule);
		playerComponent_->init(context_, profile_->playerModule);
		h265Component_->init(context_, profile_->h265Module);
		uiComponent_->init(context_, profile_->recordModule);
		markComponent_->init(context_, profile_->markModule);
		materialComponent_->init(context_, profile_->materialModule);
		gizmoComponent_->init(context_, profile_->dragModule);
		dragComponent_->init(context_, profile_->dragModule);
		gridComponent_->init(context_, profile_->gridModule);
		lightComponent_->init(context_, profile_->entitiesModule);

		this->h265Component_->setActive(true);

		this->addComponent(entitiesComponent_.get());
		this->addComponent(offlineComponent_.get());
		this->addComponent(playerComponent_.get());
		this->addComponent(markComponent_.get());
		this->addComponent(uiComponent_.get());
		this->addComponent(materialComponent_.get());
		this->addComponent(gizmoComponent_.get());
		this->addComponent(dragComponent_.get());
		this->addComponent(gridComponent_.get());
		this->addComponent(lightComponent_.get());
		this->addComponent(recordComponent_.get());
		this->addComponent(h265Component_.get());

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
				gameObjectManager->addMessageListener("feature:input:mousemove", std::bind(&FlowerBehaviour::onMouseMotion, this, std::placeholders::_1));
				gameObjectManager->addMessageListener("feature:input:mousedown", std::bind(&FlowerBehaviour::onMouseDown, this, std::placeholders::_1));
				gameObjectManager->addMessageListener("feature:input:mouseup", std::bind(&FlowerBehaviour::onMouseUp, this, std::placeholders::_1));
				gameObjectManager->addMessageListener("feature:input:drop", std::bind(&FlowerBehaviour::onDrop, this, std::placeholders::_1));
				gameObjectManager->addMessageListener("feature:input:resize", std::bind(&FlowerBehaviour::onResize, this, std::placeholders::_1));
			}
		}
	}

	void
	FlowerBehaviour::onDeactivate() noexcept
	{
		this->disableComponents();

		recordComponent_.reset();
		entitiesComponent_.reset();
		offlineComponent_.reset();
		playerComponent_.reset();
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
			{
				gameObjectManager->removeMessageListener("feature:input:mousemove", std::bind(&FlowerBehaviour::onMouseMotion, this, std::placeholders::_1));
				gameObjectManager->removeMessageListener("feature:input:mousedown", std::bind(&FlowerBehaviour::onMouseDown, this, std::placeholders::_1));
				gameObjectManager->removeMessageListener("feature:input:mouseup", std::bind(&FlowerBehaviour::onMouseUp, this, std::placeholders::_1));
				gameObjectManager->removeMessageListener("feature:input:drop", std::bind(&FlowerBehaviour::onDrop, this, std::placeholders::_1));
			}
		}
	}

	void
	FlowerBehaviour::onFixedUpdate() noexcept
	{
		for (auto& it : components_)
		{
			if (it->getActive())
				it->onFixedUpdate();
		}
	}

	void
	FlowerBehaviour::onUpdate() noexcept
	{
		for (auto& it : components_)
		{
			if (it->getActive())
				it->onUpdate();
		}
	}

	void
	FlowerBehaviour::onLateUpdate() noexcept
	{
		for (auto& it : components_)
		{
			if (it->getActive())
				it->onLateUpdate();
		}
	}

	void
	FlowerBehaviour::onDrop(const std::any& data) noexcept
	{
		if (data.type() == typeid(std::vector<const char*>))
		{
			auto files = std::any_cast<std::vector<const char*>>(data);
			for (auto& path : files)
				this->open(path);
		}
	}

	void
	FlowerBehaviour::onMouseMotion(const std::any& data) noexcept
	{
		auto event = std::any_cast<octoon::input::InputEvent>(data);
		for (auto& it : components_)
		{
			if (it->getActive())
			{
				if (it->isCapture())
				{
					it->onMouseMotion(event);
					return;
				}
			}
		}

		for (auto& it : components_)
		{
			if (it->getActive())
			{
				it->onMouseMotion(event);
				if (it->isCapture())
					break;
			}
		}
	}

	void
	FlowerBehaviour::onMouseDown(const std::any& data) noexcept
	{
		auto event = std::any_cast<octoon::input::InputEvent>(data);
		for (auto& it : components_)
		{
			if (it->getActive())
			{
				if (it->isCapture())
				{
					it->onMouseDown(event);
					return;
				}
			}
		}

		for (auto& it : components_)
		{
			if (it->getActive())
			{
				it->onMouseDown(event);
				if (it->isCapture())
					break;
			}
		}
	}
	
	void
	FlowerBehaviour::onMouseUp(const std::any& data) noexcept
	{
		auto event = std::any_cast<octoon::input::InputEvent>(data);
		for (auto& it : components_)
		{
			if (it->getActive())
			{
				if (it->isCapture())
				{
					it->onMouseUp(event);
					return;
				}
			}
		}

		for (auto& it : components_)
		{
			if (it->getActive())
			{
				it->onMouseUp(event);
				if (it->isCapture())
					break;
			}
		}
	}

	void
	FlowerBehaviour::onResize(const std::any& data) noexcept
	{
		auto event = std::any_cast<octoon::input::InputEvent>(data);
		for (auto& it : components_)
		{
			if (it->getActive())
				it->onResize(event);
		}
	}

	const std::shared_ptr<FlowerProfile>&
	FlowerBehaviour::getProfile() const noexcept
	{
		return profile_;
	}

	void
	FlowerBehaviour::open(std::string_view path) noexcept(false)
	{
		auto ext = path.substr(path.find_last_of("."));
		if (ext == ".pmm")
			entitiesComponent_->importPMM(path);
		else if (ext == ".scene")
			entitiesComponent_->importAss(path);
		else if (ext == ".pmx")
			entitiesComponent_->importModel(path);
		else if (ext == ".hdr")
			entitiesComponent_->importHDRi(path);
		else if (ext == ".abc")
			entitiesComponent_->importAbc(path);
		else if (ext == ".ogg" || ext == ".mp3" || ext == ".wav" || ext == ".flac")
			entitiesComponent_->importAudio(path);
		else if (ext == ".mdl")
			materialComponent_->importMdl(path);
	}

	void
	FlowerBehaviour::close() noexcept
	{
		this->profile_->entitiesModule->objects.clear();
	}

	bool
	FlowerBehaviour::isOpen() const noexcept
	{
		return !profile_->entitiesModule->objects.empty();
	}

	void
	FlowerBehaviour::openModel() noexcept
	{
		auto pathLimits = this->profile_->fileModule->PATHLIMIT;
		std::vector<std::string::value_type> filepath(pathLimits);
		if (entitiesComponent_->showFileOpenBrowse(filepath.data(), pathLimits, this->profile_->fileModule->modelExtensions[0]))
			entitiesComponent_->importModel(filepath.data());
	}

	void
	FlowerBehaviour::saveModel() noexcept
	{
		auto pathLimits = this->profile_->fileModule->PATHLIMIT;
		std::vector<std::string::value_type> filepath(pathLimits);
		if (!entitiesComponent_->showFileSaveBrowse(filepath.data(), pathLimits, this->profile_->fileModule->modelExtensions[0]))
			entitiesComponent_->exportModel(filepath.data());
	}

	void
	FlowerBehaviour::play() noexcept
	{
		playerComponent_->play();
		dragComponent_->setActive(false);
	}

	void
	FlowerBehaviour::pause() noexcept
	{
		playerComponent_->pause();
		dragComponent_->setActive(true);
	}

	bool
	FlowerBehaviour::startRecord(std::string_view filepath) noexcept
	{
		try
		{
			this->dragComponent_->setActive(false);
			this->offlineComponent_->setActive(profile_->h265Module->quality == VideoQuality::High);
			this->playerComponent_->render();

			this->recordComponent_->startRecord(filepath);

			return true;
		}
		catch (...)
		{
			this->stopRecord();
			return false;
		}
	}

	void
	FlowerBehaviour::stopRecord() noexcept
	{
		this->dragComponent_->setActive(true);
		this->playerComponent_->pause();
		this->recordComponent_->stopRecord();
	}

	void
	FlowerBehaviour::loadAudio(std::string_view filepath) noexcept
	{
		entitiesComponent_->importAudio(filepath);
	}

	void
	FlowerBehaviour::clearAudio() noexcept
	{
		entitiesComponent_->clearAudio();
	}

	void
	FlowerBehaviour::renderPicture(std::string_view filepath) noexcept(false)
	{
		recordComponent_->setActive(true);
		recordComponent_->captureImage(filepath);
	}

	std::optional<octoon::RaycastHit>
	FlowerBehaviour::raycastHit(const octoon::math::float2& pos) noexcept
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
	FlowerBehaviour::clone() const noexcept
	{
		return std::make_shared<FlowerBehaviour>();
	}
}