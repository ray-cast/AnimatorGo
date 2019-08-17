#ifndef OCTOON_RENDER_COMPONENT_H_
#define OCTOON_RENDER_COMPONENT_H_

#include <octoon/model/material.h>
#include <octoon/video/render_object.h>
#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT RenderComponent : public GameComponent, public video::RenderListener
	{
		OctoonDeclareSubInterface(RenderComponent, GameComponent)
	public:
		RenderComponent() noexcept;
		explicit RenderComponent(model::Materials&& material, bool sharedMaterial = false) noexcept;
		explicit RenderComponent(model::MaterialPtr&& material, bool sharedMaterial = false) noexcept;
		explicit RenderComponent(const model::Materials& material, bool sharedMaterial = false) noexcept;
		explicit RenderComponent(const model::MaterialPtr& material, bool sharedMaterial = false) noexcept;
		virtual ~RenderComponent() noexcept;

		void setMaterial(model::MaterialPtr&& material, std::size_t n = 0, bool sharedMaterial = false) noexcept;
		void setMaterial(const model::MaterialPtr& material, std::size_t n = 0, bool sharedMaterial = false) noexcept;
		const model::MaterialPtr& getMaterial(std::size_t n = 0) const noexcept;

		void setMaterials(model::Materials&& material, bool sharedMaterial = false) noexcept;
		void setMaterials(const model::Materials& material, bool sharedMaterial = false) noexcept;
		const model::Materials& getMaterials() const noexcept;

		bool isSharedMaterial() const noexcept;
		void isSharedMaterial(bool sharedMaterial) noexcept;

		std::size_t getNumMaterials() const noexcept;

	protected:
		virtual void onPreRender(const video::Camera& camera) noexcept override;
		virtual void onPostRender(const video::Camera& camera) noexcept override;

		virtual void onMaterialReplace(const model::Materials& material) noexcept;

	private:
		RenderComponent(const RenderComponent&) = delete;
		RenderComponent& operator=(const RenderComponent&) = delete;

	private:
		bool isSharedMaterial_;
		model::Materials materials_;
	};
}

#endif