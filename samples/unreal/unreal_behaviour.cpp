#include "unreal_behaviour.h"
#include "../utils/pmm_loader.h"
#include "../utils/ass_loader.h"
#include "../utils/asset_library.h"
#include <octoon/asset_database.h>
#include <filesystem>

namespace unreal
{
	OctoonImplementSubClass(UnrealBehaviour, octoon::GameComponent, "UnrealBehaviour")

	UnrealBehaviour::UnrealBehaviour() noexcept
	{
	}

	UnrealBehaviour::UnrealBehaviour(const std::shared_ptr<UnrealProfile>& profile) noexcept
		: profile_(profile)
	{
	}

	UnrealBehaviour::~UnrealBehaviour() noexcept
	{
		AssetLibrary::instance()->close();
	}

	bool
	UnrealBehaviour::open(const std::filesystem::path& path) noexcept(false)
	{
		if (std::filesystem::is_directory(path))
		{
			this->reset();
			this->profile_->load(path);
			this->playerComponent_->updateTimeLength();
			this->playerComponent_->reset();
			this->sendMessage("editor:project:open");

			return true;
		}

		return false;
	}

	void
	UnrealBehaviour::save(const std::filesystem::path& path) noexcept(false)
	{
		this->profile_->save(path);
	}

	void
	UnrealBehaviour::reset() noexcept
	{
		this->profile_->reset();

		AssetLibrary::instance()->unload();
	}

	bool
	UnrealBehaviour::isOpen() const noexcept
	{
		if (!profile_->path.empty())
			return true;

		if (profile_->playerModule->timeLength > 0)
			return true;

		return !profile_->entitiesModule->objects.getValue().empty();
	}

	void
	UnrealBehaviour::load(const std::filesystem::path& path) noexcept(false)
	{
		auto ext = path.extension().u8string();
		for (auto& it : ext)
			it = (char)std::tolower(it);

		if (ext == u8".pmm")
		{
			PMMLoader::load(*profile_, path);

			playerComponent_->updateTimeLength();
			playerComponent_->reset();

			this->sendMessage("editor:project:open");
		}
		else if (ext == u8".scene")
		{
			AssLoader::load(*profile_, path);

			playerComponent_->updateTimeLength();
			playerComponent_->reset();

			this->sendMessage("editor:project:open");
		}
		else if (ext == u8".pmx" || ext == u8".abc" || ext == u8".obj" || ext == u8".fbx")
		{
			if (entitiesComponent_->importModel(path))
			{
				playerComponent_->updateTimeLength();
				playerComponent_->reset();
			}
		}
		else if (ext == u8".ogg" || ext == u8".mp3" || ext == u8".wav" || ext == u8".flac")
		{
			profile_->soundModule->filepath = path.string();
		}
	}

	void
	UnrealBehaviour::play() noexcept
	{
		playerComponent_->play();
	}

	void
	UnrealBehaviour::pause() noexcept
	{
		playerComponent_->pause();
	}

	bool
	UnrealBehaviour::startRecord(const std::filesystem::path& path) noexcept
	{
		try
		{
			this->offlineComponent_->setActive(profile_->encodeModule->quality == VideoQuality::High);
			this->playerComponent_->render();

			this->recordComponent_->startRecord(path);

			return true;
		}
		catch (...)
		{
			this->stopRecord();
			return false;
		}
	}

	void
	UnrealBehaviour::stopRecord() noexcept
	{
		this->playerComponent_->pause();
		this->recordComponent_->stopRecord();
	}

	void
	UnrealBehaviour::renderPicture(const std::filesystem::path& path) noexcept(false)
	{
		recordComponent_->setActive(true);
		recordComponent_->captureImage(path);
	}

