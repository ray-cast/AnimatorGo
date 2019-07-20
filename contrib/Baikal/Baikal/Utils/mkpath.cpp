#include "Utils/mkpath.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include <errno.h>

namespace Baikal
{
    static const std::string delimiters = "/\\";

    int mkpath(const std::string& path)
    {
        int err = -1;

        size_t length = path.size();
        if (length == 0)
            return 0;

        if (path.find_last_of(delimiters) == length - 1)
            --length;

        size_t idx = 0;
        while (idx < length)
        {
            idx = path.find_first_of(delimiters, idx + 1);
            auto p = path.substr(0, idx);

            // Only store the error of the creation to deepest level
            // The error of higher level can be ignored because
            // they can either be existed or a root path or something like . and ..
#ifdef _WIN32
            err = _mkdir(p.c_str());
#else
            err = mkdir(p.c_str(), 0755);
#endif
        }

        // ignore error if final directory existed
        if (err != 0 && errno == EEXIST)
            err = 0;

        return err;
    }

    int mkfilepath(const std::string& filepath)
    {
        size_t pos = filepath.find_last_of(delimiters);
        if (pos == std::string::npos)
            return 0;

        return mkpath(filepath.substr(0, pos));
    }
}
