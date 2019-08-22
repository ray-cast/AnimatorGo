#ifndef MYSTICLIT_BEHAVIOUR_H_
#define MYSTICLIT_BEHAVIOUR_H_

#include "mysticLit_profile.h"
#include "mysticLit_context.h"

#include "controllers/denoise_component.h"
#include "controllers/file_component.h"
#include "controllers/entities_component.h"
#include "controllers/canvas_component.h"
#include "controllers/offline_component.h"
#include "controllers/player_component.h"
#include "controllers/h264_component.h"
#include "controllers/ui_component.h"

namespace MysticLit
{
	class MysticlitBehaviour final : public octoon::GameComponent
	{
		OctoonDeclareSubClass(MysticlitBehaviour, octoon::GameComponent)
	public:
		MysticlitBehaviour() noexcept;
		~MysticlitBehaviour() noexcept;

		virtual octoon::GameComponentPtr clone() const noexcept override;

		void addComponent(IMysticLitComponent* component) noexcept;
		void removeComponent(const IMysticLitComponent* component) noexcept;
		void enableComponents() noexcept;
		void disableComponents() noexcept;

		IMysticLitComponent*  getComponent(const std::type_info& type) const noexcept;
		template<typename T>
		IMysticLitComponent* getComponent() const noexcept { return this->getComponent(typeid(T)); }

	private:
		void onOpenProject(const octoon::runtime::any& data) noexcept;
		void onSaveProject(const octoon::runtime::any& data) noexcept;
		void onSaveAsProject(const octoon::runtime::any& data) noexcept;

		void onOpenModel(const octoon::runtime::any& data) noexcept;
		void onSaveModel(const octoon::runtime::any& data) noexcept;

		void exit(const octoon::runtime::any& data) noexcept;

		void play(const octoon::runtime::any& data) noexcept;
		void offlineMode(const octoon::runtime::any& data) noexcept;

		void onRecord(const octoon::runtime::any& data) noexcept;
		void onRenderPicture(const octoon::runtime::any& data) noexcept;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onFixedUpdate() noexcept override;

	private:
		std::unique_ptr<MysticLitProfile> profile_;
		std::shared_ptr<MysticLitContext> context_;

		std::unique_ptr<FileComponent> fileComponent_;
		std::unique_ptr<CanvasComponent> canvasComponent_;
		std::unique_ptr<EntitiesComponent> entitiesComponent_;
		std::unique_ptr<OfflineComponent> offlineComponent_;
		std::unique_ptr<PlayerComponent> playerComponent_;
		std::unique_ptr<DenoiseComponent> denoiseComponent_;
		std::unique_ptr<H264Component> h264Component_;
		std::unique_ptr<UIComponent> uiComponent_;

		std::vector<IMysticLitComponent*> components_;
	};
}

#endif