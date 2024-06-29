#ifndef OCTOON_MDL_LOADER_H_
#define OCTOON_MDL_LOADER_H_

#include <octoon/io/iostream.h>
#include <octoon/material/mesh_standard_material.h>

#include <set>

namespace octoon
{
	class MDLContext;
	class OCTOON_EXPORT MDLLoader final
	{
	public:
		MDLLoader() noexcept(false);
		~MDLLoader() noexcept;

		static bool doCanRead(std::istream& stream) noexcept;
		static bool doCanRead(const char* type) noexcept;

		void addModulePath(std::string_view modulePath);

		void load(std::string_view moduleName, std::istream& stream) noexcept(false);
		void load(std::string_view moduleName, std::string_view source = "", std::string_view target_model = "ue4") noexcept(false);

		void save(io::ostream& stream, const MeshStandardMaterial& material) noexcept(false);

		const std::vector<std::shared_ptr<MeshStandardMaterial>>& getMaterials() const noexcept;

	private:
		std::string preprocessSource(std::string_view source);

	private:
		MDLLoader(const MDLLoader&) = delete;
		MDLLoader& operator=(const MDLLoader&) = delete;

	private:
		bool verboseLogging_;
		std::set<std::string> modulePaths_;
		std::unique_ptr<MDLContext> context_;
		std::vector<std::shared_ptr<MeshStandardMaterial>> materials_;
	};
}

#endif