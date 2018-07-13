#ifndef OCTOON_GAME_SCENE_H_
#define OCTOON_GAME_SCENE_H_

#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT GameScene final : public runtime::RttiInterface
	{
		OctoonDeclareSubClass(GameScene, runtime::RttiInterface)
	public:
		GameScene() noexcept;
		GameScene(std::string&& name) noexcept;
		GameScene(const std::string& name) noexcept;
		GameScene(io::iarchive& reader) noexcept;
		~GameScene() noexcept;

		void setActive(bool active) except;
		bool getActive() const noexcept;

		void setGameListener(const GameListenerPtr& listener) noexcept;
		GameListenerPtr getGameListener() const noexcept;

		void setName(std::string&& name) noexcept;
		void setName(const std::string& name) noexcept;
		const std::string& getName() const noexcept;

		void load(io::iarchive& reader) except;
		void save(io::oarchive& write) except;

		std::size_t id() const noexcept;

		const GameObjectPtr& root() const noexcept;

		GameScenePtr clone() const noexcept;

	private:
		class RootObject final : public GameObject
		{
		public:
			RootObject(GameScene* scene) noexcept;
			virtual ~RootObject() noexcept;

			virtual GameScene* getGameScene() noexcept override;
			virtual const GameScene* getGameScene() const noexcept override;

		private:
			GameScene* scene_;
		};

		std::string name_;
		std::size_t instance_id_;

		GameObjectPtr  root_;
		GameListenerPtr listener_;
	};
}

#endif