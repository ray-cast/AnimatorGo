#ifndef OCTOON_INPUT_MAP_H_
#define OCTOON_INPUT_MAP_H_

#include <octoon/input/input_content.h>
#include <octoon/input/input.h>

#include <map>
#include <string>
#include <type_traits>

namespace octoon
{
	namespace input
	{
		namespace impl
		{
			inline void bind_ops(InputOp& ops)
			{
				ops.pop_back();
			}

			template <bool, typename First, typename ...Args>
			struct bind_ops_d;

			template <typename First, typename ...Args>
			struct bind_ops_d<false, First, Args...>
			{
				bind_ops_d(InputOp& ops, First&& first, Args&& ...args)
				{
					if (std::is_same<std::remove_reference_t<First>, InputKey::Code>::value)
					{
						ops.emplace_back(InputContent::Keyboard, (std::uint16_t)first);
					}
					else
					{
						ops.emplace_back(InputContent::Mouse, (std::uint16_t)first);
					}
					ops.emplace_back(InputContent::Sep, 0ui16);
					bind_ops(ops, args...);
				}
			};

			template <typename First, typename ...Args>
			struct bind_ops_d<true, First, Args...>
			{
				bind_ops_d(InputOp& ops, First&& first, Args&& ...args)
				{
					for (auto&& op : first)
					{
						ops.emplace_back(op);
					}
					ops.emplace_back(InputContent::Sep, 0ui16);
					bind_ops(ops, args...);
				}
			};

			template <typename First, typename ...Args>
			void bind_ops(InputOp& ops, First&& first, Args&& ...args)
			{
				bind_ops_d<std::is_same<std::remove_reference_t<First>, InputOp>::value, First, Args...>(ops, first, args...);
			}
		}

		typedef std::pair<std::string, InputOp> bind_t;

		template <typename First, typename ...Args>
		bind_t bind(const std::string& id, First&& first, Args&& ...args)
		{
			InputOp ops;
			impl::bind_ops(ops, first, args...);
			return bind_t(id, ops);
		}

		class InputMap final
		{
		public:
			InputMap() noexcept;
			~InputMap() noexcept;

			void add(const bind_t& bind);
			void remove(const std::string& id);

			bool isInput(const std::string& id) const;
		private:
			DefaultInputPtr input_;
			std::map<std::string, InputOp> table_;
		};
	}
}
#endif // !OCTOON_INPUT_MAP_H_

