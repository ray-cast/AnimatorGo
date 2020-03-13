#include <octoon/runtime/uuid.h>
#include <chrono>

#ifndef OCTOON_BUILD_PLATFORM_EMSCRIPTEN
#	if defined(__GNUC__) || defined(__clang__)
#		include <cpuid.h>
#	elif defined _MSC_VER
#		include <intrin.h>
#	else
#		error unknown complier
#	endif
#endif

static std::uint16_t sequence = 0;
static auto start_time = std::chrono::high_resolution_clock::now();

namespace octoon
{
	namespace runtime
	{
		void cpuid(std::uint32_t func, std::uint32_t sub, std::uint32_t data[4])
		{
#ifndef OCTOON_BUILD_PLATFORM_EMSCRIPTEN
#	if defined(__GNUC__) || defined(__clang__)
			__cpuid_count(func, sub, data[0], data[1], data[2], data[3]);
#	elif defined _MSC_VER
			__cpuidex(reinterpret_cast<int*>(data), func, sub);
#	endif
#else
			data[0] = 0;
			data[1] = 0;
			data[2] = 0;
			data[3] = 0;
#endif
		}

		Guid&
		Guid::generate() noexcept
		{
#ifdef __WINDOWS__
			UUID uuid;
			CoCreateGuid(&uuid);

			this->data1 = uuid.Data1;
			this->data2 = uuid.Data2;
			this->data3 = uuid.Data3;
			this->data4[0] = uuid.Data4[0];
			this->data4[1] = uuid.Data4[1];
			this->data4[2] = uuid.Data4[2];
			this->data4[3] = uuid.Data4[3];
			this->data4[4] = uuid.Data4[4];
			this->data4[5] = uuid.Data4[5];
			this->data4[6] = uuid.Data4[6];
			this->data4[7] = uuid.Data4[7];
#else
			std::uint32_t data[4];
			cpuid(0x80000002, 0, data);

			this->data1 = data[0] ^ (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time).count() & 0xFFFFFFFF);
			this->data2 = data[1] & 0xFFFF;
			this->data3 = data[2] ^ (sequence++ & 0xFFFF);

			std::uint64_t time = std::chrono::system_clock::now().time_since_epoch().count();
			this->data4[0] = (time >> 0) & 0xFF;
			this->data4[1] = (time >> 8) & 0xFF;
			this->data4[2] = (time >> 16) & 0xFF;
			this->data4[3] = (time >> 24) & 0xFF;
			this->data4[4] = (time >> 32) & 0xFF;
			this->data4[5] = (time >> 40) & 0xFF;
			this->data4[6] = (time >> 48) & 0xFF;
			this->data4[7] = (time >> 56) & 0xFF;
#endif
			return *this;
		}
	}
}