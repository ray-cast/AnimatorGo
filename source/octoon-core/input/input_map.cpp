#include <octoon/input/input_map.h>
#include <octoon/runtime/except.h>

namespace octoon
{
	namespace input
	{
		InputMap::InputMap() noexcept
		{
		}

		InputMap::~InputMap() noexcept
		{
		}

		void
		InputMap::add(const bind_t& bind)
		{
			auto&& id = bind.first;
			auto pos = table_.find(id);
			if (pos != table_.end())
			{
				throw runtime_error::create("Bind duplicate id: " + id);
			}
			table_.emplace_hint(pos, id, bind.second);
		}

		void
		InputMap::remove(const std::string& id)
		{
			table_.erase(id);
		}

		bool
		InputMap::isInput(const std::string& id) const
		{
			auto pos = table_.find(id);
			if (pos == table_.end())
			{
				throw runtime_error::create("No InputMap id: " + id);
			}
			auto&& ops = pos->second;

			// TODO: does not support combined key
			std::size_t i = 0;
			for (; i < ops.size() - 1; ++i)
			{
				if (ops[i].input == InputContent::Keyboard)
				{
					if (!input_->isKeyPressed((InputKey::Code)ops[i].code)) return false;
				}
				else if(ops[i].input == InputContent::Mouse)
				{
					if (!input_->isButtonPressed((InputButton::Code)ops[i].code)) return false;
				}
				else
				{
					throw runtime_error::create("InputMap doesn't support combined key yet.");
				}
			}
			if (ops[i].input == InputContent::Keyboard)
			{
				if (!input_->isKeyDown((InputKey::Code)ops[i].code)) return false;
			}
			else if (ops[i].input == InputContent::Mouse)
			{
				if (!input_->isButtonDown((InputButton::Code)ops[i].code)) return false;
			}
			else
			{
				throw runtime_error::create("InputMap doesn't support combined key yet.");
			}
			return true;
		}
	}
}