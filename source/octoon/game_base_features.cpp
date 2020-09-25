#include <octoon/game_base_features.h>
#include <octoon/game_object_manager.h>
#include <octoon/game_listener.h>
#include <octoon/input/input.h>

namespace octoon
{
	OctoonImplementSubClass(GameBaseFeature, GameFeature, "GameBaseFeatures")

	GameBaseFeature::GameBaseFeature() noexcept
	{
	}

	GameBaseFeature::~GameBaseFeature() noexcept
	{
	}

	GameObjectManager*
	GameBaseFeature::getGameObjectManager() const noexcept
	{
		return GameObjectManager::instance();
	}

	void
	GameBaseFeature::log(std::string_view message) noexcept
	{
		if (this->getGameListener())
			this->getGameListener()->onMessage(message);
	}

    void
	GameBaseFeature::onActivate() noexcept
    {
		this->addMessageListener("feature:input:event", std::bind(&GameBaseFeature::onInputEvent, this, std::placeholders::_1));
		this->addMessageListener("feature:timer:fixed", std::bind(&GameBaseFeature::onFixedUpdate, this, std::placeholders::_1));
    }

	void
	GameBaseFeature::onFrameBegin() noexcept
	{
	}

	void
	GameBaseFeature::onFrame() noexcept
	{
		try
		{
			GameObjectManager::instance()->onUpdate();
			GameObjectManager::instance()->onLateUpdate();
		}
		catch (const std::exception& e)
		{
			this->getGameListener()->onMessage(e.what());
		}
	}

	void
	GameBaseFeature::onFrameEnd() noexcept
	{		
	}

	void
	GameBaseFeature::onInputEvent(const std::any& data) noexcept
	{
		assert(data.type() == typeid(input::InputEvent));

		auto event = std::any_cast<input::InputEvent>(data);
		switch (event.event)
		{
		case input::InputEvent::MouseMotion:
			break;
		case input::InputEvent::MouseButtonDown:
			break;
		case input::InputEvent::MouseButtonUp:
			break;
		case input::InputEvent::KeyDown:
			break;
		case input::InputEvent::KeyUp:
			break;
		case input::InputEvent::Character:
			break;
		case input::InputEvent::LostFocus:
			break;
		case input::InputEvent::GetFocus:
			break;
		case input::InputEvent::MouseWheelUp:
			break;
		case input::InputEvent::MouseWheelDown:
			break;
		case input::InputEvent::SizeChange:
			GameObjectManager::instance()->sendMessage("feature:input:event", event);
			break;
		case input::InputEvent::SizeChangeDPI:
			GameObjectManager::instance()->sendMessage("feature:input:event", event);
			break;
		case input::InputEvent::Drop:
			{
				std::vector<const char*> files(event.drop.count);
				for (std::size_t i = 0; i < event.drop.count; i++)
					files[i] = event.drop.files[i];
				GameObjectManager::instance()->sendMessage("feature:input:drop", files);
			}
			break;
		default:
			return;
		}
	}

	void
	GameBaseFeature::onFixedUpdate(const std::any& data) noexcept
	{
		GameObjectManager::instance()->onFixedUpdate();
	}
}