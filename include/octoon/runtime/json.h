#ifndef OCTOON_JSON_H_
#define OCTOON_JSON_H_

#pragma warning(push)
#pragma warning(disable : 4996)

#include "json/json.hpp"

#pragma warning(pop) 

namespace octoon
{
	namespace runtime
	{
		using json = nlohmann::json;
	}
}

#endif