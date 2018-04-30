#ifndef OCTOON_INPUT_MAP_H_
#define OCTOON_INPUT_MAP_H_

#include <octoon/input/input_content.h>

#include <map>
#include <string>

namespace octoon
{
	namespace input
	{

		class InputMap final
		{
		public:
			friend class DefaultInput;

			static void bind(const std::string& id, const InputOp& ops);
			static void bind(const std::string& id, const InputOp& ops1, const InputOp& ops2);

		private:
			InputMap() = delete;
			InputMap(const InputMap&) = delete;

			static std::map<std::string, InputOp> table;
		};
	}
}
#endif // !OCTOON_INPUT_MAP_H_

