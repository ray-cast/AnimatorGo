#ifndef OCTOON_GAME_TYPES_H_
#define OCTOON_GAME_TYPES_H_

#include <octoon/math/math.h>
#include <octoon/input/input.h>

namespace octoon
{
	class GameServer;
	class GameFeature;
	class GameApplication;
	class GameScene;
	class GameSceneManager;
	class GameListener;
	class GameObject;
	class GameObjectManager;
	class GameComponent;

	typedef std::shared_ptr<GameScene> GameScenePtr;
	typedef std::shared_ptr<GameListener> GameListenerPtr;
	typedef std::shared_ptr<GameObject> GameObjectPtr;
	typedef std::shared_ptr<GameComponent> GameComponentPtr;
	typedef std::shared_ptr<GameFeature> GameFeaturePtr;
	typedef std::shared_ptr<GameServer> GameServerPtr;
	typedef std::shared_ptr<GameApplication> GameApplicationPtr;

	typedef std::weak_ptr<GameScene> GameSceneWeakPtr;
	typedef std::weak_ptr<GameListener> GameListenerWeakPtr;
	typedef std::weak_ptr<GameObject> GameObjectWeakPtr;
	typedef std::weak_ptr<GameComponent> GameComponentWeakPtr;
	typedef std::weak_ptr<GameFeature> GameFeatureWeakPtr;
	typedef std::weak_ptr<GameServer> GameServerWeakPtr;
	typedef std::weak_ptr<GameApplication> GameApplicationWeakPtr;

	typedef std::vector<GameScenePtr> GameScenes;
	typedef std::vector<GameObjectPtr> GameObjects;
	typedef std::vector<GameComponentPtr> GameComponents;
	typedef std::vector<GameComponent*> GameComponentRaws;
	typedef std::vector<GameFeaturePtr> GameFeatures;

	typedef void* WindHandle;

	struct GameDispatchType
	{
		enum Type
		{
			FrameBegin,
			Frame,
			FrameEnd,
			Gui,
			MoveBefore,
			MoveAfter,
			BeginRange_ = FrameBegin,
			EndRange_ = MoveAfter,
			RangeSize_ = (EndRange_ - BeginRange_ + 1),
		};
	};

	typedef std::uint8_t GameDispatchTypes;
}

#endif