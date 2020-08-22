#include <octoon/mdl_loader.h>

namespace octoon
{
	MDLLoader::MDLLoader() noexcept
	{
	}

	MDLLoader::~MDLLoader() noexcept
	{
	}

	bool
	MDLLoader::doCanRead(io::istream& stream) noexcept
	{
		return false;
	}

	bool
	MDLLoader::doCanRead(const char* type) noexcept
	{
		return false;
	}

	std::unique_ptr<material::Material>
	MDLLoader::load(io::istream& stream) noexcept(false)
	{
		return nullptr;
	}

	void
	MDLLoader::save(io::ostream& stream, const material::Material& animation) noexcept(false)
	{
	}
}