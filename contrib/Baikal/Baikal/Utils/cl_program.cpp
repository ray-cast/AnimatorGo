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

#include "cl_program.h"

#include <assert.h>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <regex>

#include "cl_program_manager.h"
#include "version.h"
#include "Utils/mkpath.h"

//#define DUMP_PROGRAM_SOURCE 1

using namespace Baikal;

inline std::uint32_t jenkins_one_at_a_time_hash(char const *key, size_t len)
{
    std::uint32_t hash, i;
    for (hash = i = 0; i < len; ++i)
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

// Computting check sum algo
// Copy from here: https://codereview.stackexchange.com/questions/104948/32-bit-checksum-of-a-file
inline std::uint32_t CheckSum(const std::string &source)
{
    std::uint32_t check_sum = 0;
    unsigned shift = 0;
    for (uint32_t ch : source)
    {
        check_sum += (ch << shift);
        shift += 8;
        if (shift == 32) {
            shift = 0;
        }
    }
    return check_sum;
}

inline bool LoadBinaries(std::string const& name, std::vector<std::uint8_t>& data)
{
    std::ifstream in(name, std::ios::in | std::ios::binary);
    if (in)
    {
        data.clear();
        std::streamoff beg = in.tellg();
        in.seekg(0, std::ios::end);
        std::streamoff fileSize = in.tellg() - beg;
        in.seekg(0, std::ios::beg);
        data.resize(static_cast<unsigned>(fileSize));
        in.read((char*)&data[0], fileSize);
        return true;
    }
    else
    {
        return false;
    }
}

inline void SaveBinaries(std::string const& name, std::vector<std::uint8_t>& data)
{
    mkfilepath(name);

    std::ofstream out(name, std::ios::out | std::ios::binary);

    if (out)
    {
        out.write((char*)&data[0], data.size());
    }
}


CLProgram::CLProgram(const CLProgramManager *program_manager, uint32_t id, CLWContext context,
                     const std::string &program_name, const std::string &cache_path) :
    m_program_manager(program_manager),
    m_program_name(program_name),
    m_cache_path(cache_path),
    m_id(id),
    m_context(context)
{
};

void CLProgram::SetSource(const std::string &source)
{
    m_compiled_source.reserve(1024 * 1024); //Just reserve 1M for now
    m_program_source = source;
    ParseSource(m_program_source);
}

void CLProgram::ParseSource(const std::string &source)
{
    std::string::size_type offset = 0;
    std::string::size_type position = 0;
    std::string find_str("#include");
    while ((position = source.find(find_str, offset)) != std::string::npos)
    {
        std::string::size_type end_position = source.find(">", position);
        assert(end_position != std::string::npos);
        std::string fname = source.substr(position, end_position - position);
        position = fname.find("<");
        assert(position != std::string::npos);
        fname = fname.substr(position + 1, fname.length() - position);
        offset = end_position;

        m_program_manager->LoadHeader(fname);
        m_required_headers.insert(fname);
        std::string arr = m_program_manager->ReadHeader(fname);
        ParseSource(arr);
    }
}

void CLProgram::BuildSource(const std::string &source)
{
    std::string::size_type offset = 0;
    std::string::size_type position = 0;
    std::string find_str("#include");
    while ((position = source.find(find_str, offset)) != std::string::npos)
    {
        // Append not-include part of source
        if (position != offset)
            m_compiled_source += source.substr(offset, position - offset - 1);

        // Get include file name
        std::string::size_type end_position = source.find(">", position);
        assert(end_position != std::string::npos);
        std::string fname = source.substr(position, end_position - position);
        position = fname.find("<");
        assert(position != std::string::npos);
        fname = fname.substr(position + 1, fname.length() - position);
        offset = end_position + 1;

        if (m_included_headers.find(fname) == m_included_headers.end())
        {
            m_included_headers.insert(fname);
            // Append included file to source
            BuildSource(m_program_manager->ReadHeader(fname));
        }
    }

    // Append rest of the file
    m_compiled_source += source.substr(offset);
}

CLWProgram CLProgram::Compile(const std::string &opts)
{
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    start = std::chrono::high_resolution_clock::now();

    CLWProgram compiled_program;
    try
    {
        compiled_program = CLWProgram::CreateFromSource(m_compiled_source.c_str(), m_compiled_source.size(), opts.c_str(), m_context);
        /*
         * Code below usable for cache debugging
         */
#ifdef DUMP_PROGRAM_SOURCE
        auto e = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        std::ofstream file(m_program_name + std::to_string(e) + ".cl");
        file << m_compiled_source;
        file.close();
#endif
    }
    catch (CLWException& )
    {
        std::cerr << "Compilation failed!" << std::endl;
        std::cerr << "Dumping source to file:" << m_program_name << ".cl.failed" << std::endl;
        std::string fname = m_program_name + ".cl.failed";
        std::ofstream file(fname);
        file << m_compiled_source;
        file.close();
        throw;
    }

    end = std::chrono::high_resolution_clock::now();
    int elapsed_ms = (int)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cerr << "Program compilation time: " << elapsed_ms << " ms" << std::endl;

    m_is_dirty = false;
    return compiled_program;
}

bool CLProgram::IsHeaderNeeded(const std::string &header_name) const
{
    return (m_required_headers.find(header_name) != m_required_headers.end());
}

CLWProgram CLProgram::GetCLWProgram(const std::string &opts)
{
    // global dirty flag
    if (m_is_dirty)
    {
        m_programs.clear();
        m_compiled_source.clear();
        m_included_headers.clear();
        BuildSource(m_program_source);
    }

    auto it = m_programs.find(opts);
    if (it != m_programs.end())
    {
        return it->second;
    }

    CLWProgram result;
    //check if we can get it from cache
    if (!m_cache_path.empty())
    {
        std::string filename = GetFilenameHash(opts);

        auto cached_program_path = m_cache_path;
        cached_program_path.append("/");
        cached_program_path.append(filename);
        cached_program_path.append(".bin");

        std::vector<std::uint8_t> binary;
        if (LoadBinaries(cached_program_path, binary))
        {
            // Create from binary
            std::size_t size = binary.size();
            auto binaries = &binary[0];
            result = CLWProgram::CreateFromBinary(&binaries, &size, m_context);
            m_programs[opts] = result;
        }
        else
        {
            result = Compile(opts);
            m_programs[opts] = result;

            // Save binaries
            result.GetBinaries(0, binary);
            SaveBinaries(cached_program_path, binary);
        }
    }

    m_is_dirty = false;
    return result;
}

std::string CLProgram::GetFilenameHash(std::string const& opts) const
{
    auto name = m_program_name;
    auto device_name = m_context.GetDevice(0).GetName();

    std::regex forbidden("(\\\\)|[\\./:<>\\\"\\|\\?\\*]");

    device_name = std::regex_replace(device_name, forbidden, "_");
    device_name.erase(
        std::remove_if(device_name.begin(), device_name.end(), isspace),
                      device_name.end());

    name.append("_");
    name.append(device_name);

    auto extra = m_context.GetDevice(0).GetVersion();
    extra.append(opts);

    std::ostringstream oss;
    oss << jenkins_one_at_a_time_hash(extra.c_str(), extra.size());

    name.append("_");
    name.append(oss.str());


    std::uint32_t file_hash = CheckSum(m_compiled_source);

    name.append("_");
    name.append(std::to_string(file_hash));

    name.append(BAIKAL_VERSION);

    return name;
}
