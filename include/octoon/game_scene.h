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
		GameScene(io::archivebuf& reader) noexcept;
		~GameScene() noexcept;

		void setActive(bool active) except;
		bool getActive() const noexcept;

		void setName(std::string&& name) noexcept;
		void setName(const std::string& name) noexcept;
		const std::string& getName() const noexcept;

		void setGameListener(const GameListenerPtr& listener) noexcept;
		const GameListenerPtr& getGameListener() const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		T* getFeature() const noexcept { return dynamic_cast<T*>(this->getFeature(T::RTTI)); }
		GameFeature* getFeature(const runtime::Rtti* rtti) const noexcept;
		GameFeature* getFeature(const runtime::Rtti& rtti) const noexcept;

		void sendMessage(const std::string& event, const runtime::any& data = nullptr) noexcept;

		void load(const io::archivebuf& reader) except;
		void save(io::archivebuf& write) except;

		std::size_t id() const noexcept;

		const GameObjectPtr& root() const noexcept;

		GameScenePtr clone() const noexcept;

	public:
		static GameScenePtr find(const char* name) noexcept;
		static GameScenePtr find(const std::string& name) noexcept;

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
		std::size_t instance_id_;

		GameObjectPtr  root_;
		GameListenerPtr listener_;
		GameServer* gameServer_;
	};
}

#endif