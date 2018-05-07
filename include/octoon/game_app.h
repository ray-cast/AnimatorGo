#ifndef OCTOON_GAME_APP_H_
#define OCTOON_GAME_APP_H_

#include <chrono>
#include <octoon/game_types.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	class OCTOON_EXPORT GameApp final
	{
		OctoonDeclareSingleton(GameApp)
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
		bool openScene(const std::string& name) except;
		void closeScene(const GameScenePtr& name) noexcept;
		void closeScene(const std::string& name) noexcept;
		GameScenePtr findScene(const std::string& name) noexcept;

		template<typename T, typename ...Args, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		std::shared_ptr<T> addFeature(Args&&... args) except { auto t = std::make_shared<T>(std::forward<Args>(args)...); this->addFeature(t); return t; }
		void addFeature(const GameFeaturePtr& feature) except;
		void addFeature(GameFeaturePtr&& feature) except;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		std::shared_ptr<T> getFeature() const noexcept { return std::dynamic_pointer_cast<T>(this->getFeature(T::RTTI)); }
		GameFeaturePtr getFeature(const runtime::Rtti* type) const except;
		GameFeaturePtr getFeature(const runtime::Rtti& type) const except;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		void removeFeature() noexcept { this->removeFeature(T::RTTI); }
		void removeFeature(const runtime::Rtti* type) except;
		void removeFeature(const runtime::Rtti& type) except;
		void removeFeature(const GameFeaturePtr& feature) except;

		void sendInputEvent(const input::InputEvent& event) except;

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
		virtual void onMessage(const std::string& message) noexcept;

	private:
		GameApp(const GameApp&) noexcept = delete;
		GameApp& operator=(const GameApp&) noexcept = delete;

	private:
		GameServerPtr game_server_;
		GameListenerPtr game_listener_;

		std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
	};
}

#endif