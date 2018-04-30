#include <octoon/input/input_map.h>
#include <octoon/runtime/except.h>

namespace octoon
{
	namespace input
	{
		void
		InputMap::bind(const std::string& id, const InputOp& ops)
		{
			if (ops.empty())
			{
				throw runtime::runtime_error::create("ops is empty when binding " + id);
			}
			table[id] = ops;
		}

		void
		InputMap::bind(const std::string& id, const InputOp& ops1, const InputOp& ops2)
		{
			if (ops1.empty() || ops2.empty())
			{
				throw runtime::runtime_error::create("ops is empty when binding " + id);
			}
			InputOp ops(ops1.size() + ops2.size() + 1);
			std::copy(ops1.begin(), ops1.end(), ops.begin());
			ops[ops1.size()].input = InputContent::Sep;
			std::copy(ops2.begin(), ops2.end(), ops.begin() + ops1.size() + 1);
			table[id] = ops;
		}
	}
}