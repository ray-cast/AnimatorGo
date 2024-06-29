#ifndef OCTOON_TEXT_READER_H_
#define OCTOON_TEXT_READER_H_

#include <string>
#include <cstdint>
#include <octoon/io/istream.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT TextReader
		{
		public:
			TextReader() noexcept;
			virtual ~TextReader() noexcept;

			virtual int read() except = 0;
			virtual void read(char *str, std::int32_t begin, std::int32_t count) except = 0;
			virtual std::string readLine() except = 0;
			virtual std::string readToEnd() except = 0;

			TextReader& operator>>(std::string& v);
			TextReader& operator>>(int& v);
		private:
			TextReader & operator=(const TextReader&) = delete;
			TextReader(const TextReader&) = delete;
		protected:
			std::string new_line;
		};
	}
}

#endif