#include <octoon/game_base_features.h>
#include <octoon/game_object_manager.h>
#include <octoon/model/text_system.h>

namespace octoon
{
	OctoonImplementSubClass(GameBaseFeatures, GameFeature, "GameBaseFeatures")

	GameBaseFeatures::GameBaseFeatures() noexcept
	{
		model::TextSystem::instance()->setup();
	}

	GameBaseFeatures::~GameBaseFeatures() noexcept
	{
		model::TextSystem::instance()->close();
	}

	void
	GameBaseFeatures::onFrameBegin() noexcept
	{		
	}

	void
	GameBaseFeatures::onFrame() noexcept
	{
		GameObjectManager::instance()->onFrameBegin();
		GameObjectManager::instance()->onFrame();
		GameObjectManager::instance()->onFrameEnd();
	}

	void
	GameBaseFeatures::onFrameEnd() noexcept
	{	
	}
}