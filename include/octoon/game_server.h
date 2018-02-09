#ifndef OCTOON_GAME_SERVER_H_
#define OCTOON_GAME_SERVER_H_

#include <octoon/game_types.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	class OCTOON_EXPORT GameServer final : public runtime::RttiInterface, public runtime::Singleton<GameServer>
	{
		OctoonDeclareSubClass(GameServer, runtime::RttiInterface)
	public:
		GameServer() noexcept;
		~GameServer() noexcept;

		bool open() noexcept;
		void close() noexcept;

		void setGameListener(const GameListenerPtr& listener) noexcept;
		const GameListenerPtr& getGameListener() const noexcept;

		bool isActive() const noexcept;
		bool isStopping() const noexcept;
		bool isQuitRequest() const noexcept;

		bool openScene(const std::string& sceneName) noexcept;
		void closeScene(const std::string& sceneName) noexcept;

		bool addScene(const GameScenePtr& scene) noexcept;
		void closeScene(const GameScenePtr& scene) noexcept;

		GameScenePtr findScene(const std::string& sceneName) noexcept;
		const GameScenes& getScenes() const noexcept;

		bool addFeature(GameFeaturePtr& features) noexcept;
		void removeFeature(GameFeaturePtr& features) noexcept;
		GameFeaturePtr getFeature(const runtime::Rtti& rtti) const noexcept;
		GameFeaturePtr getFeature(const runtime::Rtti* rtti) const noexcept;

		template<typename T>
		std::shared_ptr<T> getFeature() const noexcept { return std::dynamic_pointer_cast<T>(this->getFeature(T::getRtti())); }

		const GameFeatures& getGameFeatures() const noexcept;

		GameApplication* getGameApp() noexcept;

		bool start() noexcept;
		void stop() noexcept;
		void update() noexcept;

	private:
		friend GameApplication;
		void _setGameApp(GameApplication* app) noexcept;

	private:
		GameServer(const GameServer&) noexcept = delete;
		GameServer& operator=(const GameServer&) noexcept = delete;

	private:

		bool _isActive;
		bool _isStopping;
		bool _isQuitRequest;

		GameScenes _scenes;
		GameFeatures _features;

		GameApplication* _gameApp;
		GameListenerPtr _gameListener;
	};
}

#endif