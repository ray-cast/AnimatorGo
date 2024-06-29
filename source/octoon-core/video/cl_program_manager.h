#ifndef OCTOON_CL_PROGRAM_MANAGER_H_
#define OCTOON_CL_PROGRAM_MANAGER_H_

#include <string>
#include <stdint.h>
#include <map>
#include <vector>
#include <CLW.h>
#include <filesystem>

#include "cl_program.h"

namespace octoon
{
    class CLProgramManager final
    {
    public:
        explicit CLProgramManager(const std::filesystem::path& cache_path);

        std::uint32_t CreateProgramFromFile(CLWContext context, std::string_view fname) noexcept(false);
        std::uint32_t CreateProgramFromSource(CLWContext context, std::string_view name, std::string_view source) noexcept(false);

        void LoadHeader(const std::string &header) noexcept(false);
        void AddHeader(const std::string &header, const std::string &source) noexcept(false);
        const std::string& ReadHeader(const std::string &header) const;

        CLWProgram GetProgram(std::uint32_t id, const std::string &opts) noexcept(false);
        void CompileProgram(std::uint32_t id, const std::string &opts) noexcept(false);

    private:
        static std::uint32_t nextProgramId_;

        std::filesystem::path cachePath_;
        std::map<uint32_t, CLProgram> programs_;
        std::map<std::string, std::string> headers_;
    };
}

#endif