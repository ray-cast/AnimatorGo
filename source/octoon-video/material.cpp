#include <octoon/video/material.h>
#include <octoon/graphics/graphics.h>

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

		graphics::GraphicsUniformSetPtr 
		Material::getParameter(const std::string& name) const
		{
			auto begin = this->getDescriptorSet()->getUniformSets().begin();
			auto end = this->getDescriptorSet()->getUniformSets().end();

			auto it = std::find_if(begin, end, [&](const graphics::GraphicsUniformSetPtr& set){ return set->getName() == name; });
			if (it != end)
				return *it;

			return nullptr;
		}		
	}
}