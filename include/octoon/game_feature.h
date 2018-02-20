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

		void set_active(bool active)  except;
		bool get_active() noexcept;

		const GameListenerPtr& get_game_listener() const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of_v<GameFeature, T>>>
		std::shared_ptr<T> get_feature() const noexcept { return std::dynamic_pointer_cast<T>(this->get_feature(T::RTTI)); }
		GameFeaturePtr get_feature(const runtime::Rtti* rtti) const noexcept;
		GameFeaturePtr get_feature(const runtime::Rtti& rtti) const noexcept;
		const GameFeatures& get_featurs() const noexcept;

		GameServer* get_game_server() noexcept;

	protected:
		virtual void on_activate() except;
		virtual void on_deactivate() noexcept;

		virtual void on_open_scene(const GameScenePtr& scene) except;
		virtual void on_close_scene(const GameScenePtr& scene) noexcept;

		virtual void on_input_event(const input::InputEvent& event) noexcept;

		virtual void on_reset() except;

		virtual void on_frame_begin() except;
		virtual void on_frame() except;
		virtual void on_frame_end() except;

	private:
		friend GameServer;
		void _set_game_server(GameServer* server) noexcept;

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