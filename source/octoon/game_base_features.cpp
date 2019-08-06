#include <octoon/game_base_features.h>
#include <octoon/game_object_manager.h>
#include <octoon/model/text_system.h>

namespace octoon
{
	OctoonImplementSubClass(GameBaseFeature, GameFeature, "GameBaseFeatures")

	GameBaseFeature::GameBaseFeature() noexcept
	{
		model::TextSystem::instance()->setup();
	}

	GameBaseFeature::~GameBaseFeature() noexcept
	{
		model::TextSystem::instance()->close();
	}

    void
	GameBaseFeature::onActivate() noexcept
    {
		this->addMessageListener("feature:timer:fixed", std::bind(&GameBaseFeature::onFixedUpdate, this, std::placeholders::_1));
    }

	void
	GameBaseFeature::onFrameBegin() noexcept
	{
	}

	void
	GameBaseFeature::onFrame() noexcept
	{
		GameObjectManager::instance()->onUpdate();
		GameObjectManager::instance()->onLateUpdate();
	}

	void
	GameBaseFeature::onFrameEnd() noexcept
	{		
	}

	void
	GameBaseFeature::onFixedUpdate(const runtime::any& data) noexcept
	{
		GameObjectManager::instance()->onFixedUpdate();
	}
}