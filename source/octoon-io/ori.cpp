#include <octoon/io/ori.h>

namespace octoon
{
	namespace io
	{
		std::string sanitize_path(const std::string& path)
		{
		  auto separator =
		#ifdef OCTOON_BUILD_PLATFORM_WINDOWS
			"/\\";
		#else
			'/';
		#endif
		  std::string rv;
		  size_t next = path.find_first_of(separator, 0);
		  if (next == std::string::npos) {
			rv.append(path.begin(), path.end());

			// Only one segment, return at this point.
			return rv;
		  }
		  size_t beg;

		  // Multiple segments.
		  rv.append(path.begin(), path.begin() + next);
		  while (beg = next + 1, // Search from the next char to '/'.
			(next = path.find_first_of(separator, beg)) != std::string::npos) {
			switch (next - beg) {
			 case 0: // Ignore Empty segment.
			  continue;
			 case 1: // Ignore `.` segment.
			  if (path[beg] == '.') {
				continue;
			  }
			  break;
			 case 2: // Met `..`, trace back to parent.
			  if (path[beg] == '.' && path[beg + 1] == '.') {
				rv.resize(rv.find_last_of("/", rv.size()));
				continue;
			  }
			  break;
			}
			rv.push_back('/');
			rv.append(path.begin() + beg, path.begin() + next);
		  }
		  rv.push_back('/');
		  rv.append(path.begin() + beg, path.end());
		  return rv;
		}

		Orl::Orl(const std::string& vdir, const std::string& path)
			: vdir_(vdir)
			, path_(path)
		{
		}

		bool
		Orl::is_valid() const
		{
			return vdir_.size() && path_.front() != '/';
		}

		const std::string&
		Orl::virtual_dir() const
		{
			return vdir_;
		}

		const std::string&
		Orl::path() const
		{
			return path_;
		}

		bool
		Orl::parse(const std::string& orl, Orl& out)
		{
			size_t vdir_size = orl.find(':');
			if (vdir_size == std::string::npos || vdir_size == 0) {
				return false;
			}

			auto vdir = orl.substr(0, vdir_size);
			size_t path_size = orl.size() - vdir_size - 1;
			auto path = sanitize_path(orl.substr(vdir_size + 1, path_size));

			if (path.size() > 0 && path.front() == '/') {
			return false;
			}

			out = Orl(vdir, path);
			return true;
		}

		std::string
		Orl::to_string() const
		{
			std::string out;
			out.append(vdir_);
			out.push_back(':');
			out.append(path_);
			return out;
		}

		Orl
		Orl::parent() const
		{
		  auto end = path_.find_last_of("/", path_.size());
		  if (end == std::string::npos) {
			return Orl(vdir_, "");
		  } else {
			return Orl(vdir_, path_.substr(0, end));
		  }
		}
	}
}