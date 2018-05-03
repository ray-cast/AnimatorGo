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
			template <typename Last>
			void bind_ops_dispatch(std::true_type, InputOp& ops, Last&& last)
			{
				for (auto&& op : last) ops.emplace_back(op);
			}

			template <typename Last>
			void bind_ops_dispatch(std::false_type, InputOp& ops, Last&& last)
			{
				if (std::is_same<std::remove_cv_t<Last>, InputKey>::value)
				{
					ops.emplace_back(InputContent::Keyboard, (std::uint16_t)last);
				}
				else
				{
					ops.emplace_back(InputContent::Mouse, (std::uint16_t)last);
				}
			}

			template <typename First, typename ...Args>
			void bind_ops_dispatch(std::true_type, InputOp& ops, First&& first, Args&& ...args)
			{
				for (auto&& op : first)
				{
					ops.emplace_back(op);
				}
				ops.emplace_back(InputContent::Sep, 0ui16);
				bind_ops(ops, args...);
			}

			template <typename First, typename ...Args>
			void bind_ops_dispatch(std::false_type, InputOp& ops, First&& first, Args&& ...args)
			{
				if (std::is_same<std::remove_cv_t<First>, InputKey>::value)
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

			template <typename First, typename ...Args>
			void bind_ops(InputOp& ops, First&& first, Args&& ...args) 
			{
				bind_ops_dispatch(std::is_same<std::remove_cv_t<First>, InputOp>(),
					std::forward<First>(first), std::forward<Args>(args)...);
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

		//bind_t bind(const std::string& id, InputKey::Code code);
		//bind_t bind(const std::string& id, InputButton::Code code);
		//bind_t bind(const std::string& id, const InputOp& ops);

		//bind_t bind(const std::string& id, InputOp ops, InputKey::Code code);
		//bind_t bind(const std::string& id, InputOp ops, InputButton::Code code);

		//bind_t bind(const std::string& id, InputKey::Code code, InputOp ops);
		//bind_t bind(const std::string& id, InputButton::Code code, InputOp ops);

		//bind_t bind(const std::string& id, InputOp ops1, InputOp ops2);

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

