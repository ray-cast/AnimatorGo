#ifndef OCTOON_GAME_FEATURE_H_
#define OCTOON_GAME_FEATURE_H_

#include <octoon/game_types.h>

#include <any>
#include <functional>

namespace octoon
{
	class OCTOON_EXPORT GameFeature : public Object
	{
		OctoonDeclareSubInterface(GameFeature, Object)
	public:
		GameFeature() noexcept;
		virtual ~GameFeature() noexcept;

		void setActive(bool active) noexcept(false);
		bool getActive() noexcept;

		const GameListenerPtr& getGameListener() const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		T* getFeature() const noexcept { return dynamic_cast<T*>(this->getFeature(T::RTTI)); }
		GameFeature* getFeature(const Rtti* rtti) const noexcept;
		GameFeature* getFeature(const Rtti& rtti) const noexcept;
		const GameFeatures& getFeaturs() const noexcept;

		void sendMessage(std::string_view event, const std::any& data = std::any()) noexcept;
		void addMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept;
		void removeMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept;

		GameServer* getGameServer() noexcept;

	protected:
		virtual void onActivate() noexcept(false);
		virtual void onDeactivate() noexcept;

		virtual void onOpenScene(const GameScenePtr& scene) noexcept(false);
		virtual void onCloseScene(const GameScenePtr& scene) noexcept;

		virtual void onReset() noexcept(false);

		virtual void onFrameBegin() noexcept(false);
		virtual void onFrame() noexcept(false);
		virtual void onFrameEnd() noexcept(false);

	private:
		friend GameServer;
		void _setGameServer(GameServer* server) noexcept;

	private:
		GameFeature(const GameFeature&) noexcept = delete;
		GameFeature& operator=(const GameFeature&) noexcept = delete;

	private:
		bool isActived_;

		GameServer* server_;
		GameListenerPtr listener_;
	};
}

#endif