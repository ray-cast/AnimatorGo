#ifndef OCTOON_UUID_H_
#define OCTOON_UUID_H_

#include <string>
#include <octoon/runtime/platform.h>

namespace octoon
{
	namespace runtime
	{
		struct GUID
		{
			std::uint32_t data1;
			std::uint16_t data2;
			std::uint16_t data3;
			std::uint8_t  data4[8];
		};

		OCTOON_EXPORT void guid_generate(GUID& guid);
		OCTOON_EXPORT void guid_to_string(const GUID& guid, char str[64]);
	}
}

#endif