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
		GameApp(WindHandle hwnd, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) except;
		virtual ~GameApp() noexcept;

		void open(WindHandle hwnd, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) except;
		void close() noexcept;

		void setActive(bool active) except;
		bool getActive() const noexcept;

		void setGameListener(GameListenerPtr&& listener) noexcept;
		void setGameListener(const GameListenerPtr& listener) noexcept;
		const GameListenerPtr& getGameListener() const noexcept;

		bool isQuitRequest() const noexcept;

		bool openScene(const GameScenePtr& scene) except;
		void closeScene(const GameScenePtr& name) noexcept;

		bool openScene(std::string_view name) except;
		void closeScene(std::string_view name) noexcept;

		GameScenePtr findScene(std::string_view name) noexcept;

		template<typename T, typename ...Args, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		void addFeature(Args&&... args) except { this->addFeature(std::make_shared<T>(std::forward<Args>(args)...)); }
		void addFeature(GameFeaturePtr&& feature) except;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		T* getFeature() const noexcept { return dynamic_cast<T*>(this->getFeature(T::RTTI)); }
		GameFeature* getFeature(const runtime::Rtti* type) const except;
		GameFeature* getFeature(const runtime::Rtti& type) const except;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		void removeFeature() noexcept { this->removeFeature(T::RTTI); }
		void removeFeature(const runtime::Rtti* type) except;
		void removeFeature(const runtime::Rtti& type) except;
		void removeFeature(const GameFeaturePtr& feature) except;

		void sendInputEvent(const input::InputEvent& event) except;

		void sendMessage(std::string_view event, const std::any& data = std::any()) except;
		void addMessageListener(std::string_view event, std::function<void(const std::any&)> listener) except;
		void removeMessageListener(std::string_view event, std::function<void(const std::any&)> listener) except;

		void doWindowResize(WindHandle window, std::uint32_t w, std::uint32_t h) except;
		void doWindowFramebufferResize(WindHandle window, std::uint32_t w, std::uint32_t h) except;
		void doWindowClose(WindHandle window) except;
		void doWindowFocus(WindHandle window, bool focus) except;
		void doWindowKeyDown(WindHandle window, input::InputKey::Code key, std::uint16_t scancode, std::uint16_t mods) except;
		void doWindowKeyUp(WindHandle window, input::InputKey::Code key, std::uint16_t scancode, std::uint16_t mods) except;
		void doWindowKeyPress(WindHandle window, input::InputKey::Code key, std::uint16_t scancode, std::uint16_t mods) except;
		void doWindowKeyChar(WindHandle window, std::uint16_t unicode, std::uint16_t mods) except;
		void doWindowMouseButtonDown(WindHandle window, input::InputButton::Code button, float x, float y) except;
		void doWindowMouseButtonUp(WindHandle window, input::InputButton::Code button, float x, float y) except;
		void doWindowMouseButtonDoubleClick(WindHandle window, input::InputButton::Code button, float x, float y) except;
		void doWindowMouseMotion(WindHandle window, float x, float y) except;
		void doWindowScrool(WindHandle window, float x, float y) except;
		void doWindowDrop(WindHandle window, std::uint32_t count, const char** file_utf8) except;

		void start() except;
		void stop() noexcept;

		void update() except;

	protected:
		void onMessage(std::string_view message) noexcept;

	private:
		GameApp(const GameApp&) noexcept = delete;
		GameApp& operator=(const GameApp&) noexcept = delete;

	private:
		GameServerPtr server_;
		GameListenerPtr listener_;

		std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
	};
}

#endif