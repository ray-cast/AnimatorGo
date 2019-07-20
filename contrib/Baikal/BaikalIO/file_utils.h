#include <string>

namespace Baikal
{
    bool FindFilenameFromCaseInsensitive(std::string const& req_filename, std::string & actual_filename, bool resolve_symlinks = true);
}
