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
		RenderComponent(model::MaterialPtr&& material, bool sharedMaterial = false) noexcept;
		RenderComponent(const model::MaterialPtr& material, bool sharedMaterial = false) noexcept;
		virtual ~RenderComponent() noexcept;

		void setMaterial(model::MaterialPtr&& material, bool sharedMaterial = false) noexcept;
		void setMaterial(const model::MaterialPtr& material, bool sharedMaterial = false) noexcept;
		const model::MaterialPtr& getMaterial() const noexcept;

		bool isSharedMaterial() const noexcept;
		void isSharedMaterial(bool sharedMaterial) noexcept;

	protected:
		virtual void onPreRender(const video::Camera& camera) noexcept override;
		virtual void onPostRender(const video::Camera& camera) noexcept override;

		virtual void onMaterialReplace(const model::MaterialPtr& material) noexcept;

	private:
		RenderComponent(const RenderComponent&) = delete;
		RenderComponent& operator=(const RenderComponent&) = delete;

	private:
		bool isSharedMaterial_;
		model::MaterialPtr material_;
	};
}

#endif