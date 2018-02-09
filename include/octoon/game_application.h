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

		bool open(WindHandle hwnd, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h, float dpi) noexcept;
		void close() noexcept;

		bool start() noexcept;
		void stop() noexcept;

		void setGameListener(const GameListenerPtr& listener) noexcept;
		const GameListenerPtr& getGameListener() const noexcept;

		bool isQuitRequest() const noexcept;

		bool openScene(const GameScenePtr& scene) noexcept;
		bool openScene(const std::string& name) noexcept;
		void closeScene(const GameScenePtr& name) noexcept;
		void closeScene(const std::string& name) noexcept;
		GameScenePtr findScene(const std::string& name) noexcept;

		bool addFeatures(GameFeaturePtr& feature) noexcept;
		void removeFeatures(GameFeaturePtr& feature) noexcept;

		bool sendInputEvent(const input::InputEvent& event) noexcept;
		bool postInputEvent(const input::InputEvent& event) noexcept;

		void update() noexcept;

	private:
		GameApplication(const GameApplication&) noexcept = delete;
		GameApplication& operator=(const GameApplication&) noexcept = delete;

	private:
		bool isInitialize_;

		std::string workDir_;
		std::string engineDir_;
		std::string resourceBaseDir_;
		std::string downloadURL_;

		GameServer* gameServer_;
		GameListenerPtr gameListener_;

		GameFeaturePtr inputFeature_;
	};
}

#endif