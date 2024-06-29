#include "cl_program_manager.h"

#include <fstream>
#include <regex>

namespace octoon
{
	std::uint32_t CLProgramManager::nextProgramId_ = 0;

	std::string ReadFile(const std::string& path)
	{
		std::ifstream file(path);
		if (file)
		{
			std::string str;

			file.seekg(0, std::ios::end);
			str.reserve(file.tellg());
			file.seekg(0, std::ios::beg);

			str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			return str;
		}
		else
		{
			return "";
		}
	}

	CLProgramManager::CLProgramManager(const std::filesystem::path& cache_path)
		: cachePath_(cache_path)
	{
	}

	std::uint32_t
	CLProgramManager::CreateProgramFromFile(CLWContext context, std::string_view filepath) noexcept(false)
	{
		std::regex delimiter("\\\\");

		auto path = std::string(filepath);
		auto fullpath = std::regex_replace(path, delimiter, "/");
		auto filename_start = fullpath.find_last_of('/');

		if (filename_start == std::string::npos)
			filename_start = 0;
		else
			filename_start += 1;

		auto filename_end = fullpath.find_last_of('.');

		if (filename_end == std::string::npos)
			filename_end = fullpath.size();

		auto name = fullpath.substr(filename_start, filename_end - filename_start);

		return CLProgramManager::CreateProgramFromSource(context, name, ReadFile(path));
	}

	std::uint32_t
	CLProgramManager::CreateProgramFromSource(CLWContext context, std::string_view name, std::string_view source) noexcept(false)
	{
		CLProgram prg(this, nextProgramId_++, context, name, cachePath_);
		prg.setSource(source);
		programs_.insert(std::make_pair(prg.getId(), prg));
		return prg.getId();
	}

	void
	CLProgramManager::AddHeader(const std::string& header, const std::string& source) noexcept(false)
	{
		std::string currect_header_code = headers_[header];
		if (currect_header_code != source)
		{
			headers_[header] = source;

			for (auto& program : programs_)
			{
				if (program.second.isHeaderNeeded(header))
				{
					program.second.setDirty();
				}
			}
		}
	}

	void
	CLProgramManager::LoadHeader(const std::string& header) noexcept(false)
	{
		std::string header_source = ReadFile(header);
		AddHeader(header, header_source);
	}

	const std::string&
	CLProgramManager::ReadHeader(const std::string& header) const
	{
		return headers_.at(header);
	}

	CLWProgram
	CLProgramManager::GetProgram(uint32_t id, const std::string& opts) noexcept(false)
	{
		CLProgram& program = programs_[id];
		return program.getCLWProgram(opts);
	}

	void
	CLProgramManager::CompileProgram(uint32_t id, const std::string& opts) noexcept(false)
	{
		CLProgram& program = programs_[id];
		program.compile(opts);
	}
}