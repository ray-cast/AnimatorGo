#include <octoon/video/material.h>

namespace octoon
{
	namespace video
	{
		Material::Material() noexcept
		{
		}

		Material::~Material() noexcept
		{
		}

		MaterialPtr
		Material::clone() const noexcept
		{
			return std::make_shared<Material>();
		}
	}
}