	std::optional<octoon::RaycastHit>
	UnrealBehaviour::raycastHit(const octoon::math::float2& pos) noexcept
	{
		auto camera = this->profile_->cameraModule->camera.getValue();
		if (camera)
		{
			auto cameraComponent = camera->getComponent<octoon::CameraComponent>();
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
	UnrealBehaviour::addComponent(IUnrealComponent* component) noexcept
	{
		components_.push_back(component);
	}

	void
	UnrealBehaviour::removeComponent(const IUnrealComponent* component) noexcept
	{
		auto it = std::find(components_.begin(), components_.end(), component);
		if (it != components_.end())
			components_.erase(it);
	}

	const std::vector<IUnrealComponent*>&
	UnrealBehaviour::getComponents() const noexcept
	{
		return components_;
	}

	IUnrealComponent*
	UnrealBehaviour::getComponent(const std::type_info& type) const noexcept
	{
		for (auto& it : components_)
		{
			if (it->type_info() == type)
				return it;
		}

		return nullptr;
	}

	void
	UnrealBehaviour::initializeComponents() noexcept(false)
	{
		auto feature = this->tryGetFeature<octoon::GameBaseFeature>();

		for (auto& it : components_)
		{
			if (feature)
				feature->log("Initialize :" + std::string(it->type_info().name()));

			if (it->getActive())
				it->onEnable();
		}

		if (feature)
			feature->log("End of the components initialization process.");
	}

	void
	UnrealBehaviour::disableComponents() noexcept
	{
		for (auto& it : components_)
			it->onDisable();
	}

	const std::shared_ptr<UnrealProfile>&
	UnrealBehaviour::getProfile() const noexcept
	{
		return profile_;
	}

	void
	UnrealBehaviour::onActivate() noexcept(false)
	{
		if (!std::filesystem::exists(profile_->resourceModule->rootPath))
			std::filesystem::create_directory(profile_->resourceModule->rootPath);

		context_ = std::make_shared<UnrealContext>();
		context_->behaviour = this;
		context_->profile = profile_.get();

		AssetLibrary::instance()->open(profile_->resourceModule->rootPath);

		recordComponent_ = std::make_unique<RecordComponent>();
		entitiesComponent_ = std::make_unique<EntitiesComponent>();
		mainLightComponent_ = std::make_unique<MainLightComponent>();
		environmentComponent_ = std::make_unique<EnvironmentComponent>();
		cameraComponent_ = std::make_unique<CameraComponent>();
		offlineComponent_ = std::make_unique<OfflineComponent>();
		playerComponent_ = std::make_unique<PlayerComponent>();
		soundComponent_ = std::make_unique<SoundComponent>();
		h264Component_ = std::make_unique<H264Component>();
		h265Component_ = std::make_unique<H265Component>();
		frameSequenceComponent_ = std::make_unique<FrameSequenceComponent>();
		markComponent_ = std::make_unique<MarkComponent>();
		materialComponent_ = std::make_unique<MaterialComponent>();
		selectorComponent_ = std::make_unique<SelectorComponent>();
		//gizmoComponent_ = std::make_unique<GizmoComponent>();
		gridComponent_ = std::make_unique<GridComponent>();
		lightComponent_ = std::make_unique<LightComponent>();

		recordComponent_->init(context_, profile_->recordModule);
		entitiesComponent_->init(context_, profile_->entitiesModule);
		mainLightComponent_->init(context_, profile_->mainLightModule);
		environmentComponent_->init(context_, profile_->environmentLightModule);
		cameraComponent_->init(context_, profile_->cameraModule);
		soundComponent_->init(context_, profile_->soundModule);
		offlineComponent_->init(context_, profile_->offlineModule);
		playerComponent_->init(context_, profile_->playerModule);
		h264Component_->init(context_, profile_->encodeModule);
		h265Component_->init(context_, profile_->encodeModule);
		frameSequenceComponent_->init(context_, profile_->encodeModule);
		markComponent_->init(context_, profile_->markModule);
		materialComponent_->init(context_, profile_->resourceModule);
		//gizmoComponent_->init(context_, profile_->selectorModule);
		selectorComponent_->init(context_, profile_->selectorModule);
		gridComponent_->init(context_, profile_->gridModule);
		lightComponent_->init(context_, profile_->entitiesModule);

		this->addComponent(entitiesComponent_.get());
		this->addComponent(mainLightComponent_.get());
		this->addComponent(environmentComponent_.get());
		this->addComponent(cameraComponent_.get());
		this->addComponent(soundComponent_.get());
		this->addComponent(offlineComponent_.get());
		this->addComponent(playerComponent_.get());
		this->addComponent(markComponent_.get());
		this->addComponent(materialComponent_.get());
		//this->addComponent(gizmoComponent_.get());
		this->addComponent(selectorComponent_.get());
		this->addComponent(gridComponent_.get());
		this->addComponent(lightComponent_.get());
		this->addComponent(recordComponent_.get());
		this->addComponent(h264Component_.get());
		this->addComponent(h265Component_.get());
		this->addComponent(frameSequenceComponent_.get());

		this->initializeComponents();

		this->addComponentDispatch(octoon::GameDispatchType::FixedUpdate);
		this->addComponentDispatch(octoon::GameDispatchType::Frame);
		this->addComponentDispatch(octoon::GameDispatchType::LateUpdate);

		auto baseFeature = this->getFeature<octoon::GameBaseFeature>();
		if (baseFeature)
		{
			auto gameObjectManager = baseFeature->getGameObjectManager();
			if (gameObjectManager)
			{
				gameObjectManager->addMessageListener("feature:input:mousemove", std::bind(&UnrealBehaviour::onMouseMotion, this, std::placeholders::_1));
				gameObjectManager->addMessageListener("feature:input:mousedown", std::bind(&UnrealBehaviour::onMouseDown, this, std::placeholders::_1));
				gameObjectManager->addMessageListener("feature:input:mouseup", std::bind(&UnrealBehaviour::onMouseUp, this, std::placeholders::_1));
				gameObjectManager->addMessageListener("feature:input:resize", std::bind(&UnrealBehaviour::onResize, this, std::placeholders::_1));
			}
		}
	}

	void
	UnrealBehaviour::onDeactivate() noexcept
	{
		this->disableComponents();

		recordComponent_.reset();
		entitiesComponent_.reset();
		mainLightComponent_.reset();
		environmentComponent_.reset();
		cameraComponent_.reset();
		offlineComponent_.reset();
		playerComponent_.reset();
		h264Component_.reset();
		h265Component_.reset();
		frameSequenceComponent_.reset();
		context_.reset();
		profile_.reset();
		materialComponent_.reset();
		selectorComponent_.reset();
		gridComponent_.reset();

		auto baseFeature = this->getFeature<octoon::GameBaseFeature>();
		if (baseFeature)
		{
			auto gameObjectManager = baseFeature->getGameObjectManager();
			if (gameObjectManager)
			{
				gameObjectManager->removeMessageListener("feature:input:mousemove", std::bind(&UnrealBehaviour::onMouseMotion, this, std::placeholders::_1));
				gameObjectManager->removeMessageListener("feature:input:mousedown", std::bind(&UnrealBehaviour::onMouseDown, this, std::placeholders::_1));
				gameObjectManager->removeMessageListener("feature:input:mouseup", std::bind(&UnrealBehaviour::onMouseUp, this, std::placeholders::_1));
			}
		}

		AssetLibrary::instance()->close();
	}

	void
	UnrealBehaviour::onFixedUpdate() noexcept
	{
		for (auto& it : components_)
		{
			if (it->getActive())
				it->onFixedUpdate();
		}
	}

	void
	UnrealBehaviour::onUpdate() noexcept
	{
		for (auto& it : components_)
		{
			if (it->getActive())
				it->onUpdate();
		}
	}

	void
	UnrealBehaviour::onLateUpdate() noexcept
	{
		for (auto& it : components_)
		{
			if (it->getActive())
				it->onLateUpdate();
		}
	}

	void
	UnrealBehaviour::onMouseMotion(const std::any& data) noexcept
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
	UnrealBehaviour::onMouseDown(const std::any& data) noexcept
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
	UnrealBehaviour::onMouseUp(const std::any& data) noexcept
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
	UnrealBehaviour::onResize(const std::any& data) noexcept
	{
		auto event = std::any_cast<octoon::input::InputEvent>(data);
		for (auto& it : components_)
		{
			if (it->getActive())
				it->onResize(event);
		}
	}

	octoon::GameComponentPtr
	UnrealBehaviour::clone() const noexcept
	{
		return std::make_shared<UnrealBehaviour>();
	}
}