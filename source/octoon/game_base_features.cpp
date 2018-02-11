#include <octoon/game_base_features.h>
#include <octoon/game_object_manager.h>
#include <octoon/game_scene_manager.h>

namespace octoon
{
	OctoonImplementSubClass(GameBaseFeatures, GameFeature, "GameBaseFeatures")

	GameBaseFeatures::GameBaseFeatures() noexcept
	{
	}

	GameBaseFeatures::~GameBaseFeatures() noexcept
	{
	}

	void
	GameBaseFeatures::on_frame_begin() noexcept
	{
		GameSceneManager::instance()->on_frame_begin();
		GameObjectManager::instance()->on_frame_begin();
	}

	void
	GameBaseFeatures::on_frame() noexcept
	{
		GameSceneManager::instance()->on_frame();
		GameObjectManager::instance()->on_frame();
	}

	void
	GameBaseFeatures::on_frame_end() noexcept
	{
		GameSceneManager::instance()->on_frame_end();
		GameObjectManager::instance()->on_frame_end();
	}
}