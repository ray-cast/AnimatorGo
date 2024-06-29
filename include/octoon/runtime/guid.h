#ifndef OCTOON_GUID_H_
#define OCTOON_GUID_H_

#include <string>
#include <sstream>
#include <octoon/runtime/platform.h>

namespace octoon
{
	class OCTOON_EXPORT Guid final
	{
	public:
		Guid() noexcept
		{
			this->generate();
		}

		std::uint32_t data1;
		std::uint16_t data2;
		std::uint16_t data3;
		std::uint8_t  data4[8];

		Guid& generate() noexcept;

		std::string to_string() const noexcept
		{
			char str[64];
			sprintf(str, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
				data1,
				data2,
				data3,
				data4[0], data4[1], data4[2], data4[3],
				data4[4], data4[5], data4[6], data4[7]);

			return str;
		}

		friend inline std::ostream& operator<<(std::ostream& o, const Guid& uuid) noexcept
		{
			char str[64];
			sprintf(str, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
				uuid.data1,
				uuid.data2,
				uuid.data3,
				uuid.data4[0], uuid.data4[1], uuid.data4[2], uuid.data4[3],
				uuid.data4[4], uuid.data4[5], uuid.data4[6], uuid.data4[7]);

			o << str;
			return o;
		}
	};

	inline std::string make_guid() noexcept
	{
		Guid guid;
		return guid.to_string();
	}
}

#endif