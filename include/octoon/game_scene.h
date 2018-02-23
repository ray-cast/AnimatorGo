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
		GameScene(const std::string& name) noexcept;
		~GameScene() noexcept;

		void set_active(bool active) except;
		bool get_active() const noexcept;

		void set_game_listener(const GameListenerPtr& listener) noexcept;
		GameListenerPtr get_game_listener() const noexcept;

		void set_name(std::string&& name) noexcept;
		void set_name(const std::string& name) noexcept;
		const std::string& get_name() const noexcept;

		std::size_t id() const noexcept;

		const GameObjectPtr& root() const noexcept;

		GameScenePtr clone() const noexcept;

	private:
		class RootObject : public GameObject
		{
		public:
			RootObject(GameScene* scene) noexcept;
			virtual ~RootObject() noexcept;

			virtual GameScene* get_game_scene() noexcept override;
			virtual const GameScene* get_game_scene() const noexcept override;

		private:
			GameScene* scene_;
		};

		std::string name_;
		std::size_t instance_id_;

		GameObjectPtr  root_;
		GameListenerPtr game_listener_;
	};
}

#endif