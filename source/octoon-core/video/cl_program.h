#ifndef OCTOON_CL_PROGRAM_H_
#define OCTOON_CL_PROGRAM_H_

#include <CLWProgram.h>
#include <CLWContext.h>

#include <set>
#include <filesystem>
#include <unordered_set>
#include <unordered_map>

namespace octoon
{
    class CLProgramManager;
    class CLProgram final
    {
    public:
        CLProgram() = default;
        CLProgram(CLProgramManager *program_manager, uint32_t id, CLWContext context, std::string_view program_name, const std::filesystem::path& cache_path);

        bool isDirty() const { return isDirty_; }
        void setDirty() { isDirty_ = true; }

        std::uint32_t getId() const { return id_; }

        void setSource(std::string_view source);

        bool isHeaderNeeded(const std::string& header_name) const;

        CLWProgram compile(const std::string& opts);
        CLWProgram getCLWProgram(const std::string &opts);

    private:
        void parseSource(const std::string &source);
        void buildSource(const std::string &source);

        std::string getFilenameHash(std::string const& opts) const;

    private:
        CLProgramManager* programManager_;

        std::string programName_;
        std::string compiledSource_;
        std::string programSource_;
        std::filesystem::path cachePath_;
        std::unordered_set<std::string> requiredHeaders_;
        std::unordered_map<std::string, CLWProgram> programs_;

        bool isDirty_ = true;
        uint32_t id_;
        CLWContext context_;
        std::set<std::string> includedHeaders_;
    };
}

#endif