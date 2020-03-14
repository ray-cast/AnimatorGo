#include <octoon/video/material.h>
#include <octoon/hal/graphics.h>

namespace octoon
{
	namespace video
	{
		HALMaterial::HALMaterial() noexcept
		{
		}

		HALMaterial::HALMaterial(std::string_view name) noexcept
		{
			this->setName(name);
		}

		HALMaterial::~HALMaterial() noexcept
		{
		}

		void
		HALMaterial::setName(std::string_view name) noexcept
		{
			_name = name;
		}

		const std::string&
		HALMaterial::getName() const noexcept
		{
			return _name;
		}

		hal::GraphicsUniformSetPtr 
		HALMaterial::at(std::string_view name) const
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