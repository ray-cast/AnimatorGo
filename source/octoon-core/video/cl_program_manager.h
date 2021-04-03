#ifndef OCTOON_VIDEO_CL_PROGRAM_MANAGER_H_
#define OCTOON_VIDEO_CL_PROGRAM_MANAGER_H_

#include <string>
#include <stdint.h>
#include <map>
#include <vector>
#include <CLW.h>

#include "cl_program.h"

namespace octoon
{
    class CLProgramManager final
    {
    public:
        explicit CLProgramManager(std::string_view cache_path);

        std::uint32_t CreateProgramFromFile(CLWContext context, std::string_view fname) const;
        std::uint32_t CreateProgramFromSource(CLWContext context, std::string_view name, std::string_view source) const;

        void LoadHeader(const std::string &header) const;
        void AddHeader(const std::string &header, const std::string &source) const;
        const std::string& ReadHeader(const std::string &header) const;

        CLWProgram GetProgram(std::uint32_t id, const std::string &opts) const;
        void CompileProgram(std::uint32_t id, const std::string &opts) const;

    private:
        mutable std::string cachePath_;
        mutable std::map<uint32_t, CLProgram> programs_;
        mutable std::map<std::string, std::string> headers_;
        static std::uint32_t nextProgramId_;
    };
}

#endif