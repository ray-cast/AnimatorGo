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
#include <stdint.h>
#include <map>
#include <vector>

#include "CLWProgram.h"
#include "CLWContext.h"
#include "cl_program.h"


namespace Baikal
{
    class CLProgramManager
    {
    public:
        // Constructor
        explicit CLProgramManager(const std::string &cache_path);
        // Creates program from file and returns its id
        uint32_t CreateProgramFromFile(CLWContext context, const std::string &fname) const;
        // Creates program from source and returns its id
        uint32_t CreateProgramFromSource(CLWContext context, const std::string &name, const std::string &source) const;
        // Loads header from file into map of headers
        void LoadHeader(const std::string &header) const;
        // Adds header to map from source
        void AddHeader(const std::string &header, const std::string &source) const;
        // Reads header from disk and returns its source
        const std::string& ReadHeader(const std::string &header) const;
        // Returns compiled program
        CLWProgram GetProgram(uint32_t id, const std::string &opts) const;
        // Compiles program
        void CompileProgram(uint32_t id, const std::string &opts) const;

    private:
        mutable std::string m_cache_path; ///< Path to cache folder
        mutable std::map<uint32_t, CLProgram> m_programs; ///< Cache of programs by id
        mutable std::map<std::string, std::string> m_headers; ///< Headers map
        static uint32_t m_next_program_id;
    };
}
