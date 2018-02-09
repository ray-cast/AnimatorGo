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
#include <octoon/game_scene.h>
#include <octoon/game_scene_manager.h>
#include <octoon/game_listener.h>
#include <octoon/game_component.h>

namespace octoon
{
	OctoonImplementSubClass(GameScene, runtime::RttiInterface, "GameScene")

	GameScene::RootObject::RootObject(GameScene* scene) noexcept
		: _scene(scene)
	{
		assert(scene);
	}

	GameScene::RootObject::~RootObject() noexcept
	{
	}

	GameScene*
	GameScene::RootObject::getGameScene() noexcept
	{
		return _scene;
	}

	const GameScene*
	GameScene::RootObject::getGameScene() const noexcept
	{
		return _scene;
	}

	GameScene::GameScene() noexcept
		: _root(std::make_unique<RootObject>(this))
	{
		GameSceneManager::instance()->_instanceScene(this, _instanceID);
	}

	GameScene::GameScene(const std::string& name) noexcept
		: _name(name)
	{
		GameSceneManager::instance()->_instanceScene(this, _instanceID);
	}

	GameScene::~GameScene() noexcept
	{
		assert(_root.unique());

		this->setActive(false);
		_root.unique();

		GameSceneManager::instance()->_unsetScene(this);
	}

	void
	GameScene::setActive(bool active) except
	{
		if (this->getActive() != active)
		{
			GameSceneManager::instance()->_activeScene(this, active);

			_root->setActiveDownwards(active);
		}
	}

	bool
	GameScene::getActive() const noexcept
	{
		return _root->getActive();
	}

	void
	GameScene::setGameListener(const GameListenerPtr& listener) noexcept
	{
		if (_gameListener != listener)
			_gameListener = listener;
	}

	GameListenerPtr
	GameScene::getGameListener() const noexcept
	{
		return _gameListener;
	}

	void
	GameScene::setName(std::string&& name) noexcept
	{
		_name = std::move(name);
	}

	void
	GameScene::setName(const std::string& name) noexcept
	{
		_name = name;
	}

	const std::string&
	GameScene::getName() const noexcept
	{
		return _name;
	}

	std::size_t
	GameScene::getInstanceID() const noexcept
	{
		return _instanceID;
	}

	GameObjectPtr
	GameScene::getRootObject() const noexcept
	{
		return _root;
	}

	GameScenePtr
	GameScene::clone() const noexcept
	{
		auto scene = std::make_shared<GameScene>();
		scene->setName(this->getName());
		scene->_root = _root->clone();
		return scene;
	}
}