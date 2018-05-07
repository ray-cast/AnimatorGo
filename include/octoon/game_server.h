#ifndef OCTOON_GAME_SERVER_H_
#define OCTOON_GAME_SERVER_H_

#include <octoon/game_types.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	class OCTOON_EXPORT GameServer final : public runtime::RttiInterface
	{
		OctoonDeclareSubClass(GameServer, runtime::RttiInterface)
	public:
		GameServer() noexcept;
		~GameServer() noexcept;

		void setActive(bool active) except;
		bool getActive() const noexcept;

		void setGameListener(GameListenerPtr&& listener) noexcept;
		void setGameListener(const GameListenerPtr& listener) noexcept;
		const GameListenerPtr& getGameListener() const noexcept;

		bool isQuitRequest() const noexcept;

		bool openScene(const std::string& scene_name) noexcept;
		void closeScene(const std::string& scene_name) noexcept;

		bool addScene(const GameScenePtr& scene) noexcept;
		void closeScene(const GameScenePtr& scene) noexcept;

		GameScenePtr findScene(const std::string& scene_name) noexcept;
		const GameScenes& getScenes() const noexcept;

		template<typename T, typename ...Args, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		std::shared_ptr<T> addFeature(Args&&... args) except { auto t = std::make_shared<T>(std::forward<Args>(args)...); this->addFeature(t); return t; }
		void addFeature(const GameFeaturePtr& component) except;
		void addFeature(GameFeaturePtr&& component) except;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		std::shared_ptr<T> getFeature() const noexcept { return std::dynamic_pointer_cast<T>(this->getFeature(T::RTTI)); }
		GameFeaturePtr getFeature(const runtime::Rtti* rtti) const noexcept;
		GameFeaturePtr getFeature(const runtime::Rtti& rtti) const noexcept;

		const GameFeatures& getFeatures() const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		void removeFeature() noexcept { this->removeFeature(T::RTTI); }
		void removeFeature(const runtime::Rtti* rtti) noexcept;
		void removeFeature(const runtime::Rtti& rtti) noexcept;
		void removeFeature(const GameFeaturePtr& feature) noexcept;

		void cleanupAll() noexcept;
		void cleanupScenes() noexcept;
		void cleanupFeatures() noexcept;

		void sendInputEvent(const input::InputEvent& event) noexcept;

		GameApp* getGameApp() noexcept;

		void update() noexcept;

	private:
		void onActivate() except;
		void onDeactivate() noexcept;

	private:
		friend GameApp;
		void setGameApp(GameApp* app) noexcept;

	private:
		GameServer(const GameServer&) noexcept = delete;
		GameServer& operator=(const GameServer&) noexcept = delete;

	private:
		bool is_actived_;
		bool is_quit_request_;

		GameScenes scenes_;
		GameFeatures features_;

		GameApp* game_app_;
		GameListenerPtr game_listener_;
	};
}

#endif