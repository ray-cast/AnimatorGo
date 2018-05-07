#ifndef OCTOON_RENDERER_H_
#define OCTOON_RENDERER_H_

#include <octoon/video/material.h>
#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT RenderComponent : public GameComponent
	{
		OctoonDeclareSubInterface(RenderComponent, GameComponent)
	public:
		RenderComponent() noexcept;
		RenderComponent(video::MaterialPtr&& material, bool sharedMesh = false) noexcept;
		RenderComponent(const video::MaterialPtr& material, bool sharedMesh = false) noexcept;
		virtual ~RenderComponent() noexcept;

		void setMaterial(video::MaterialPtr&& material, bool sharedMesh = false) noexcept;
		void setMaterial(const video::MaterialPtr& material, bool sharedMesh = false) noexcept;
		const video::MaterialPtr& getMaterial() const noexcept;

		bool isSharedMaterial() const noexcept;

	protected:
		virtual void onMaterialReplace(const video::MaterialPtr& material) noexcept;

	private:
		RenderComponent(const RenderComponent&) = delete;
		RenderComponent& operator=(const RenderComponent&) = delete;

	private:
		bool isSharedMaterial_;
		video::MaterialPtr material_;
	};
}

#endif