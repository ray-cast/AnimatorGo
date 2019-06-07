#include <octoon/video/material.h>
#include <octoon/hal/graphics.h>

namespace octoon
{
	namespace video
	{
		Material::Material() noexcept
		{
		}

		Material::Material(std::string&& name) noexcept
		{
			this->setName(std::move(name));
		}

		Material::Material(const std::string& name) noexcept
		{
			this->setName(name);
		}

		Material::~Material() noexcept
		{
		}

		void
		Material::setName(std::string&& name) noexcept
		{
			_name = std::move(name);
		}

		void
		Material::setName(const std::string& name) noexcept
		{
			_name = name;
		}

		const std::string&
		Material::getName() const noexcept
		{
			return _name;
		}

		hal::GraphicsUniformSetPtr 
		Material::at(const std::string& name) const
		{
			auto begin = this->getDescriptorSet()->getUniformSets().begin();
			auto end = this->getDescriptorSet()->getUniformSets().end();

			auto it = std::find_if(begin, end, [&](const hal::GraphicsUniformSetPtr& set){ return set->getName() == name; });
			if (it != end)
				return *it;

			return nullptr;
		}		
	}
}