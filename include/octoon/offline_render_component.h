#ifndef OCTOON_OFFLINE_RENDER_COMPONENT_H_
#define OCTOON_OFFLINE_RENDER_COMPONENT_H_

#include <octoon/video/material.h>
#include <octoon/game_component.h>
#include <octoon/offline_listener.h>

namespace octoon
{
	class OCTOON_EXPORT OfflineRenderComponent : public GameComponent, public OfflineListener
	{
		OctoonDeclareSubInterface(OfflineRenderComponent, GameComponent)
	public:
		OfflineRenderComponent() noexcept;
		OfflineRenderComponent(video::MaterialPtr&& material, bool sharedMaterial = false) noexcept;
		OfflineRenderComponent(const video::MaterialPtr& material, bool sharedMaterial = false) noexcept;
		virtual ~OfflineRenderComponent() noexcept;

		void setMaterial(video::MaterialPtr&& material, bool sharedMaterial = false) noexcept;
		void setMaterial(const video::MaterialPtr& material, bool sharedMaterial = false) noexcept;
		const video::MaterialPtr& getMaterial() const noexcept;

		bool isSharedMaterial() const noexcept;
		void isSharedMaterial(bool sharedMaterial) noexcept;

	protected:
		virtual void onPreRender() noexcept override;
		virtual void onPostRender() noexcept override;

		virtual void onMaterialReplace(const video::MaterialPtr& material) noexcept;

	private:
		OfflineRenderComponent(const OfflineRenderComponent&) = delete;
		OfflineRenderComponent& operator=(const OfflineRenderComponent&) = delete;

	private:
		bool isSharedMaterial_;
		video::MaterialPtr material_;
	};
}

#endif