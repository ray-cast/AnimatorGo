/**********************************************************************
Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
********************************************************************/

#pragma once

#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include "CLWProgram.h"
#include "CLWContext.h"

namespace Baikal
{
    class CLProgramManager;
    class CLProgram
    {
    public:
        CLProgram() = default;
        // Constructs CLProgram empty object
        CLProgram(const CLProgramManager *program_manager, uint32_t id, CLWContext context, const std::string &program_name, const std::string &cache_path);
        // Check if program should be recompiled
        bool IsDirty() const { return m_is_dirty; }
        // Sets dirty flag on program
        void SetDirty() { m_is_dirty = true; }
        // Returns program id
        uint32_t GetId() const { return m_id; }
        /**
         * @brief Sets program source
         *
         * Function parses provided source code and collect set of headers that 
         * required for program to compile.
         * Each header added into CLProgramManager headers array and recursively parsed for headers.
         */
        void SetSource(const std::string &source);
        /**
         * @brief returns CLWProgram object
         * 
         * This function will build program and compile it if it's durty.
         * This function respronsible for shader cache handling. If required program
         * already exists in disk or in-memory cache returns it.
         * For caching uses simple uint32 crc of source code.
         */
        CLWProgram GetCLWProgram(const std::string &opts);

        // Checks if specified header required by program
        bool IsHeaderNeeded(const std::string &header_name) const;

        // Compiles program. In case of error dumps source into current folder
        CLWProgram Compile(const std::string &opts);

    private:
        // Parses source
        void ParseSource(const std::string &source);
        /**
         * This function builds full program source by replacing 
         * include directives with source code.
         * Duplicate includes removed.
         */
        void BuildSource(const std::string &source);
        // Returns hash for file name
        std::string GetFilenameHash(std::string const& opts) const;

        const CLProgramManager *m_program_manager;
        std::string m_program_name;    ///< Program name
        std::string m_cache_path;      ///< Cache folder path
        std::string m_compiled_source; ///< Final program source with all headers
        std::string m_program_source;  ///< Program source code without modifications
        std::unordered_set<std::string> m_required_headers; ///< Set of required headers

        std::unordered_map<std::string, CLWProgram> m_programs; ///< In-memory cache for compiled programs

        bool m_is_dirty = true;
        uint32_t m_id;
        CLWContext m_context;
        std::set<std::string> m_included_headers; ///< Set of included headers

    };
}
