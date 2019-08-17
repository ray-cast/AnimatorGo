#ifndef OCTOON_OFFLINE_RENDER_COMPONENT_H_
#define OCTOON_OFFLINE_RENDER_COMPONENT_H_

#include <octoon/model/material.h>
#include <octoon/game_component.h>
#include <octoon/offline_listener.h>

namespace octoon
{
	class OCTOON_EXPORT OfflineRenderComponent : public GameComponent, public OfflineListener
	{
		OctoonDeclareSubInterface(OfflineRenderComponent, GameComponent)
	public:
		OfflineRenderComponent() noexcept;
		OfflineRenderComponent(model::MaterialPtr&& material, bool sharedMaterial = false) noexcept;
		OfflineRenderComponent(const model::MaterialPtr& material, bool sharedMaterial = false) noexcept;
		virtual ~OfflineRenderComponent() noexcept;

		void setMaterial(model::MaterialPtr&& material, bool sharedMaterial = false) noexcept;
		void setMaterial(const model::MaterialPtr& material, bool sharedMaterial = false) noexcept;
		const model::MaterialPtr& getMaterial() const noexcept;

		bool isSharedMaterial() const noexcept;
		void isSharedMaterial(bool sharedMaterial) noexcept;

	protected:
		virtual void onPreRender() noexcept override;
		virtual void onPostRender() noexcept override;

		virtual void onMaterialReplace(const model::MaterialPtr& material) noexcept;

	private:
		OfflineRenderComponent(const OfflineRenderComponent&) = delete;
		OfflineRenderComponent& operator=(const OfflineRenderComponent&) = delete;

	private:
		bool isSharedMaterial_;
		model::MaterialPtr material_;
	};
}

#endif