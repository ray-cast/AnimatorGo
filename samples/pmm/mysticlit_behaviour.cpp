#include "mysticlit_behaviour.h"
#include <octoon/offline_camera_component.h>
#include <octoon/offline_environment_light_component.h>

namespace MysticLit
{
	OctoonImplementSubClass(MysticlitBehaviour, octoon::GameComponent, "MysticlitBehaviour")

	MysticlitBehaviour::MysticlitBehaviour() noexcept
	{
	}

	MysticlitBehaviour::~MysticlitBehaviour() noexcept
	{
	}

	void
	MysticlitBehaviour::addComponent(IMysticLitComponent* component) noexcept
	{
		components_.push_back(component);
	}

	void
	MysticlitBehaviour::removeComponent(const IMysticLitComponent* component) noexcept
	{
		auto it = std::find(components_.begin(), components_.end(), component);
		if (it != components_.end())
			components_.erase(it);
	}

	IMysticLitComponent*
	MysticlitBehaviour::getComponent(const std::type_info& type) const noexcept
	{
		for (auto& it : components_)
		{
			if (it->type_info() == type)
				return it;
		}

		return nullptr;
	}

	void
	MysticlitBehaviour::enableComponents() noexcept
	{
		for (auto& it : components_)
		{
			it->onEnable();
		}
	}

	void
	MysticlitBehaviour::disableComponents() noexcept
	{
		for (auto& it : components_)
		{
			it->onDisable();
		}
	}

	void
	MysticlitBehaviour::onActivate() noexcept
	{
		profile_ = MysticLitProfile::load("sys:config/config.conf");

		context_ = std::make_shared<MysticLitContext>();
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

		fileComponent_->init(context_, profile_->fileModule);
		canvasComponent_->init(context_, profile_->canvasModule);
		entitiesComponent_->init(context_, profile_->entitiesModule);
		offlineComponent_->init(context_, profile_->offlineModule);
		playerComponent_->init(context_, profile_->timeModule);
		denoiseComponent_->init(context_, profile_->denoiseModule);
		h264Component_->init(context_, profile_->h264Module);
		uiComponent_->init(context_, profile_->canvasModule);

		this->addComponent(fileComponent_.get());
		this->addComponent(canvasComponent_.get());
		this->addComponent(entitiesComponent_.get());
		this->addComponent(offlineComponent_.get());
		this->addComponent(playerComponent_.get());
		this->addComponent(denoiseComponent_.get());
		this->addComponent(h264Component_.get());
		this->addComponent(uiComponent_.get());

		this->enableComponents();
	}

	void
	MysticlitBehaviour::onDeactivate() noexcept
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
	}

	void
	MysticlitBehaviour::onFixedUpdate() noexcept
	{
		bool finish = false;
		for (auto& it : this->profile_->entitiesModule->objects)
		{
			auto animator = it->getComponent<octoon::AnimatorComponent>();
			if (animator)
				finish |= animator->getCurrentAnimatorStateInfo().finish;
		}

		if (!finish)
		{
			if (this->profile_->offlineModule->offlineEnable)
			{
				for (auto& it : components_)
				{
					if (it->getActive())
						it->onPostProcess();
				}
			}
		}
		else
		{
			this->stopRecord();
			this->removeComponentDispatch(octoon::GameDispatchType::FixedUpdate);
		}
	}

	void
	MysticlitBehaviour::open() noexcept
	{
		auto pathLimits = fileComponent_->getModel()->PATHLIMIT;
		std::vector<std::string::value_type> filepath(pathLimits);
		if (fileComponent_->showFileOpenBrowse(filepath.data(), pathLimits, fileComponent_->getModel()->projectExtensions[0]))
			fileComponent_->open(filepath.data());
	}

	void
	MysticlitBehaviour::save() noexcept
	{
	}

	void
	MysticlitBehaviour::saveAs() noexcept
	{
		auto pathLimits = fileComponent_->getModel()->PATHLIMIT;
		std::vector<std::string::value_type> filepath(pathLimits);
		if (!fileComponent_->showFileSaveBrowse(filepath.data(), pathLimits, fileComponent_->getModel()->projectExtensions[0]))
			return;
	}

	void
	MysticlitBehaviour::openModel() noexcept
	{
		auto pathLimits = fileComponent_->getModel()->PATHLIMIT;
		std::vector<std::string::value_type> filepath(pathLimits);
		if (fileComponent_->showFileOpenBrowse(filepath.data(), pathLimits, fileComponent_->getModel()->modelExtensions[0]))
			fileComponent_->importModel(filepath.data());
	}

	void
	MysticlitBehaviour::saveModel() noexcept
	{
		auto pathLimits = fileComponent_->getModel()->PATHLIMIT;
		std::vector<std::string::value_type> filepath(pathLimits);
		if (!fileComponent_->showFileSaveBrowse(filepath.data(), pathLimits, fileComponent_->getModel()->modelExtensions[0]))
			return;
	}

	void
	MysticlitBehaviour::startRecord() noexcept
	{
		auto pathLimits = fileComponent_->getModel()->PATHLIMIT;
		std::vector<std::string::value_type> filepath(pathLimits);
		if (fileComponent_->showFileSaveBrowse(filepath.data(), pathLimits, fileComponent_->getModel()->videoExtensions[0]))
		{
			canvasComponent_->setActive(true);
			denoiseComponent_->setActive(true);
			offlineComponent_->setActive(true);
			h264Component_->setActive(true);

			playerComponent_->render();
			entitiesComponent_->play();
			h264Component_->record(filepath.data());

			this->addComponentDispatch(octoon::GameDispatchType::FixedUpdate);
		}
	}

	void
	MysticlitBehaviour::stopRecord() noexcept
	{
		canvasComponent_->setActive(false);
		denoiseComponent_->setActive(false);
		offlineComponent_->setActive(false);
		h264Component_->setActive(false);

		playerComponent_->stop();
	}

	void
	MysticlitBehaviour::renderPicture() noexcept
	{
		auto pathLimits = fileComponent_->getModel()->PATHLIMIT;
		std::string filepath(pathLimits, 0);
		if (fileComponent_->showFileSaveBrowse(filepath.data(), pathLimits, fileComponent_->getModel()->imageExtensions[0]))
		{
			canvasComponent_->setActive(true);
			denoiseComponent_->setActive(true);

			if (this->profile_->offlineModule->offlineEnable)
			{
				for (auto& it : components_)
				{
					if (it->getActive())
						it->onPostProcess();
				}
			}

			canvasComponent_->save(filepath);
		}
	}

	void
	MysticlitBehaviour::play() noexcept
	{
		playerComponent_->play();
		entitiesComponent_->play();
	}

	void
	MysticlitBehaviour::stop() noexcept
	{
		playerComponent_->stop();
		entitiesComponent_->stop();
	}

	void
	MysticlitBehaviour::startPathTracing() noexcept
	{
		offlineComponent_->setActive(true);
	}

	void
	MysticlitBehaviour::stopPathTracing() noexcept
	{
		offlineComponent_->setActive(false);
	}

	void
	MysticlitBehaviour::loadHDRi(const std::string& path) noexcept
	{
		fileComponent_->importHDRi(path);
	}

	octoon::GameComponentPtr
	MysticlitBehaviour::clone() const noexcept
	{
		return std::make_shared<MysticlitBehaviour>();
	}
}