#ifndef OCTOON_INPUT_MANAGER_H_
#define OCTOON_INPUT_MANAGER_H_

#include <octoon/input/input_map.h>
#include <octoon/input/input_event.h>

namespace octoon
{
	namespace input
	{
		class InputManager
		{
		public:
			InputManager() noexcept;
			~InputManager() noexcept;

			void setInputMap(InputMapPtr inputmap);

			bool isInput(const std::string& id) const;

			void sendInputEvent(const InputEvent& event);

		private:
			InputMapPtr inputmap_;
		};
	}
}
#endif // !OCTOON_INPUT_MANAGER_H_
