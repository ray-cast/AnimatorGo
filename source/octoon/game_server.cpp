// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#include <octoon/game_server.h>
#include <octoon/game_scene.h>
#include <octoon/game_features.h>
#include <octoon/game_listener.h>

namespace octoon
{
	OctoonImplementSubClass(GameServer, runtime::RttiInterface, "GameServer")

	GameServer::GameServer() noexcept
		: isActive_(false)
		, isStopping_(false)
		, isQuitRequest_(false)
		, gameApp_(nullptr)
	{
	}

	GameServer::~GameServer() noexcept
	{
		this->close();
	}

	bool
	GameServer::open() noexcept
	{
		return true;
	}

	void
	GameServer::close() noexcept
	{
		try
		{
			if (gameListener_)
				gameListener_->onMessage("GameServer : Stopping.");

			for (auto& it : scenes_)
			{
				if (gameListener_)
					gameListener_->onMessage(std::string("GameServer : Stopping : ") + it->type_name() + " : " + it->getName());

				it->setActive(false);

				if (gameListener_)
					gameListener_->onMessage(std::string("GameServer : Stopped : ") + it->type_name() + " : " + it->getName());
			}

			for (auto& it : features_)
			{
				for (auto& scene : scenes_)
				{
					if (gameListener_)
						gameListener_->onMessage(std::string("GameServer : Stopping feature : ") + it->type_name() + " with scene : " + scene->getName());

					it->onCloseScene(scene);
				}
			}

			scenes_.clear();

			for (auto& it : features_)
			{
				if (gameListener_)
					gameListener_->onMessage(std::string("GameServer : Stopping : ") + it->type_name());

				it->setActive(false);

				if (gameListener_)
					gameListener_->onMessage(std::string("GameServer : Stopped : ") + it->type_name());
			}

			features_.clear();

			if (gameListener_)
				gameListener_->onMessage("GameServer : Stopped.");

			isActive_ = false;
			isQuitRequest_ = true;
		}
		catch (const std::exception& e)
		{
			isActive_ = false;
			isQuitRequest_ = true;

			if (gameListener_)
				gameListener_->onMessage(std::string("GameServer : except with ") + e.what());
		}
	}

	void
	GameServer::setGameListener(const GameListenerPtr& listener) noexcept
	{
		if (gameListener_ != listener)
		{
			for (auto& it : features_)
				it->setGameListener(listener);

			for (auto& it : scenes_)
				it->setGameListener(listener);

			gameListener_ = listener;
		}
	}

	const GameListenerPtr&
	GameServer::getGameListener() const noexcept
	{
		return gameListener_;
	}

	bool
	GameServer::isActive() const noexcept
	{
		return isActive_;
	}

	bool
	GameServer::isStopping() const noexcept
	{
		return isStopping_;
	}

	bool
	GameServer::isQuitRequest() const noexcept
	{
		return isQuitRequest_;
	}

	bool
	GameServer::openScene(const std::string& filename) noexcept
	{
		assert(!filename.empty());

		try
		{
			auto scene = std::make_shared<GameScene>();
			scene->setGameListener(gameListener_);

			return this->addScene(scene);
		}
		catch (const std::exception& e)
		{
			if (gameListener_)
				gameListener_->onMessage(e.what());

			return false;
		}
	}

	void
	GameServer::closeScene(const std::string& sceneName) noexcept
	{
		assert(!sceneName.empty());

		auto scene = this->findScene(sceneName);
		if (scene)
			this->closeScene(scene);
	}

	GameScenePtr
	GameServer::findScene(const std::string& sceneName) noexcept
	{
		assert(!sceneName.empty());

		for (auto& it : scenes_)
		{
			if (it->getName() == sceneName)
				return it;
		}

		return nullptr;
	}

	const GameScenes&
	GameServer::getScenes() const noexcept
	{
		return scenes_;
	}

	bool
	GameServer::addScene(const GameScenePtr& scene) noexcept
	{
		assert(scene);
		assert(std::find(scenes_.begin(), scenes_.end(), scene) == scenes_.end());

		try
		{
			if (gameListener_)
				gameListener_->onMessage(std::string("GameServer : Scene adding : ") + scene->getName());

			if (this->isActive())
			{
				for (auto& feature : features_)
				{
					if (gameListener_)
						gameListener_->onMessage(std::string("GameServer : Scene adding : ") + scene->getName() + " with feature : " + feature->type_name());

					feature->onOpenScene(scene);
				}

				scene->setActive(true);
			}

			scenes_.push_back(scene);

			if (gameListener_)
				gameListener_->onMessage(std::string("GameServer : Scene added : ") + scene->getName());

			return true;
		}
		catch (const std::exception& e)
		{
			if (gameListener_)
				gameListener_->onMessage(e.what());

			return false;
		}
	}

	void
	GameServer::closeScene(const GameScenePtr& scene) noexcept
	{
		assert(scene);

		auto it = std::find(scenes_.begin(), scenes_.end(), scene);
		if (it != scenes_.end())
		{
			if (gameListener_)
				gameListener_->onMessage(std::string("GameServer : Scene removeing : ") + scene->getName());

			if (this->isActive())
			{
				for (auto& feature : features_)
					feature->onCloseScene(*it);

				(*it)->setActive(false);
			}

			scenes_.erase(it);

			if (gameListener_)
				gameListener_->onMessage(std::string("GameServer : scene Removed : ") + scene->getName());
		}
	}

