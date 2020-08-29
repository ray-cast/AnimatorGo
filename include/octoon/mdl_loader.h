#ifndef OCTOON_MDL_LOADER_H_
#define OCTOON_MDL_LOADER_H_

#include <octoon/io/iostream.h>
#include <octoon/material/material.h>

#include <set>

namespace octoon
{
	class MDLContext;
	class OCTOON_EXPORT MDLLoader final
	{
	public:
		MDLLoader() noexcept(false);
		~MDLLoader() noexcept;

		static bool doCanRead(io::istream& stream) noexcept;
		static bool doCanRead(const char* type) noexcept;

		void addModulePath(std::string_view modulePath);

		std::unique_ptr<material::Material> load(io::istream& stream, std::string_view material) noexcept(false);
		std::unique_ptr<material::Material> load(std::string_view material, std::string_view moduleName, std::string_view source) noexcept(false);
		void save(io::ostream& stream, const material::Material& material) noexcept(false);

	private:
		std::string preprocessSource(std::string_view source);

	private:
		MDLLoader(const MDLLoader&) = delete;
		MDLLoader& operator=(const MDLLoader&) = delete;

	private:
		bool verboseLogging_;
		std::set<std::string> modulePaths_;
		std::unique_ptr<MDLContext> context_;
	};
}

#endif