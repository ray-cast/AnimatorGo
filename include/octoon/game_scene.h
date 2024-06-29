#ifndef OCTOON_GAME_SCENE_H_
#define OCTOON_GAME_SCENE_H_

#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT GameScene final : public Object
	{
		OctoonDeclareSubClass(GameScene, Object)
	public:
		GameScene() noexcept;
		GameScene(std::string&& name) noexcept;
		GameScene(std::string_view name) noexcept;
		GameScene(const nlohmann::json& json) noexcept;
		~GameScene() noexcept;

		std::size_t id() const noexcept;

		void setActive(bool active) noexcept(false);
		bool getActive() const noexcept;

		void setName(std::string_view name) noexcept;
		const std::string& getName() const noexcept;

		void setGameListener(const GameListenerPtr& listener) noexcept;
		const GameListenerPtr& getGameListener() const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		T* getFeature() const noexcept { return dynamic_cast<T*>(this->getFeature(T::RTTI)); }
		GameFeature* getFeature(const Rtti* rtti) const noexcept;
		GameFeature* getFeature(const Rtti& rtti) const noexcept;

		void sendMessage(std::string_view event, const std::any& data = std::any()) noexcept;

		void load(const nlohmann::json& reader) noexcept(false);
		void save(nlohmann::json& json) noexcept(false);

		const GameObjectPtr& root() const noexcept;

		GameScenePtr clone() const noexcept;

	public:
		static GameScenePtr find(std::string_view name) noexcept;

	private:
		friend GameServer;
		void _setGameServer(GameServer* gameServ) noexcept;

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
		std::size_t instanceID_;

		GameObjectPtr  root_;
		GameListenerPtr listener_;
		GameServer* gameServer_;
	};
}

#endif