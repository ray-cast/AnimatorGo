#ifndef OCTOON_APPLICATION_H_
#define OCTOON_APPLICATION_H_

#include <octoon/game_types.h>

namespace octoon
{
	class OCTOON_EXPORT GameApplication
	{
	public:
		GameApplication() noexcept;
		virtual ~GameApplication() noexcept;

		bool open(WindHandle hwnd, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept;
		void close() noexcept;

		bool start() noexcept;
		void stop() noexcept;

		void set_game_listener(const GameListenerPtr& listener) noexcept;
		const GameListenerPtr& get_game_listener() const noexcept;

		bool is_quit_request() const noexcept;

		bool open_scene(const GameScenePtr& scene) noexcept;
		bool open_scene(const std::string& name) noexcept;
		void close_scene(const GameScenePtr& name) noexcept;
		void close_scene(const std::string& name) noexcept;
		GameScenePtr find_scene(const std::string& name) noexcept;

		bool add_feature(GameFeaturePtr& feature) noexcept;
		void remove_feature(GameFeaturePtr& feature) noexcept;

		void send_input_event(const input::InputEvent& event) noexcept;

		void update() noexcept;

	private:
		GameApplication(const GameApplication&) noexcept = delete;
		GameApplication& operator=(const GameApplication&) noexcept = delete;

	private:
		bool is_initialize_;

		GameServer* game_server_;
		GameListenerPtr game_listener_;

		GameFeaturePtr input_feature_;
		GameFeaturePtr base_feature_;
		GameFeaturePtr gui_feature_;
	};
}

#endif