#ifndef OCTOON_GAME_FEATURE_H_
#define OCTOON_GAME_FEATURE_H_

#include <octoon/game_types.h>
#include <octoon/runtime/any.h>
#include <functional>

namespace octoon
{
	class OCTOON_EXPORT GameFeature : public runtime::RttiInterface
	{
		OctoonDeclareSubInterface(GameFeature, runtime::RttiInterface)
	public:
		GameFeature() noexcept;
		virtual ~GameFeature() noexcept;

		void setActive(bool active) except;
		bool getActive() noexcept;

		const GameListenerPtr& getGameListener() const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		T* getFeature() const noexcept { return dynamic_cast<T*>(this->getFeature(T::RTTI)); }
		GameFeature* getFeature(const runtime::Rtti* rtti) const noexcept;
		GameFeature* getFeature(const runtime::Rtti& rtti) const noexcept;
		const GameFeatures& getFeaturs() const noexcept;

		void sendMessage(const std::string& event, const runtime::any& data = nullptr) noexcept;
		void addMessageListener(const std::string& event, std::function<void(const runtime::any&)> listener) noexcept;
		void removeMessageListener(const std::string& event, std::function<void(const runtime::any&)> listener) noexcept;

		GameServer* getGameServer() noexcept;

	protected:
		virtual void onActivate() except;
		virtual void onDeactivate() noexcept;

		virtual void onOpenScene(const GameScenePtr& scene) except;
		virtual void onCloseScene(const GameScenePtr& scene) noexcept;

		virtual void onReset() except;

		virtual void onFrameBegin() except;
		virtual void onFrame() except;
		virtual void onFrameEnd() except;

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