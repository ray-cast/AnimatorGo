#ifndef OCTOON_RENDER_COMPONENT_H_
#define OCTOON_RENDER_COMPONENT_H_

#include <octoon/material/material.h>
#include <octoon/video/render_object.h>
#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT RenderComponent : public GameComponent, public video::RenderListener
	{
		OctoonDeclareSubInterface(RenderComponent, GameComponent)
	public:
		RenderComponent() noexcept;
		explicit RenderComponent(material::Materials&& material, bool sharedMaterial = false) noexcept;
		explicit RenderComponent(material::MaterialPtr&& material, bool sharedMaterial = false) noexcept;
		explicit RenderComponent(const material::Materials& material, bool sharedMaterial = false) noexcept;
		explicit RenderComponent(const material::MaterialPtr& material, bool sharedMaterial = false) noexcept;
		virtual ~RenderComponent() noexcept;

		virtual void setVisible(bool visable) noexcept = 0;
		virtual bool getVisible() const noexcept = 0;

		virtual void setRenderOrder(std::int32_t order) noexcept = 0;
		virtual std::int32_t getRenderOrder() const noexcept = 0;

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
		virtual void onPreRender(const camera::Camera& camera) noexcept override;
		virtual void onPostRender(const camera::Camera& camera) noexcept override;

		virtual void onMaterialReplace(const material::Materials& material) noexcept;

	private:
		RenderComponent(const RenderComponent&) = delete;
		RenderComponent& operator=(const RenderComponent&) = delete;

	private:
		bool isSharedMaterial_;
		material::Materials materials_;
	};
}

#endif