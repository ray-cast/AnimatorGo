#ifndef OCTOON_ANY_H_
#define OCTOON_ANY_H_

#include "any/any.hpp"

namespace octoon
{
	namespace runtime
	{
		using any = linb::any;

		template<typename T>
		inline T any_cast(const any& operand)
		{
			return linb::any_cast<T>(operand);
		}

		template<typename T>
		inline T any_cast(any& operand)
		{
			return linb::any_cast<T>(operand);
		}

		template<typename T>
		inline T any_cast(any&& operand)
		{
			return linb::any_cast<T>(std::move(operand));
		}

		template<typename T>
		inline const T* any_cast(const any* operand) noexcept
		{
			return linb::any_cast<T>(operand);
		}

		template<typename T>
		inline T* any_cast(any* operand) noexcept
		{
			return linb::any_cast<T>(operand);
		}
	}
}

#endif