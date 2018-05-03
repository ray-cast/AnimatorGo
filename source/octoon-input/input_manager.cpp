#include <octoon/input/input_manager.h>

namespace octoon
{
	namespace input
	{
		InputManager::InputManager() noexcept
		{
		}

		InputManager::~InputManager() noexcept
		{
		}

		void
		InputManager::setInputMap(InputMapPtr inputmap)
		{
			inputmap_ = inputmap;
		}
	}
}