#ifndef OCTOON_JSONREADER_H_
#define OCTOON_JSONREADER_H_

#include <octoon/io/iarchive.h>
#include <octoon/io/oarchive.h>
#include <octoon/io/iostream.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT JsonReader final : public iarchive
		{
		public:
			JsonReader() noexcept;
			JsonReader(istream& stream) except;
			JsonReader(const std::string& path) except;
			~JsonReader() noexcept;

			JsonReader& open(istream& stream, const ios_base::open_mode mode = ios_base::in) except;
			JsonReader& open(const std::string& path) except;

			bool is_open() const noexcept;

			void close() noexcept;

		private:
			JsonReader(const JsonReader&) noexcept = delete;
			JsonReader& operator=(const JsonReader&) noexcept = delete;

		private:
			archivebuf _json;
		};

		class OCTOON_EXPORT JsonWrite final : public oarchive
		{
		public:
			JsonWrite() noexcept;
			~JsonWrite() noexcept;

			JsonWrite& save(ostream& stream, const ios_base::open_mode mode = ios_base::out) except;
			JsonWrite& save(const std::string& path) except;

			void close() noexcept;

			bool is_open() const noexcept;

		private:
			JsonWrite(const JsonWrite&) noexcept = delete;
			JsonWrite& operator=(const JsonWrite&) noexcept = delete;

		private:
			archivebuf _json;
		};
	}
}

#endif