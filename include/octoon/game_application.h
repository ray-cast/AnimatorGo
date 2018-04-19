#ifndef OCTOON_APPLICATION_H_
#define OCTOON_APPLICATION_H_

#include <chrono>
#include <octoon/game_types.h>

namespace octoon
{
	class OCTOON_EXPORT GameApplication
	{
	public:
		GameApplication() noexcept;
		GameApplication(WindHandle hwnd, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) except;
		virtual ~GameApplication() noexcept;

		void open(WindHandle hwnd, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) except;
		void close() noexcept;

		void setActive(bool active) except;
		bool getActive() const noexcept;

		void setGameListener(GameListenerPtr&& listener) noexcept;
		void setGameListener(const GameListenerPtr& listener) noexcept;
		const GameListenerPtr& getGameListener() const noexcept;

		bool isQuitRequest() const noexcept;

		bool openScene(const GameScenePtr& scene) except;
		bool openScene(const std::string& name) except;
		void closeScene(const GameScenePtr& name) noexcept;
		void closeScene(const std::string& name) noexcept;
		GameScenePtr findScene(const std::string& name) noexcept;

		template<typename T, typename ...Args, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		void addFeature(Args&&... args) except { this->addFeature(std::make_shared<T>(std::forward<Args>(args)...)); }
		void addFeature(const GameFeaturePtr& feature) except;
		void addFeature(GameFeaturePtr&& feature) except;
		void removeFeature(const GameFeaturePtr& feature) except;

		void sendInputEvent(const input::InputEvent& event) except;

		void doWindowResize(WindHandle window, std::uint32_t w, std::uint32_t h) except;
		void doWindowFramebufferResize(WindHandle window, std::uint32_t w, std::uint32_t h) except;
		void doWindowClose(WindHandle window) except;
		void doWindowFocus(WindHandle window, bool focus) except;
		void doWindowKeyDown(WindHandle window, std::uint16_t input_key, std::uint16_t scancode, std::uint16_t mods) except;
		void doWindowKeyUp(WindHandle window, std::uint16_t input_key, std::uint16_t scancode, std::uint16_t mods) except;
		void doWindowKeyPress(WindHandle window, std::uint16_t input_key, std::uint16_t scancode, std::uint16_t mods) except;
		void doWindowKeyChar(WindHandle window, std::uint16_t unicode, std::uint16_t mods) except;
		void doWindowMouseButtonDown(WindHandle window, std::uint8_t input_button, float x, float y) except;
		void doWindowMouseButtonUp(WindHandle window, std::uint8_t input_button, float x, float y) except;
		void doWindowMouseButtonDoubleClick(WindHandle window, std::uint8_t input_button, float x, float y) except;
		void doWindowMouseMotion(WindHandle window, float x, float y) except;
		void doWindowScrool(WindHandle window, float x, float y) except;
		void doWindowDrop(WindHandle window, std::uint32_t count, const char** file_utf8) except;

		void update() except;

	protected:
		virtual void onMessage(const std::string& message) noexcept;

	private:
		GameApplication(const GameApplication&) noexcept = delete;
		GameApplication& operator=(const GameApplication&) noexcept = delete;

	private:
		GameServerPtr game_server_;
		GameListenerPtr game_listener_;

		GameFeaturePtr io_feature_;
		GameFeaturePtr timer_feature_;
		GameFeaturePtr input_feature_;
		GameFeaturePtr base_feature_;
		GameFeaturePtr gui_feature_;

		std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
	};
}

#endif