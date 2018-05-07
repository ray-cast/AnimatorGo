#include <octoon/runtime/uuid.h>
#include <chrono>

#if defined(__GNUC__) || defined(__clang__)
#	include <cpuid.h>
#elif defined _MSC_VER
#	include <intrin.h>
#else
#	error unknown complier
#endif

static std::uint16_t sequence = 0;
static auto start_time = std::chrono::high_resolution_clock::now();

namespace octoon
{
	namespace runtime
	{
		void cpuid(std::uint32_t func, std::uint32_t sub, std::uint32_t data[4])
		{
#if defined(__GNUC__) || defined(__clang__)
			__cpuid_count(func, sub, data[0], data[1], data[2], data[3]);
#elif defined _MSC_VER
			__cpuidex(reinterpret_cast<int*>(data), func, sub);
#endif
		}

		void guid_generate(GUID& guid)
		{
#ifdef __WINDOWS__
			UUID uuid;
			CoCreateGuid(&uuid);

			guid.data1 = uuid.Data1;
			guid.data2 = uuid.Data2;
			guid.data3 = uuid.Data3;
			guid.data4[0] = uuid.Data4[0];
			guid.data4[1] = uuid.Data4[1];
			guid.data4[2] = uuid.Data4[2];
			guid.data4[3] = uuid.Data4[3];
			guid.data4[4] = uuid.Data4[4];
			guid.data4[5] = uuid.Data4[5];
			guid.data4[6] = uuid.Data4[6];
			guid.data4[7] = uuid.Data4[7];
#else
			std::uint32_t data[4];
			cpuid(0x80000002, 0, data);

			guid.data1 = data[0] ^ (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time).count() & 0xFFFFFFFF);
			guid.data2 = data[1] & 0xFFFF;
			guid.data3 = data[2] ^ (sequence++ & 0xFFFF);

			std::uint64_t time = std::chrono::system_clock::now().time_since_epoch().count();
			guid.data4[0] = (time >> 0) & 0xFF;
			guid.data4[1] = (time >> 8) & 0xFF;
			guid.data4[2] = (time >> 16) & 0xFF;
			guid.data4[3] = (time >> 32) & 0xFF;
			guid.data4[4] = (time >> 40) & 0xFF;
			guid.data4[5] = (time >> 48) & 0xFF;
			guid.data4[6] = (time >> 56) & 0xFF;
			guid.data4[7] = (time >> 64) & 0xFF;
#endif
		}

		void guid_to_string(const GUID& uuid, char GUID[64])
		{
			sprintf(GUID, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
				uuid.data1, uuid.data2, uuid.data3, uuid.data4[0], uuid.data4[1], uuid.data4[2],
				uuid.data4[3], uuid.data4[4], uuid.data4[5], uuid.data4[6], uuid.data4[7]);
		}
	}
}