	bool
	GameServer::addFeature(GameFeaturePtr& feature) noexcept
	{
		assert(feature);

		try
		{
			auto it = std::find_if(features_.begin(), features_.end(), [feature](GameFeaturePtr it) { return feature->isInstanceOf(it->rtti()); });
			if (it == features_.end())
			{
				if (gameListener_)
					gameListener_->onMessage(std::string("GameServer : Feature adding : ") + feature->type_name());

				feature->_setGameServer(this);
				feature->setGameListener(gameListener_);

				if (this->isActive())
				{
					feature->onActivate();

					for (auto& scene : scenes_)
					{
						if (gameListener_)
							gameListener_->onMessage(std::string("GameServer : Feature adding : ") + feature->type_name() + " with scene : " + scene->getName());

						feature->onOpenScene(scene);
					}
				}

				features_.push_back(feature);

				if (gameListener_)
					gameListener_->onMessage(std::string("GameServer : Feature added : ") + feature->type_name());

				return true;
			}
			else
			{
				if (gameListener_)
					gameListener_->onMessage(std::string("GameServer : Feature has already added with : ") + feature->type_name());

				return false;
			}
		}
		catch (const std::exception& e)
		{
			if (gameListener_)
				gameListener_->onMessage(e.what());

			return false;
		}
	}

	void
	GameServer::removeFeature(GameFeaturePtr& feature) noexcept
	{
		assert(feature);
		auto it = std::find(features_.begin(), features_.end(), feature);
		if (it != features_.end())
		{
			if (gameListener_)
				gameListener_->onMessage(std::string("GameServer : Feature removeing: ") + feature->type_name());

			(*it)->onDeactivate();
			(*it)->_setGameServer(nullptr);

			features_.erase(it);

			if (gameListener_)
				gameListener_->onMessage(std::string("GameServer : Feature removed: ") + feature->type_name());
		}
	}

	GameFeaturePtr
	GameServer::getFeature(const runtime::Rtti& rtti) const noexcept
	{
		for (auto& it : features_)
		{
			if (it->isInstanceOf(rtti))
				return it;
		}

		return nullptr;
	}

	GameFeaturePtr
	GameServer::getFeature(const runtime::Rtti* rtti) const noexcept
	{
		for (auto& it : features_)
		{
			if (it->isInstanceOf(rtti))
				return it;
		}

		return nullptr;
	}

	const GameFeatures&
	GameServer::getGameFeatures() const noexcept
	{
		return features_;
	}

	void
	GameServer::_setGameApp(GameApplication* app) noexcept
	{
		gameApp_ = app;
	}

	GameApplication*
	GameServer::getGameApp() noexcept
	{
		return gameApp_;
	}

	bool
	GameServer::start() noexcept
	{
		if (this->isQuitRequest())
			return false;

		if (isActive_)
		{
			if (gameListener_)
				gameListener_->onMessage("GameServer : The server has been started.");

			return true;
		}

		if (gameListener_)
			gameListener_->onMessage("GameServer : Starting.");

		try
		{
			for (auto& it : features_)
			{
				if (gameListener_)
					gameListener_->onMessage(std::string("GameServer : Starting : ") + it->type_name());

				it->setActive(true);

				if (gameListener_)
					gameListener_->onMessage(std::string("GameServer : Started : ") + it->type_name());
			}

			for (auto& it : features_)
			{
				for (auto& scene : scenes_)
				{
					if (gameListener_)
						gameListener_->onMessage(std::string("GameServer : Starting feature : ") + it->type_name() + " with scene : " + scene->getName());

					it->onOpenScene(scene);
				}
			}

			for (auto& it : scenes_)
			{
				if (gameListener_)
					gameListener_->onMessage(std::string("GameServer : Starting scene : ") + it->getName());

				it->setActive(true);
			}

			isActive_ = true;

			if (gameListener_)
				gameListener_->onMessage("GameServer : Started.");

			return true;
		}
		catch (const std::exception& e)
		{
			isQuitRequest_ = true;

			if (gameListener_)
				gameListener_->onMessage(std::string("GameServer : except with ") + e.what());

			return false;
		}
	}

	void
	GameServer::stop() noexcept
	{
		if (!this->isStopping())
		{
			if (gameListener_)
				gameListener_->onMessage("GameServer : stopped.");

			isStopping_ = false;
		}
		else
		{
			if (gameListener_)
				gameListener_->onMessage("GameServer : The server has been stoped.");
		}
	}

	void
	GameServer::update() noexcept
	{
		if (this->isQuitRequest() || !this->isActive() || this->isStopping())
			return;

		try
		{
			if (!isQuitRequest_)
			{
				for (auto& it : features_)
					it->onFrameBegin();

				for (auto& it : features_)
					it->onFrame();

				for (auto& it : features_)
					it->onFrameEnd();
			}
		}
		catch (const std::exception& e)
		{
			if (gameListener_)
				gameListener_->onMessage(e.what());

			isQuitRequest_ = true;
		}
	}
}