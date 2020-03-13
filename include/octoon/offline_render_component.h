#ifndef OCTOON_OFFLINE_RENDER_COMPONENT_H_
#define OCTOON_OFFLINE_RENDER_COMPONENT_H_

#include <octoon/material/material.h>
#include <octoon/game_component.h>
#include <octoon/offline_listener.h>

namespace octoon
{
	class OCTOON_EXPORT OfflineRenderComponent : public GameComponent, public OfflineListener
	{
		OctoonDeclareSubInterface(OfflineRenderComponent, GameComponent)
	public:
		OfflineRenderComponent() noexcept;
		explicit OfflineRenderComponent(model::Materials&& material, bool sharedMaterial = false) noexcept;
		explicit OfflineRenderComponent(model::MaterialPtr&& material, bool sharedMaterial = false) noexcept;
		explicit OfflineRenderComponent(const model::Materials& material, bool sharedMaterial = false) noexcept;
		explicit OfflineRenderComponent(const model::MaterialPtr& material, bool sharedMaterial = false) noexcept;
		virtual ~OfflineRenderComponent() noexcept;

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
		virtual void onPreRender() noexcept override;
		virtual void onPostRender() noexcept override;

		virtual void onMaterialReplace(const model::Materials& material) noexcept;

	private:
		OfflineRenderComponent(const OfflineRenderComponent&) = delete;
		OfflineRenderComponent& operator=(const OfflineRenderComponent&) = delete;

	private:
		bool isSharedMaterial_;
		model::Materials materials_;
	};
}

#endif