#include <octoon/input/input_map.h>
#include <octoon/runtime/except.h>

namespace octoon
{
	namespace input
	{
		void
		InputMap::bind(const std::string& id, InputKey::Code code)
		{
			InputOp op;
			op.emplace_back(InputContent::Keyboard, (std::uint16_t)code);
			table[id] = op;
		}

		void
		InputMap::bind(const std::string& id, InputButton::Code code)
		{
			InputOp op;
			op.emplace_back(InputContent::Mouse, (std::uint16_t)code);
			table[id] = op;
		}

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
		InputMap::bind(const std::string& id, InputOp ops, InputKey::Code code)
		{
			if (ops.empty())
			{
				throw runtime::runtime_error::create("ops is empty when binding " + id);
			}
			InputOp op(std::move(ops));
			op.emplace_back(InputContent::Keyboard, (std::uint16_t)code);
			table[id] = op;
		}

		void
		InputMap::bind(const std::string& id, InputOp ops, InputButton::Code code)
		{
			if (ops.empty())
			{
				throw runtime::runtime_error::create("ops is empty when binding " + id);
			}
			InputOp op(std::move(ops));
			op.emplace_back(InputContent::Mouse, (std::uint16_t)code);
			table[id] = op;
		}

		void
		InputMap::bind(const std::string& id, InputKey::Code code, InputOp ops)
		{
			if (ops.empty())
			{
				throw runtime::runtime_error::create("ops is empty when binding " + id);
			}
			InputOp op(std::move(ops));
			op.emplace(op.begin(), InputContent::Keyboard, (std::uint16_t)code);
			table[id] = op;
		}

		void
		InputMap::bind(const std::string& id, InputButton::Code code, InputOp ops)
		{
			if (ops.empty())
			{
				throw runtime::runtime_error::create("ops is empty when binding " + id);
			}
			InputOp op(std::move(ops));
			op.emplace(op.begin(), InputContent::Mouse, (std::uint16_t)code);
			table[id] = op;
		}

		void
		InputMap::bind(const std::string& id, InputOp ops1, InputOp ops2)
		{
			if (ops1.empty() || ops2.empty())
			{
				throw runtime::runtime_error::create("ops is empty when binding " + id);
			}
			InputOp ops(ops1.size() + ops2.size() + 1);
			std::move(ops1.begin(), ops1.end(), ops.begin());
			ops[ops1.size()].input = InputContent::Sep;
			std::move(ops2.begin(), ops2.end(), ops.begin() + ops1.size() + 1);
			table[id] = ops;
		}

		void
		InputMap::unbind(const std::string& id)
		{
			table.erase(id);
		}
	}
}