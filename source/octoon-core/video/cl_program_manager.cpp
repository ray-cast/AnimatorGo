#include "cl_program_manager.h"

#include <fstream>
#include <regex>

namespace octoon::video
{
	std::uint32_t CLProgramManager::m_next_program_id = 0;

	std::string ReadFile(const std::string& fname)
	{
		std::ifstream file(fname);
		if (!file)
		{
			return "";
		}
		std::string str;

		file.seekg(0, std::ios::end);
		str.reserve(file.tellg());
		file.seekg(0, std::ios::beg);

		str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		return str;
	}

	CLProgramManager::CLProgramManager(const std::string& cache_path)
		: m_cache_path(cache_path)
	{
	}

	std::uint32_t
	CLProgramManager::CreateProgramFromFile(CLWContext context, const std::string& fname) const
	{
		std::regex delimiter("\\\\");
		auto fullpath = std::regex_replace(fname, delimiter, "/");

		auto filename_start = fullpath.find_last_of('/');

		if (filename_start == std::string::npos)
			filename_start = 0;
		else
			filename_start += 1;

		auto filename_end = fullpath.find_last_of('.');

		if (filename_end == std::string::npos)
			filename_end = fullpath.size();

		auto name = fullpath.substr(filename_start, filename_end - filename_start);

		return CLProgramManager::CreateProgramFromSource(context, name, ReadFile(fname));
	}

	std::uint32_t
	CLProgramManager::CreateProgramFromSource(CLWContext context, const std::string& name, const std::string& source) const
	{
		CLProgram prg(this, m_next_program_id++, context, name, m_cache_path);
		prg.SetSource(source);
		m_programs.insert(std::make_pair(prg.GetId(), prg));
		return prg.GetId();
	}

	void
	CLProgramManager::AddHeader(const std::string& header, const std::string& source) const
	{
		std::string currect_header_code = m_headers[header];
		if (currect_header_code != source)
		{
			m_headers[header] = source;

			for (auto& program : m_programs)
			{
				if (program.second.IsHeaderNeeded(header))
				{
					program.second.SetDirty();
				}
			}
		}
	}

	void
	CLProgramManager::LoadHeader(const std::string& header) const
	{
		std::string header_source = ReadFile(header);
		AddHeader(header, header_source);
	}

	const std::string&
	CLProgramManager::ReadHeader(const std::string& header) const
	{
		return m_headers[header];
	}

	CLWProgram
	CLProgramManager::GetProgram(uint32_t id, const std::string& opts) const
	{
		CLProgram& program = m_programs[id];
		return program.GetCLWProgram(opts);
	}

	void
	CLProgramManager::CompileProgram(uint32_t id, const std::string& opts) const
	{
		CLProgram& program = m_programs[id];
		program.Compile(opts);
	}
}