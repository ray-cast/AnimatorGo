#include <octoon/game_base_features.h>
#include <octoon/game_object_manager.h>
#include <octoon/model/text_system.h>
#include <octoon/input/input.h>

namespace octoon
{
	OctoonImplementSubClass(GameBaseFeature, GameFeature, "GameBaseFeatures")

	GameBaseFeature::GameBaseFeature() noexcept
	{
		model::TextSystem::instance()->setup();
	}

	GameBaseFeature::~GameBaseFeature() noexcept
	{
		model::TextSystem::instance()->close();
	}

	GameObjectManager*
	GameBaseFeature::getGameObjectManager() const noexcept
	{
		return GameObjectManager::instance();
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
		GameObjectManager::instance()->onUpdate();
		GameObjectManager::instance()->onLateUpdate();
	}

	void
	GameBaseFeature::onFrameEnd() noexcept
	{		
	}

	void
	GameBaseFeature::onInputEvent(const runtime::any& data) noexcept
	{
		assert(data.type() == typeid(input::InputEvent));

		auto event = runtime::any_cast<input::InputEvent>(data);
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
			break;
		case input::InputEvent::SizeChangeDPI:
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
	GameBaseFeature::onFixedUpdate(const runtime::any& data) noexcept
	{
		GameObjectManager::instance()->onFixedUpdate();
	}
}