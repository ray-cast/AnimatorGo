#ifndef OCTOON_INPUT_MANAGER_H_
#define OCTOON_INPUT_MANAGER_H_

#include <octoon/input/input_map.h>

namespace octoon
{
	namespace input
	{
		class InputManager
		{
		public:
			InputManager() noexcept;
			~InputManager() noexcept;

			void setInput();
			void setInputMap(InputMapPtr inputmap);

			bool isInput(const std::string& id) const;

			void sendInputEvent();

		private:
			InputMapPtr inputmap_;
		};
	}
}
#endif // !OCTOON_INPUT_MANAGER_H_
