#include "cl_program.h"
#include "cl_program_manager.h"

#include <assert.h>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>

namespace octoon::video
{
	inline std::uint32_t jenkins_one_at_a_time_hash(char const* key, size_t len)
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

	inline std::uint32_t CheckSum(const std::string& source)
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
		std::filesystem::create_directories(name);

		std::ofstream out(name, std::ios::out | std::ios::binary);
		if (out)
			out.write((char*)&data[0], data.size());
	}

	CLProgram::CLProgram(const CLProgramManager* program_manager, uint32_t id, CLWContext context, std::string_view program_name, std::string_view cache_path)
		: programManager_(program_manager)
		, programName_(program_name)
		, cachePath_(cache_path)
		, id_(id)
		, context_(context)
	{
	}

	void
	CLProgram::setSource(std::string_view source)
	{
		compiledSource_.reserve(1024 * 1024);
		programSource_ = source;
		parseSource(programSource_);
	}

	void
	CLProgram::parseSource(const std::string& source)
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

			programManager_->LoadHeader(fname);
			requiredHeaders_.insert(fname);
			std::string arr = programManager_->ReadHeader(fname);
			parseSource(arr);
		}
	}

	void
	CLProgram::buildSource(const std::string& source)
	{
		std::string::size_type offset = 0;
		std::string::size_type position = 0;
		std::string find_str("#include");
		while ((position = source.find(find_str, offset)) != std::string::npos)
		{
			// Append not-include part of source
			if (position != offset)
				compiledSource_ += source.substr(offset, position - offset - 1);

			// Get include file name
			std::string::size_type end_position = source.find(">", position);
			assert(end_position != std::string::npos);
			std::string fname = source.substr(position, end_position - position);
			position = fname.find("<");
			assert(position != std::string::npos);
			fname = fname.substr(position + 1, fname.length() - position);
			offset = end_position + 1;

			if (includedHeaders_.find(fname) == includedHeaders_.end())
			{
				includedHeaders_.insert(fname);
				// Append included file to source¡­¡­
				buildSource(programManager_->ReadHeader(fname));
			}
		}

		// Append rest of the file
		compiledSource_ += source.substr(offset);
	}

	CLWProgram
	CLProgram::compile(const std::string& opts)
	{
		std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
		start = std::chrono::high_resolution_clock::now();

		CLWProgram compiled_program;
		try
		{
			compiled_program = CLWProgram::CreateFromSource(compiledSource_.c_str(), compiledSource_.size(), opts.c_str(), context_);
#ifdef DUMP_PROGRAM_SOURCE
			auto e = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			std::ofstream file(m_program_name + std::to_string(e) + ".cl");
			file << m_compiled_source;
			file.close();
#endif
		}
		catch (CLWException&)
		{
			std::cerr << "Compilation failed!" << std::endl;
			std::cerr << "Dumping source to file:" << programName_ << ".cl.failed" << std::endl;
			std::string fname = programName_ + ".cl.failed";
			std::ofstream file(fname);
			file << compiledSource_;
			file.close();
			throw;
		}

		end = std::chrono::high_resolution_clock::now();
		int elapsed_ms = (int)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		std::cerr << "Program compilation time: " << elapsed_ms << " ms" << std::endl;

		isDirty_ = false;
		return compiled_program;
	}

	bool
	CLProgram::isHeaderNeeded(const std::string& header_name) const
	{
		return (requiredHeaders_.find(header_name) != requiredHeaders_.end());
	}

	CLWProgram
	CLProgram::getCLWProgram(const std::string& opts)
	{
		if (isDirty_)
		{
			programs_.clear();
			compiledSource_.clear();
			includedHeaders_.clear();
			buildSource(programSource_);
		}

		auto it = programs_.find(opts);
		if (it != programs_.end())
		{
			return it->second;
		}

		CLWProgram result;

		if (!cachePath_.empty())
		{
			std::string filename = getFilenameHash(opts);

			auto cached_program_path = cachePath_;
			cached_program_path.append("/");
			cached_program_path.append(filename);
			cached_program_path.append(".bin");

			std::vector<std::uint8_t> binary;
			if (LoadBinaries(cached_program_path, binary))
			{
				// Create from binary
				std::size_t size = binary.size();
				auto binaries = &binary[0];
				result = CLWProgram::CreateFromBinary(&binaries, &size, context_);
				programs_[opts] = result;
			}
			else
			{
				result = compile(opts);
				programs_[opts] = result;

				// Save binaries
				result.GetBinaries(0, binary);
				SaveBinaries(cached_program_path, binary);
			}
		}

		isDirty_ = false;
		return result;
	}

	std::string
	CLProgram::getFilenameHash(std::string const& opts) const
	{
		auto name = programName_;
		auto device_name = context_.GetDevice(0).GetName();

		std::regex forbidden("(\\\\)|[\\./:<>\\\"\\|\\?\\*]");

		device_name = std::regex_replace(device_name, forbidden, "_");
		device_name.erase(std::remove_if(device_name.begin(), device_name.end(), isspace), device_name.end());

		name.append("_");
		name.append(device_name);

		auto extra = context_.GetDevice(0).GetVersion();
		extra.append(opts);

		std::ostringstream oss;
		oss << jenkins_one_at_a_time_hash(extra.c_str(), extra.size());

		name.append("_");
		name.append(oss.str());


		std::uint32_t file_hash = CheckSum(compiledSource_);

		name.append("_");
		name.append(std::to_string(file_hash));

		return name;
	}
}