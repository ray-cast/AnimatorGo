#ifndef OCTOON_VIDEO_CLW_CLASS_H_
#define OCTOON_VIDEO_CLW_CLASS_H_

#include <string>
#include <numeric>
#include <vector>
#include <fstream>
#include <regex>
#include <sstream>
#include <unordered_map>

#include "CLW.h"
#include "cl_program_manager.h"

namespace octoon::video
{
    class ClwClass
    {
    public:
        ClwClass(CLWContext context, const CLProgramManager *program_manager, std::string const& cl_file, std::string const& opts = "");
        virtual ~ClwClass() = default;

        CLWContext getContext() const;
        CLWKernel getKernel(std::string const& name, std::string const& opts = "");

        void setDefaultBuildOptions(std::string const& opts);
        const std::string& getDefaultBuildOpts() const;

        std::string getFullBuildOpts() const;

    private:
        void addCommonOptions(std::string& opts) const;

        CLWContext context_;
        const CLProgramManager *programManager_;
        std::uint32_t programId_;
        std::string defaultOpts_;
    };
}

#endif