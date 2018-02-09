#include <octoon/game_application.h>
#include <octoon/game_server.h>
#include <octoon/game_listener.h>

#include <octoon/runtime/rtti_factory.h>

#include <iostream>

#include "input_feature.h"
#include "game_base_features.h"

namespace octoon
{
	class GameAppListener final : public GameListener
	{
	public:
		virtual void onMessage(const std::string& message) noexcept
		{
			std::cerr << message << std::endl;
		}
	};

	GameApplication::GameApplication() noexcept
		: isInitialize_(false)
		, gameServer_(nullptr)
		, gameListener_(std::make_shared<GameAppListener>())

	{
		std::locale::global(std::locale(""));
	}

	GameApplication::~GameApplication() noexcept
	{
		this->close();
	}

	bool
	GameApplication::open(WindHandle hwnd, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h, float dpi) noexcept
	{
		if (isInitialize_)
		{
			if (gameListener_)
				gameListener_->onMessage("Game Application has been opened.");

			return false;
		}

		auto local = setlocale(LC_ALL, "");
		if (local)
		{
			if (gameListener_)
				gameListener_->onMessage(std::string("Initializing : Local : ") + local);
		}

		if (gameListener_)
			gameListener_->onMessage("Initializing : Game Application.");

		if (gameListener_)
			gameListener_->onMessage("Initializing : RTTI.");

		if (!runtime::RttiFactory::instance()->open())
		{
			if (gameListener_)
				gameListener_->onMessage("Could not initialize with RTTI.");

			return false;
		}

		if (gameListener_)
			gameListener_->onMessage("Initializing : Game Server.");

		gameServer_ = GameServer::instance();
		gameServer_->_setGameApp(this);
		gameServer_->setGameListener(gameListener_);

		if (!gameServer_->open())
			return false;

		inputFeature_ = std::make_shared<InputFeature>(hwnd);
		baseFeature_ = std::make_shared<GameBaseFeatures>();

		this->addFeatures(inputFeature_);
		this->addFeatures(baseFeature_);

		isInitialize_ = true;
		return isInitialize_;
	}

	void
	GameApplication::close() noexcept
	{
		if (gameListener_)
			gameListener_->onMessage("Shutdown : Game Server.");

		if (gameServer_)
		{
			gameServer_->close();
			gameServer_ = nullptr;
		}

		if (gameListener_)
			gameListener_->onMessage("Shutdown : IO Server.");
	}

	void
	GameApplication::setGameListener(const GameListenerPtr& listener) noexcept
	{
		gameServer_->setGameListener(listener);
	}

	const GameListenerPtr&
	GameApplication::getGameListener() const noexcept
	{
		return gameServer_->getGameListener();
	}

	bool
	GameApplication::start() noexcept
	{
		if (gameServer_)
			return gameServer_->start();
		return false;
	}

	void
	GameApplication::stop() noexcept
	{
		if (gameServer_)
			gameServer_->stop();
	}

	bool
	GameApplication::isQuitRequest() const noexcept
	{
		assert(gameServer_);
		return gameServer_->isQuitRequest();
	}

	bool
	GameApplication::openScene(const GameScenePtr& scene) noexcept
	{
		assert(gameServer_);
		return gameServer_->addScene(scene);
	}

	bool
	GameApplication::openScene(const std::string& name) noexcept
	{
		assert(gameServer_);
		return gameServer_->openScene(name);
	}

	void
	GameApplication::closeScene(const GameScenePtr& name) noexcept
	{
		assert(gameServer_);
		return gameServer_->closeScene(name);
	}

	void
	GameApplication::closeScene(const std::string& name) noexcept
	{
		assert(gameServer_);
		gameServer_->closeScene(name);
	}

	GameScenePtr
	GameApplication::findScene(const std::string& name) noexcept
	{
		assert(gameServer_);
		return gameServer_->findScene(name);
	}

	bool
	GameApplication::addFeatures(GameFeaturePtr& feature) noexcept
	{
		assert(gameServer_);
		return gameServer_->addFeature(feature);
	}

	void
	GameApplication::removeFeatures(GameFeaturePtr& feature) noexcept
	{
		assert(gameServer_);
		gameServer_->removeFeature(feature);
	}

	void
	GameApplication::sendInputEvent(const input::InputEvent& event) noexcept
	{
		assert(gameServer_);
		gameServer_->sendInputEvent(event);
	}

	void
	GameApplication::update() noexcept
	{
		assert(gameServer_);
		gameServer_->update();
	}
}