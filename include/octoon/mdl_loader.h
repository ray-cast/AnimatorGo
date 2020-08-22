#ifndef OCTOON_MDL_LOADER_H_
#define OCTOON_MDL_LOADER_H_

#include <octoon/io/iostream.h>
#include <octoon/material/material.h>

namespace octoon
{
	class MDLLoader final
	{
	public:
		MDLLoader() noexcept;
		~MDLLoader() noexcept;

		static bool doCanRead(io::istream& stream) noexcept;
		static bool doCanRead(const char* type) noexcept;

		static std::unique_ptr<material::Material> load(io::istream& stream) noexcept(false);
		static void save(io::ostream& stream, const material::Material& material) noexcept(false);

	private:
		MDLLoader(const MDLLoader&) = delete;
		MDLLoader& operator=(const MDLLoader&) = delete;
	};
}

#endif