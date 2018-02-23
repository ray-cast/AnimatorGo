// File: IoServer.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_ORI_H_
#define OCTOON_IO_ORI_H_

#include <string>

#include <octoon/runtime/platform.h>

namespace octoon
{
	namespace io
	{
		/*
		* ORL -> Octoon Resource Locator.
		*
		* ORLs are composited by two parts: virtual directory entry and path to file in
		* that virtual directory, separated by a colon (`:`). For example:
		* `app-data:images/background12.png` represents file `images/background12.png`
		* in virtual directory `app-data`.
		*
		* ORL's path part must be a relative one. A absolute path will be rejected when
		* parsing.
		*/
		struct OCTOON_EXPORT Orl
		{
		public:
			Orl() = default;
			Orl(const Orl& lv) = default;
			Orl(Orl&& rv) = default;
			Orl(const std::string& vdir, const std::string& path);

			Orl& operator=(const Orl& lv) = default;

			bool is_valid() const;
			/*
			* Parse a ORL string into a perfectly valid ORL object.
			*
			* Paths will be sanitized to protect from path traversal attack. So it's
			* recommended to use `parse()` to create ORL objects.
			*
			* Returns:
			*   True if the string is a valid, parsable ORL; otherwise, false.
			*/
			static bool parse(const std::string& orl, Orl& out);

			const std::string& virtual_dir() const;
			const std::string& path() const;

			std::string to_string() const;

			Orl parent() const;

		private:
			std::string vdir_;
			std::string path_;
		};
	}
}

#endif