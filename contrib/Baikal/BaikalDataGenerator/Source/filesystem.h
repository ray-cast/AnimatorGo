#pragma once

#if (defined(__GNUC__) && (__GNUC__ < 8))
#include <experimental/filesystem>
#else
#include <filesystem>
#endif

// Visual Studio 2015 and GCC 7 work-around ...
// std::filesystem was incorporated into C++-17 (which is obviously after VS
// 2015 was released). However, Microsoft implemented the draft standard in
// the std::exerimental namespace. To avoid nasty ripple effects when the
// compiler is updated, make it look like the standard here
#if (defined(_MSC_VER) && (_MSC_VER < 1900)) || (defined(__GNUC__) && (__GNUC__ < 8))
namespace std
{
    namespace filesystem = experimental::filesystem::v1;
}
#endif