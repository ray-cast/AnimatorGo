#ifndef OCTOON_GAME_FEATURE_H_
#define OCTOON_GAME_FEATURE_H_

#include <octoon/game_types.h>

namespace octoon
{
	class OCTOON_EXPORT GameFeature : public runtime::RttiInterface
	{
		OctoonDeclareSubInterface(GameFeature, MessageListener)
	public:
		GameFeature() noexcept;
		virtual ~GameFeature() noexcept;

		void set_active(bool active)  except;
		bool get_active() noexcept;

		void set_game_listener(const GameListenerPtr& listener) noexcept;
		const GameListenerPtr& get_game_listener() const noexcept;

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
		bool is_active_;

		GameServer* game_server_;
		GameListenerPtr game_listener_;
	};
}

#endif