#include <octoon/video/render_pipeline.h>
#include <octoon/hal/graphics.h>

namespace octoon
{
	namespace video
	{
		RenderPipeline::RenderPipeline() noexcept
		{
		}

		RenderPipeline::RenderPipeline(std::string_view name) noexcept
		{
			this->setName(name);
		}

		RenderPipeline::~RenderPipeline() noexcept
		{
		}

		void
		RenderPipeline::setName(std::string_view name) noexcept
		{
			name_ = name;
		}

		const std::string&
		RenderPipeline::getName() const noexcept
		{
			return name_;
		}

		void
		RenderPipeline::setMaterial(const material::MaterialPtr& material) noexcept
		{
			if (this->material_ != material)
			{
				this->material_ = material;
				this->onMaterialReplace(this->material_);
			}
		}
		
		const material::MaterialPtr&
		RenderPipeline::getMaterial() const noexcept
		{
			return this->material_;
		}

		hal::GraphicsUniformSetPtr 
		RenderPipeline::at(std::string_view name) const
		{
			auto begin = this->getDescriptorSet()->getUniformSets().begin();
			auto end = this->getDescriptorSet()->getUniformSets().end();

			auto it = std::find_if(begin, end, [&](const hal::GraphicsUniformSetPtr& set){ return set->getName() == name; });
			if (it != end)
				return *it;

			return nullptr;
		}

		void
		RenderPipeline::onMaterialReplace(const material::MaterialPtr& material) noexcept(false)
		{
		}
	}
}