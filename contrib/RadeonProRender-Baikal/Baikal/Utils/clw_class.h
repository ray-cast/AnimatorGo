#pragma once

#include <string>
#include <numeric>
#include <vector>
#include <fstream>
#include <regex>
#include <sstream>
#include <unordered_map>

#include "CLW.h"
#include "version.h"
#include "cl_program_manager.h"

namespace Baikal
{
    class ClwClass
    {
    public:
#ifdef BAIKAL_EMBED_KERNELS
        //create from source
        ClwClass(CLWContext context,
            const CLProgramManager *program_manager,
            std::string const& name,
            std::string const& source,
            std::unordered_map<char const*, char const*> const& headers,
            std::string const& opts = "");
#else
        //create from file
        ClwClass(CLWContext context,
            const CLProgramManager *program_manager,
            std::string const& cl_file,
            std::string const& opts = "");
#endif

        virtual ~ClwClass() = default;

        CLWContext GetContext() const { return m_context; }
        CLWKernel GetKernel(std::string const& name, std::string const& opts = "");
        void SetDefaultBuildOptions(std::string const& opts);
        std::string GetDefaultBuildOpts() const { return m_default_opts; }
        std::string GetFullBuildOpts() const;

    private:
        void AddCommonOptions(std::string& opts) const;

        // Context to build programs for
        CLWContext m_context;
        // Program manager pointer
        const CLProgramManager *m_program_manager;
        // Program id
        uint32_t m_program_id;
        // Default build options
        std::string m_default_opts;
    };

#ifdef BAIKAL_EMBED_KERNELS
    inline ClwClass::ClwClass(
        CLWContext context,
        const CLProgramManager *program_manager,
        std::string const& name,
        std::string const& source,
        std::unordered_map<char const*, char const*> const& headers,
        std::string const& opts)
        : m_context(context)
        , m_program_manager(program_manager)
    {
        auto options = opts;
        AddCommonOptions(options);

        m_program_id = m_program_manager->CreateProgramFromSource(context, name, source);
        for (auto const& header : headers)
        {
            m_program_manager->AddHeader(header.first, header.second);
        }
    }
#else
    inline ClwClass::ClwClass(
        CLWContext context,
        const CLProgramManager *program_manager,
        std::string const& cl_file,
        std::string const& opts)
        : m_context(context)
        , m_program_manager(program_manager)
    {
        auto options = opts;
        AddCommonOptions(options);

        m_program_id = m_program_manager->CreateProgramFromFile(context, cl_file);
    }
#endif

    inline CLWKernel ClwClass::GetKernel(std::string const& name, std::string const& opts)
    {
        std::string options = opts.empty() ? m_default_opts : opts;
        AddCommonOptions(options);
        return m_program_manager->GetProgram(m_program_id, options).GetKernel(name);
    }


    inline void ClwClass::AddCommonOptions(std::string& opts) const
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

    inline std::string ClwClass::GetFullBuildOpts() const
    {
        auto options = m_default_opts;
        AddCommonOptions(options);
        return options;
    }

    inline void ClwClass::SetDefaultBuildOptions(std::string const& opts)
    {
        m_default_opts = opts;
    }
}
