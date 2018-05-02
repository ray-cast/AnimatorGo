#ifndef OCTOON_GAME_FEATURE_H_
#define OCTOON_GAME_FEATURE_H_

#include <octoon/game_types.h>

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
		std::shared_ptr<T> getFeature() const noexcept { return std::dynamic_pointer_cast<T>(this->getFeature(T::RTTI)); }
		GameFeaturePtr getFeature(const runtime::Rtti* rtti) const noexcept;
		GameFeaturePtr getFeature(const runtime::Rtti& rtti) const noexcept;
		const GameFeatures& getFeatures() const noexcept;

		GameServer* getGameServer() noexcept;

	protected:
		virtual void onActivate() except;
		virtual void onDeactivate() noexcept;

		virtual void onOpenScene(const GameScenePtr& scene) except;
		virtual void onCloseScene(const GameScenePtr& scene) noexcept;

		virtual void onInputEvent(const input::InputEvent& event) noexcept;

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
		bool is_actived_;

		GameServer* game_server_;
		GameListenerPtr game_listener_;
	};
}

#endif