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
		explicit OfflineRenderComponent(material::Materials&& material, bool sharedMaterial = false) noexcept;
		explicit OfflineRenderComponent(material::MaterialPtr&& material, bool sharedMaterial = false) noexcept;
		explicit OfflineRenderComponent(const material::Materials& material, bool sharedMaterial = false) noexcept;
		explicit OfflineRenderComponent(const material::MaterialPtr& material, bool sharedMaterial = false) noexcept;
		virtual ~OfflineRenderComponent() noexcept;

		void setMaterial(material::MaterialPtr&& material, std::size_t n = 0, bool sharedMaterial = false) noexcept;
		void setMaterial(const material::MaterialPtr& material, std::size_t n = 0, bool sharedMaterial = false) noexcept;
		const material::MaterialPtr& getMaterial(std::size_t n = 0) const noexcept;

		void setMaterials(material::Materials&& material, bool sharedMaterial = false) noexcept;
		void setMaterials(const material::Materials& material, bool sharedMaterial = false) noexcept;
		const material::Materials& getMaterials() const noexcept;

		bool isSharedMaterial() const noexcept;
		void isSharedMaterial(bool sharedMaterial) noexcept;

		std::size_t getNumMaterials() const noexcept;

	protected:
		virtual void onPreRender() noexcept override;
		virtual void onPostRender() noexcept override;

		virtual void onMaterialReplace(const material::Materials& material) noexcept;

	private:
		OfflineRenderComponent(const OfflineRenderComponent&) = delete;
		OfflineRenderComponent& operator=(const OfflineRenderComponent&) = delete;

	private:
		bool isSharedMaterial_;
		material::Materials materials_;
	};
}

#endif