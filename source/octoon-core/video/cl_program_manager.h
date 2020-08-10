#include <string>
#include <stdint.h>
#include <map>
#include <vector>

#include "CLWProgram.h"
#include "CLWContext.h"
#include "cl_program.h"

namespace octoon::video
{
    class CLProgramManager
    {
    public:
        explicit CLProgramManager(const std::string &cache_path);
        uint32_t CreateProgramFromFile(CLWContext context, const std::string &fname) const;
        uint32_t CreateProgramFromSource(CLWContext context, const std::string &name, const std::string &source) const;
        void LoadHeader(const std::string &header) const;
        void AddHeader(const std::string &header, const std::string &source) const;
        const std::string& ReadHeader(const std::string &header) const;
        CLWProgram GetProgram(uint32_t id, const std::string &opts) const;
        void CompileProgram(uint32_t id, const std::string &opts) const;

    private:
        mutable std::string m_cache_path;
        mutable std::map<uint32_t, CLProgram> m_programs;
        mutable std::map<std::string, std::string> m_headers;
        static uint32_t m_next_program_id;
    };
}
