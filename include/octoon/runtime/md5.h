#ifndef OCTOON_BASE32_H_
#define OCTOON_BASE32_H_

#include <fstream>
#include <octoon/runtime/platform.h>

namespace octoon
{
	class OCTOON_EXPORT MD5 final
	{
	public:
		MD5();
		MD5(const void* input, size_t length);
		MD5(const std::string& str);
		MD5(const std::u8string& str);
		MD5(std::ifstream& in);

		void update(const void* input, size_t length);
		void update(const std::string& str);
		void update(const std::u8string& str);
		void update(std::ifstream& in);

		const byte* digest();
		std::string toString();
		void reset();

	private:
		void update(const byte* input, size_t length);
		void final();
		void transform(const byte block[64]);
		void encode(const std::uint32_t* input, byte* output, size_t length);
		void decode(const byte* input, std::uint32_t* output, size_t length);
		std::string bytesToHexString(const byte* input, size_t length);

	private:
		MD5(const MD5&);
		MD5& operator=(const MD5&);

	private:
		std::uint32_t _state[4];
		std::uint32_t _count[2];
		byte _buffer[64];
		byte _digest[16];
		bool _finished;

		static const byte PADDING[64];
		static const char HEX[16];
		enum { BUFFER_SIZE = 1024 };
	};
}

#endif