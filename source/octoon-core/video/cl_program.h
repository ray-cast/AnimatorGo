#ifndef OCTOON_VIDEO_CL_PROGRAM_H_
#define OCTOON_VIDEO_CL_PROGRAM_H_

#include <CLWProgram.h>
#include <CLWContext.h>

#include <set>
#include <unordered_set>
#include <unordered_map>

namespace octoon::video
{
    class CLProgramManager;
    class CLProgram
    {
    public:
        CLProgram() = default;
        CLProgram(const CLProgramManager *program_manager, uint32_t id, CLWContext context, const std::string &program_name, const std::string &cache_path);

        bool IsDirty() const { return m_is_dirty; }
        void SetDirty() { m_is_dirty = true; }

        std::uint32_t GetId() const { return m_id; }

        void SetSource(const std::string &source);

        bool IsHeaderNeeded(const std::string& header_name) const;

        CLWProgram Compile(const std::string& opts);
        CLWProgram GetCLWProgram(const std::string &opts);

    private:
        void ParseSource(const std::string &source);
        void BuildSource(const std::string &source);

        std::string GetFilenameHash(std::string const& opts) const;

        const CLProgramManager* m_program_manager;

        std::string m_program_name;
        std::string m_cache_path;
        std::string m_compiled_source;
        std::string m_program_source;
        std::unordered_set<std::string> m_required_headers;
        std::unordered_map<std::string, CLWProgram> m_programs;

        bool m_is_dirty = true;
        uint32_t m_id;
        CLWContext m_context;
        std::set<std::string> m_included_headers;
    };
}

#endif