#include "clw_class.h"

namespace octoon::video
{
    ClwClass::ClwClass(CLWContext context, const CLProgramManager *program_manager, std::string const& cl_file, std::string const& opts)
        : context_(context)
        , programManager_(program_manager)
    {
        auto options = opts;
        this->addCommonOptions(options);

        programId_ = programManager_->CreateProgramFromFile(context, cl_file);
    }

    CLWContext
    ClwClass::getContext() const
    {
        return context_;
    }

    CLWKernel
    ClwClass::getKernel(std::string const& name, std::string const& opts)
    {
        std::string options = opts.empty() ? defaultOpts_ : opts;
        this->addCommonOptions(options);
        return programManager_->GetProgram(programId_, options).GetKernel(name);
    }

    void
    ClwClass::addCommonOptions(std::string& opts) const
    {
        opts.append(" -cl-mad-enable -cl-fast-relaxed-math "
            "-cl-std=CL1.2 -I . ");

        opts.append(
#if defined(__APPLE__)
            "-D APPLE "
#elif defined(_WIN32) || defined (WIN32)
            "-D WIN32 "
#elif defined(__linux__)
            "-D __linux__ "
#else
            ""
#endif
        );
    }

    void
    ClwClass::setDefaultBuildOptions(std::string const& opts)
    {
        defaultOpts_ = opts;
    }

    const std::string&
    ClwClass::getDefaultBuildOpts() const
    {
        return defaultOpts_; 
    }

    std::string
    ClwClass::getFullBuildOpts() const
    {
        auto options = defaultOpts_;
        this->addCommonOptions(options);
        return options;
    }
}
