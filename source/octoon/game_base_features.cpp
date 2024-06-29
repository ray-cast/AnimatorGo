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
	GameBaseFeature::onInputEvent(const std::any& data) noexcept(false)
	{
		assert(data.type() == typeid(input::InputEvent));

		auto event = std::any_cast<input::InputEvent>(data);
		switch (event.event)
		{
		case input::InputEvent::MouseMotion:
			GameObjectManager::instance()->sendMessage("feature:input:mousemove", event);
			break;
		case input::InputEvent::MouseButtonDown:
			GameObjectManager::instance()->sendMessage("feature:input:mousedown", event);
			break;
		case input::InputEvent::MouseButtonUp:
			GameObjectManager::instance()->sendMessage("feature:input:mouseup", event);
			break;
		case input::InputEvent::KeyDown:
			GameObjectManager::instance()->sendMessage("feature:input:keydown", event);
			break;
		case input::InputEvent::KeyUp:
			GameObjectManager::instance()->sendMessage("feature:input:keyup", event);
			break;
		case input::InputEvent::Character:
			GameObjectManager::instance()->sendMessage("feature:input:character", event);
			break;
		case input::InputEvent::LostFocus:
			GameObjectManager::instance()->sendMessage("feature:input:blur", event);
			break;
		case input::InputEvent::GetFocus:
			GameObjectManager::instance()->sendMessage("feature:input:focus", event);
			break;
		case input::InputEvent::MouseWheelUp:
			GameObjectManager::instance()->sendMessage("feature:input:wheel", event);
			break;
		case input::InputEvent::MouseWheelDown:
			GameObjectManager::instance()->sendMessage("feature:input:wheel", event);
			break;
		case input::InputEvent::SizeChange:
			GameObjectManager::instance()->sendMessage("feature:input:resize", event);
			break;
		case input::InputEvent::SizeChangeDPI:
			GameObjectManager::instance()->sendMessage("feature:input:resize", event);
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