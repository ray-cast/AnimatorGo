#ifndef OCTOON_GAME_APP_H_
#define OCTOON_GAME_APP_H_

#include <any>
#include <chrono>
#include <functional>

#include <octoon/input/input.h>
#include <octoon/game_types.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	class OCTOON_EXPORT GameApp final
	{
	public:
		GameApp() noexcept;
		GameApp(WindHandle hwnd, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept(false);
		virtual ~GameApp() noexcept;

		void open(WindHandle hwnd, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept(false);
		bool isOpen() const noexcept;
		void close() noexcept;

		void setActive(bool active) noexcept(false);
		bool getActive() const noexcept;

		void setGameListener(GameListenerPtr&& listener) noexcept;
		void setGameListener(const GameListenerPtr& listener) noexcept;
		const GameListenerPtr& getGameListener() const noexcept;

		bool isQuitRequest() const noexcept;

		bool openScene(const GameScenePtr& scene) noexcept(false);
		void closeScene(const GameScenePtr& name) noexcept;

		bool openScene(std::string_view name) noexcept(false);
		void closeScene(std::string_view name) noexcept;

		GameScenePtr findScene(std::string_view name) noexcept;

		template<typename T, typename ...Args, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		void addFeature(Args&&... args) except { this->addFeature(std::make_shared<T>(std::forward<Args>(args)...)); }
		void addFeature(GameFeaturePtr&& feature) noexcept(false);

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		T* getFeature() const noexcept { return dynamic_cast<T*>(this->getFeature(T::RTTI)); }
		GameFeature* getFeature(const Rtti* type) const noexcept(false);
		GameFeature* getFeature(const Rtti& type) const noexcept(false);

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		void removeFeature() noexcept { this->removeFeature(T::RTTI); }
		void removeFeature(const Rtti* type) noexcept(false);
		void removeFeature(const Rtti& type) noexcept(false);
		void removeFeature(const GameFeaturePtr& feature) noexcept(false);

		void sendInputEvent(const input::InputEvent& event) noexcept(false);

		void sendMessage(std::string_view event, const std::any& data = std::any()) noexcept(false);
		void addMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept(false);
		void removeMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept(false);

		void doWindowResize(WindHandle window, std::uint32_t w, std::uint32_t h) noexcept(false);
		void doWindowFramebufferResize(WindHandle window, std::uint32_t w, std::uint32_t h) noexcept(false);
		void doWindowClose(WindHandle window) noexcept(false);
		void doWindowFocus(WindHandle window, bool focus) noexcept(false);
		void doWindowKeyDown(WindHandle window, input::InputKey::Code key, std::uint16_t scancode, std::uint16_t mods) noexcept(false);
		void doWindowKeyUp(WindHandle window, input::InputKey::Code key, std::uint16_t scancode, std::uint16_t mods) noexcept(false);
		void doWindowKeyPress(WindHandle window, input::InputKey::Code key, std::uint16_t scancode, std::uint16_t mods) noexcept(false);
		void doWindowKeyChar(WindHandle window, std::uint16_t unicode, std::uint16_t mods) noexcept(false);
		void doWindowMouseButtonDown(WindHandle window, input::InputButton::Code button, float x, float y) noexcept(false);
		void doWindowMouseButtonUp(WindHandle window, input::InputButton::Code button, float x, float y) noexcept(false);
		void doWindowMouseButtonDoubleClick(WindHandle window, input::InputButton::Code button, float x, float y) noexcept(false);
		void doWindowMouseMotion(WindHandle window, float x, float y) noexcept(false);
		void doWindowScrool(WindHandle window, float x, float y) noexcept(false);
		void doWindowDrop(WindHandle window, std::uint32_t count, const char** file_utf8) noexcept(false);

		void start() noexcept(false);
		void stop() noexcept;

		void update() noexcept(false);

	protected:
		void onMessage(std::string_view message) noexcept;

	private:
		GameApp(const GameApp&) noexcept = delete;
		GameApp& operator=(const GameApp&) noexcept = delete;

	private:
		bool isOpen_;

		GameServerPtr server_;
		GameListenerPtr listener_;

		std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
	};
}

#endif