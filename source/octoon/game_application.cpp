#include <octoon/game_application.h>
#include <octoon/game_server.h>
#include <octoon/game_listener.h>

#include <octoon/runtime/rtti_factory.h>

#include <iostream>

#include "input_feature.h"

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
		: _isInitialize(false)
		, _gameServer(nullptr)
		, _gameListener(std::make_shared<GameAppListener>())
		, _workDir("")
		, _engineDir("../../octoon/")
		, _resourceBaseDir("../../media/")
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
		if (_isInitialize)
		{
			if (_gameListener)
				_gameListener->onMessage("Game Application has already opened.");

			return false;
		}

		auto local = setlocale(LC_ALL, "");
		if (local)
		{
			if (_gameListener)
				_gameListener->onMessage(std::string("Initializing : Local : ") + local);
		}

		if (_gameListener)
			_gameListener->onMessage("Initializing : Game Application.");

		if (_gameListener)
			_gameListener->onMessage("Initializing : RTTI.");

		if (!runtime::RttiFactory::instance()->open())
		{
			if (_gameListener)
				_gameListener->onMessage("Could not initialize with RTTI.");

			return false;
		}

		if (_gameListener)
			_gameListener->onMessage("Initializing : Game Server.");

		_gameServer = GameServer::instance();
		_gameServer->_setGameApp(this);
		_gameServer->setGameListener(_gameListener);

		if (!_gameServer->open())
			return false;

		_inputFeature = std::make_shared<InputFeature>((input::WindHandle)hwnd);
		this->addFeatures(_inputFeature);

		_isInitialize = this->start();
		return _isInitialize;
	}

	void
	GameApplication::close() noexcept
	{
		if (_gameListener)
			_gameListener->onMessage("Shutdown : Game Server.");

		if (_gameServer)
		{
			_gameServer->close();
			_gameServer = nullptr;
		}

		if (_gameListener)
			_gameListener->onMessage("Shutdown : IO Server.");
	}

	void
	GameApplication::setGameListener(const GameListenerPtr& listener) noexcept
	{
		_gameServer->setGameListener(listener);
	}

	const GameListenerPtr&
	GameApplication::getGameListener() const noexcept
	{
		return _gameServer->getGameListener();
	}

	bool
	GameApplication::start() noexcept
	{
		if (_gameServer)
			return _gameServer->start();
		return false;
	}

	void
	GameApplication::stop() noexcept
	{
		if (_gameServer)
			_gameServer->stop();
	}

	bool
	GameApplication::isQuitRequest() const noexcept
	{
		assert(_gameServer);
		return _gameServer->isQuitRequest();
	}

	bool
	GameApplication::openScene(const GameScenePtr& scene) noexcept
	{
		assert(_gameServer);
		return _gameServer->addScene(scene);
	}

	bool
	GameApplication::openScene(const std::string& name) noexcept
	{
		assert(_gameServer);
		return _gameServer->openScene(name);
	}

	void
	GameApplication::closeScene(const GameScenePtr& name) noexcept
	{
		assert(_gameServer);
		return _gameServer->closeScene(name);
	}

	void
	GameApplication::closeScene(const std::string& name) noexcept
	{
		assert(_gameServer);
		_gameServer->closeScene(name);
	}

	GameScenePtr
	GameApplication::findScene(const std::string& name) noexcept
	{
		assert(_gameServer);
		return _gameServer->findScene(name);
	}

	bool
	GameApplication::addFeatures(GameFeaturePtr& feature) noexcept
	{
		assert(_gameServer);
		return _gameServer->addFeature(feature);
	}

	void
	GameApplication::removeFeatures(GameFeaturePtr& feature) noexcept
	{
		assert(_gameServer);
		_gameServer->removeFeature(feature);
	}

	bool
	GameApplication::sendInputEvent(const input::InputEvent& event) noexcept
	{
		if (_inputFeature)
			return _inputFeature->downcast<InputFeature>()->sendInputEvent(event);
		return false;
	}

	bool
	GameApplication::postInputEvent(const input::InputEvent& event) noexcept
	{
		if (_inputFeature)
			return _inputFeature->downcast<InputFeature>()->postInputEvent(event);
		return false;
	}

	void
	GameApplication::update() noexcept
	{
		assert(_gameServer);
		_gameServer->update();
	}
}