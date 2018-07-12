#ifndef OCTOON_VARIANT_H_
#define OCTOON_VARIANT_H_

#include "variant/variant.hpp"

namespace octoon
{
	namespace runtime
	{
		template<typename ...T>
		using variant = mpark::variant<T...>;
	}
}

#endif